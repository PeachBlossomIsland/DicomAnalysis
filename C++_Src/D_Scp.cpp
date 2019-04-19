#include "pch.h"
#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "D_Scp.h"
#include "dcmtk/dcmnet/assoc.h"
#include "dcmtk/dcmdata/dcostrmf.h" /* for class DcmOutputFileStream */
#include "sharedBuf.h"
#include "global.h"
 // ----------------------------------------------------------------------------

D_DcmSCP::D_DcmSCP() :
	m_assoc(NULL),
	m_cfg()
{
	OFStandard::initializeNetwork();
}

// ----------------------------------------------------------------------------

D_DcmSCP::~D_DcmSCP()
{
	// If there is an open association, drop it and free memory (just to be sure...)
	if (m_assoc)
	{
		dropAndDestroyAssociation();
	}

	OFStandard::shutdownNetwork();
}

// ----------------------------------------------------------------------------

DcmSCPConfig& D_DcmSCP::getConfig()
{
	return *m_cfg;
}

// ----------------------------------------------------------------------------

OFCondition D_DcmSCP::setConfig(const DcmSCPConfig& config)
{
	if (isConnected())
	{
		return NET_EC_AlreadyConnected;
	}
	m_cfg = DcmSharedSCPConfig(config);
	return EC_Normal;
}


// ----------------------------------------------------------------------------

OFCondition D_DcmSCP::listen(SharedBuf& srb)
{
	// make sure not to let dcmdata remove trailing blank padding or perform other
	// manipulations. We want to see the real data.
	dcmEnableAutomaticInputDataCorrection.set(OFFalse);

	OFCondition cond = EC_Normal;
	// Make sure data dictionary is loaded.
	if (!dcmDataDict.isDictionaryLoaded())
	{
		DCMNET_WARN("No data dictionary loaded, check environment variable: " << DCM_DICT_ENVIRONMENT_VARIABLE);
		g_log->error("D_DcmSCP::listen:未加载数据字典，请检查环境变量");
	}

#ifndef DISABLE_PORT_PERMISSION_CHECK
#ifdef HAVE_GETEUID
	// If port is privileged we must be as well.
	if (m_cfg->getPort() < 1024 && geteuid() != 0)
	{
		DCMNET_ERROR("No privileges to open this network port (" << m_cfg->getPort() << ")");
		g_log->error("D_DcmSCP::listen:没有权限打开端口");
		return NET_EC_InsufficientPortPrivileges;
	}
#endif
#endif

	// Check whether current association profile is valid
	OFString tmp;
	OFCondition result = m_cfg->checkAssociationProfile(m_cfg->getActiveAssociationProfile(), tmp);
	if (result.bad())
		return result;

	// Initialize network, i.e. create an instance of T_ASC_Network*.
	T_ASC_Network *network = NULL;
	cond = ASC_initializeNetwork(NET_ACCEPTOR, OFstatic_cast(int, m_cfg->getPort()), m_cfg->getACSETimeout(), &network);
	if (cond.bad())
		return cond;

	// drop root privileges now and revert to the calling user id (if we are running as setuid root)
	cond = OFStandard::dropPrivileges();
	if (cond.bad())
	{
		DCMNET_ERROR("setuid() failed, maximum number of processes/threads for uid already running.");
		g_log->error("D_DcmSCP::listen:setuid()失败，uid的进程/线程数已达到最大值");
		return cond;
	}

	// If we get to this point, the entire initialization process has been completed
	// successfully. Now, we want to start handling all incoming requests. Since
	// this activity is supposed to represent a server process, we do not want to
	// terminate this activity (unless indicated by the stopAfterCurrentAssociation()
	// or stopAfterConnectionTimeout() methods).
	while (cond.good())
	{
		// Wait for an association and handle the requests of
		// the calling applications correspondingly.
		cond = waitForAssociationRQ(network,srb);

		// Check whether we have a timeout
		if (cond == DUL_NOASSOCIATIONREQUEST)
		{
			// If a stop is requested, stop
			if (stopAfterConnectionTimeout())
			{
				cond = NET_EC_StopAfterConnectionTimeout;
				break;
			}
			else
			{
				// stay in loop
				cond = EC_Normal;
			}
		}
		// Stop if SCP is told to stop after association was handled
		else if (stopAfterCurrentAssociation())
		{
			cond = NET_EC_StopAfterAssociation;
			break;
		}
	}

	// Drop the network, i.e. free memory of T_ASC_Network* structure. This call
	// is the counterpart of ASC_initializeNetwork(...) which was called above.
	ASC_dropNetwork(&network);
	network = NULL;

	// return ok
	return cond;
}

// ----------------------------------------------------------------------------

void D_DcmSCP::findPresentationContext(const T_ASC_PresentationContextID presID,
	OFString &abstractSyntax,
	OFString &transferSyntax)
{
	transferSyntax.clear();
	abstractSyntax.clear();
	if (m_assoc == NULL)
		return;

	DUL_PRESENTATIONCONTEXT *pc;
	LST_HEAD **l;

	/* we look for a presentation context matching
	 * both abstract and transfer syntax
	 */
	l = &m_assoc->params->DULparams.acceptedPresentationContext;
	pc = (DUL_PRESENTATIONCONTEXT*)LST_Head(l);
	(void)LST_Position(l, (LST_NODE*)pc);
	while (pc)
	{
		if (presID == pc->presentationContextID)
		{
			if (pc->result == ASC_P_ACCEPTANCE)
			{
				// found a match
				transferSyntax = pc->acceptedTransferSyntax;
				abstractSyntax = pc->abstractSyntax;
			}
			break;
		}
		pc = (DUL_PRESENTATIONCONTEXT*)LST_Next(l);
	}
}


DUL_PRESENTATIONCONTEXT* D_DcmSCP::findPresentationContextID(LST_HEAD *head,
	T_ASC_PresentationContextID presentationContextID)
{
	DUL_PRESENTATIONCONTEXT *pc;
	LST_HEAD **l;
	OFBool found = OFFalse;

	if (head == NULL)
		return NULL;

	l = &head;
	if (*l == NULL)
		return NULL;

	pc = (DUL_PRESENTATIONCONTEXT*)LST_Head(l);
	(void)LST_Position(l, (LST_NODE*)pc);

	while (pc && !found)
	{
		if (pc->presentationContextID == presentationContextID)
		{
			found = OFTrue;
		}
		else
		{
			pc = (DUL_PRESENTATIONCONTEXT*)LST_Next(l);
		}
	}
	return pc;
}

// ----------------------------------------------------------------------------

void D_DcmSCP::refuseAssociation(const DcmRefuseReasonType reason)
{
	if (m_assoc == NULL)
	{
		DCMNET_WARN("D_DcmSCP::refuseAssociation() called but actually no association running, ignoring");
		g_log->info("D_DcmSCP::refuseAssociation:调用了d_dcmscp::refuseAssociation（），但实际上没有运行任何关联，忽略");
		return;
	}

	T_ASC_RejectParameters rej;

	// dump some information if required
	switch (reason)
	{
	case DCMSCP_TOO_MANY_ASSOCIATIONS:
		DCMNET_INFO("Refusing Association (too many associations)");
		g_log->info("D_DcmSCP::refuseAssociation:拒绝关联（关联太多）");
		break;
	case DCMSCP_CANNOT_FORK:
		DCMNET_INFO("Refusing Association (cannot fork)");
		g_log->info("D_DcmSCP::refuseAssociation:拒绝关联（cannot fork）");
		break;
	case DCMSCP_BAD_APPLICATION_CONTEXT_NAME:
		DCMNET_INFO("Refusing Association (bad application context)");
		g_log->info("D_DcmSCP::refuseAssociation:拒绝关联(不合适的应用上下文)");
		break;
	case DCMSCP_CALLING_HOST_NOT_ALLOWED:
		DCMNET_INFO("Refusing Association (connecting host not allowed)");
		g_log->info("D_DcmSCP::refuseAssociation:拒绝关联(连接对方被拒)");
		break;
	case DCMSCP_CALLED_AE_TITLE_NOT_RECOGNIZED:
		DCMNET_INFO("Refusing Association (called AE title not recognized)");
		g_log->info("D_DcmSCP::refuseAssociation:拒绝关联(called AE没有识别)");
		break;
	case DCMSCP_CALLING_AE_TITLE_NOT_RECOGNIZED:
		DCMNET_INFO("Refusing Association (calling AE title not recognized)");
		g_log->info("D_DcmSCP::refuseAssociation:拒绝关联(called AE没有识别)");
		break;
	case DCMSCP_FORCED:
		DCMNET_INFO("Refusing Association (forced via command line)");
		g_log->info("D_DcmSCP::refuseAssociation:拒绝关联(forced via command line)");
		break;
	case DCMSCP_NO_IMPLEMENTATION_CLASS_UID:
		DCMNET_INFO("Refusing Association (no implementation class UID provided)");
		g_log->info("D_DcmSCP::refuseAssociation:拒绝关联(未提供实现类uid)");
		break;
	case DCMSCP_NO_PRESENTATION_CONTEXTS:
		DCMNET_INFO("Refusing Association (no acceptable presentation contexts)");
		g_log->info("D_DcmSCP::refuseAssociation:拒绝关联(no acceptable presentation contexts)");
		break;
	case DCMSCP_INTERNAL_ERROR:
		DCMNET_INFO("Refusing Association (internal error)");
		g_log->info("D_DcmSCP::refuseAssociation:拒绝关联(internal error)");
		break;
	default:
		DCMNET_INFO("Refusing Association (unknown reason)");
		g_log->info("D_DcmSCP::refuseAssociation:拒绝关联(unknown reason)");
		break;
	}

	// Set some values in the reject message depending on the reason
	switch (reason)
	{
	case DCMSCP_TOO_MANY_ASSOCIATIONS:
		rej.result = ASC_RESULT_REJECTEDTRANSIENT;
		rej.source = ASC_SOURCE_SERVICEPROVIDER_PRESENTATION_RELATED;
		rej.reason = ASC_REASON_SP_PRES_LOCALLIMITEXCEEDED;
		break;
	case DCMSCP_CANNOT_FORK:
		rej.result = ASC_RESULT_REJECTEDPERMANENT;
		rej.source = ASC_SOURCE_SERVICEPROVIDER_PRESENTATION_RELATED;
		rej.reason = ASC_REASON_SP_PRES_TEMPORARYCONGESTION;
		break;
	case DCMSCP_BAD_APPLICATION_CONTEXT_NAME:
		rej.result = ASC_RESULT_REJECTEDTRANSIENT;
		rej.source = ASC_SOURCE_SERVICEUSER;
		rej.reason = ASC_REASON_SU_APPCONTEXTNAMENOTSUPPORTED;
		break;
	case DCMSCP_CALLED_AE_TITLE_NOT_RECOGNIZED:
		rej.result = ASC_RESULT_REJECTEDPERMANENT;
		rej.source = ASC_SOURCE_SERVICEUSER;
		rej.reason = ASC_REASON_SU_CALLEDAETITLENOTRECOGNIZED;
		break;
	case DCMSCP_CALLING_AE_TITLE_NOT_RECOGNIZED:
		rej.result = ASC_RESULT_REJECTEDPERMANENT;
		rej.source = ASC_SOURCE_SERVICEUSER;
		rej.reason = ASC_REASON_SU_CALLINGAETITLENOTRECOGNIZED;
		break;
	case DCMSCP_FORCED:
	case DCMSCP_NO_IMPLEMENTATION_CLASS_UID:
	case DCMSCP_NO_PRESENTATION_CONTEXTS:
	case DCMSCP_CALLING_HOST_NOT_ALLOWED:
	case DCMSCP_INTERNAL_ERROR:
	default:
		rej.result = ASC_RESULT_REJECTEDPERMANENT;
		rej.source = ASC_SOURCE_SERVICEUSER;
		rej.reason = ASC_REASON_SU_NOREASON;
		break;
	}

	// Reject the association request.
	ASC_rejectAssociation(m_assoc, &rej);
}

// ----------------------------------------------------------------------------

OFCondition D_DcmSCP::waitForAssociationRQ(T_ASC_Network *network,SharedBuf& srb)
{
	if (network == NULL)
		return ASC_NULLKEY;
	if (m_assoc != NULL)
		return DIMSE_ILLEGALASSOCIATION;

	Uint32 timeout = m_cfg->getConnectionTimeout();

	// Listen to a socket for timeout seconds and wait for an association request
	OFCondition cond = ASC_receiveAssociation(network, &m_assoc, m_cfg->getMaxReceivePDULength(), NULL, NULL, OFFalse,
		m_cfg->getConnectionBlockingMode(), OFstatic_cast(int, timeout));

	// In case of a timeout in non-blocking mode, call notifier (and return
	// to main event loop later)
	if (cond == DUL_NOASSOCIATIONREQUEST)
	{
		notifyConnectionTimeout();
	}
	else
	{
		// If association could be received, handle it
		if (cond.good())
		{
			cond = processAssociationRQ(srb);
			// There was an association which has ended now:
			// Call notifier and output separator line.
			notifyAssociationTermination();
			DCMNET_DEBUG("+++++++++++++++++++++++++++++");
			g_log->info("D_DcmSCP::waitForAssociationRQ:");
		}
		// Else, if we could not receive an association request since there was
		// some error, just ignore it (and continue in main event loop later)
		else
		{
			DCMNET_ERROR("Could not receive association request: " << cond.text());
			g_log->error("D_DcmSCP::waitForAssociationRQ:无法接收连接请求:{}",(std::string)cond.text());
			cond = EC_Normal;
		}
	}

	// We are done with this association, free it and set to NULL.
	// ASC_receiveAssociation will always create a related structure, even
	// if no association was received at all.
	dropAndDestroyAssociation();
	return cond;
}


OFCondition D_DcmSCP::processAssociationRQ(SharedBuf& srb)
{
	DcmSCPActionType desiredAction = DCMSCP_ACTION_UNDEFINED;
	if ((m_assoc == NULL) || (m_assoc->params == NULL))
		return ASC_NULLKEY;

	// call notifier function
	notifyAssociationRequest(*m_assoc->params, desiredAction);
	if (desiredAction != DCMSCP_ACTION_UNDEFINED)
	{
		if (desiredAction == DCMSCP_ACTION_REFUSE_ASSOCIATION)
		{
			refuseAssociation(DCMSCP_INTERNAL_ERROR);
			return EC_Normal;
		}
		else desiredAction = DCMSCP_ACTION_UNDEFINED; // reset for later use
	}

	// Now we have to figure out if we might have to refuse the association request.
	// This is the case if at least one of five conditions is met:

	// Condition 1: if option "--refuse" is set we want to refuse the association request.
	if (m_cfg->getRefuseAssociation())
	{
		refuseAssociation(DCMSCP_FORCED);
		return EC_Normal;
	}

	// Condition 2: determine the application context name. If an error occurred or if the
	// application context name is not supported we want to refuse the association request.
	char buf[BUFSIZ];
	OFCondition cond = ASC_getApplicationContextName(m_assoc->params, buf, sizeof(buf));
	if (cond.bad() || strcmp(buf, DICOM_STDAPPLICATIONCONTEXT) != 0)
	{
		refuseAssociation(DCMSCP_BAD_APPLICATION_CONTEXT_NAME);
		return EC_Normal;
	}

	// Condition 3: if the calling host is not supported, we want to refuse
	// the association request
	if (!checkCallingHostAccepted(m_assoc->params->DULparams.calledPresentationAddress))
	{
		refuseAssociation(DCMSCP_CALLING_HOST_NOT_ALLOWED);
		return EC_Normal;
	}

	// Condition 4: if the calling or called application entity title is not supported
	// we want to refuse the association request
	if (!checkCalledAETitleAccepted(m_assoc->params->DULparams.calledAPTitle))
	{
		refuseAssociation(DCMSCP_CALLED_AE_TITLE_NOT_RECOGNIZED);
		return EC_Normal;
	}

	if (!checkCallingAETitleAccepted(m_assoc->params->DULparams.callingAPTitle))
	{
		refuseAssociation(DCMSCP_CALLING_AE_TITLE_NOT_RECOGNIZED);
		return EC_Normal;
	}

	/* set our application entity title */
	if (m_cfg->getRespondWithCalledAETitle())
		ASC_setAPTitles(m_assoc->params, NULL, NULL, m_assoc->params->DULparams.calledAPTitle);
	else
		ASC_setAPTitles(m_assoc->params, NULL, NULL, m_cfg->getAETitle().c_str());

	/* If we get to this point the association shall be negotiated.
	   Thus, for every presentation context it is checked whether
	   it can be accepted. However, this is only a "dry" run, i.e.
	   there is not yet sent a response message to the SCU
	 */
	cond = negotiateAssociation();
	if (cond.bad())
	{
		return EC_Normal;
	}

	// Reject association if no presentation context was negotiated
	if (ASC_countAcceptedPresentationContexts(m_assoc->params) == 0)
	{
		// Dump some debug information
		OFString tempStr;
		DCMNET_INFO("No Acceptable Presentation Contexts");
		g_log->info("D_DcmSCP::processAssociationRQ:No Acceptable Presentation Contexts");
		if (m_cfg->getVerbosePCMode())
		{
			DCMNET_INFO(ASC_dumpParameters(tempStr, m_assoc->params, ASC_ASSOC_RJ));
			g_log->info("D_DcmSCP::processAssociationRQ:{}",(std::string)ASC_dumpParameters(tempStr, m_assoc->params, ASC_ASSOC_RJ).c_str());
		}
		else
		{
			DCMNET_DEBUG(ASC_dumpParameters(tempStr, m_assoc->params, ASC_ASSOC_RJ));
			g_log->debug("D_DcmSCP::processAssociationRQ:{}",(std::string)ASC_dumpParameters(tempStr, m_assoc->params, ASC_ASSOC_RJ).c_str());
		}
		refuseAssociation(DCMSCP_NO_PRESENTATION_CONTEXTS);
		return EC_Normal;
	}

	// If the negotiation was successful, accept the association request
	cond = ASC_acknowledgeAssociation(m_assoc);
	if (cond.bad())
	{
		return EC_Normal;
	}
	notifyAssociationAcknowledge();

	// Dump some debug information
	OFString tempStr;
	DCMNET_INFO("Association Acknowledged (Max Send PDV: " << OFstatic_cast(Uint32, m_assoc->sendPDVLength) << ")");
	g_log->info("D_DcmSCP::processAssociationRQ:Association Acknowledged (Max Send PDV:{})", std::to_string(OFstatic_cast(Uint32, m_assoc->sendPDVLength)));
	if (m_cfg->getVerbosePCMode())
	{
		DCMNET_INFO(ASC_dumpParameters(tempStr, m_assoc->params, ASC_ASSOC_AC));
		g_log->info("D_DcmSCP::processAssociationRQ:{}", (std::string)ASC_dumpParameters(tempStr, m_assoc->params, ASC_ASSOC_AC).c_str());
	}
	else
	{
		DCMNET_DEBUG(ASC_dumpParameters(tempStr, m_assoc->params, ASC_ASSOC_AC));
		g_log->debug("D_DcmSCP::processAssociationRQ:{}", (std::string)ASC_dumpParameters(tempStr, m_assoc->params, ASC_ASSOC_AC).c_str());
	}
	// Go ahead and handle the association (i.e. handle the caller's requests) in this process
	handleAssociation(srb);

	return EC_Normal;
}

// ----------------------------------------------------------------------------

OFCondition D_DcmSCP::negotiateAssociation()
{
	// Check whether there is something to negotiate...
	if (m_assoc == NULL)
		return DIMSE_ILLEGALASSOCIATION;

	// Set presentation contexts as defined in association configuration
	OFCondition result = m_cfg->evaluateIncomingAssociation(*m_assoc);
	if (result.bad())
	{
		OFString tempStr;
		DCMNET_ERROR(DimseCondition::dump(tempStr, result));
		g_log->error("D_DcmSCP::negotiateAssociation:{}", (std::string)DimseCondition::dump(tempStr, result).c_str());
	}
	return result;
}

// ----------------------------------------------------------------------------

OFCondition D_DcmSCP::abortAssociation()
{
	OFCondition cond = DIMSE_ILLEGALASSOCIATION;
	// Check whether there is an active association
	if (isConnected())
	{
		// Abort current association
		DCMNET_INFO("Aborting Association (initiated by SCP)");
		g_log->info("D_DcmSCP::abortAssociation:Aborting Association (initiated by SCP)");
		cond = ASC_abortAssociation(m_assoc);
		// Notify user in case of error
		if (cond.bad())
		{
			OFString tempStr;
			DCMNET_ERROR("Association Abort Failed: " << DimseCondition::dump(tempStr, cond));
			g_log->error("D_DcmSCP::abortAssociation:Association Abort Failed:{}", (std::string)DimseCondition::dump(tempStr, cond).c_str());
		}
		// Note: association is dropped and memory freed somewhere else
	}
	else
	{
		DCMNET_WARN("DcmSCP::abortAssociation() called but SCP actually has no association running, ignoring");
		g_log->info("D_DcmSCP::abortAssociation:DcmSCP::abortAssociation() called but SCP actually has no association running, ignoring");
	}
	return cond;
}

// ----------------------------------------------------------------------------

void D_DcmSCP::handleAssociation(SharedBuf& srb)
{
	if (m_assoc == NULL)
	{
		DCMNET_WARN("D_DcmSCP::handleAssociation() called but SCP actually has no association running, ignoring");
		g_log->debug("D_DcmSCP::handleAssociation:D_DcmSCP::handleAssociation() called but SCP actually has no association running, ignoring");
		return;
	}

	// Receive a DIMSE command and perform all the necessary actions. (Note that ReceiveAndHandleCommands()
	// will always return a value 'cond' for which 'cond.bad()' will be true. This value indicates that either
	// some kind of error occurred, or that the peer aborted the association (DUL_PEERABORTEDASSOCIATION),
	// or that the peer requested the release of the association (DUL_PEERREQUESTEDRELEASE).) (Also note
	// that ReceiveAndHandleCommands() will never return EC_Normal.)
	OFCondition cond = EC_Normal;
	T_DIMSE_Message message;
	T_ASC_PresentationContextID presID;

	// start a loop to be able to receive more than one DIMSE command
	while (cond.good())
	{
		// receive a DIMSE command over the network
		cond = DIMSE_receiveCommand(m_assoc, m_cfg->getDIMSEBlockingMode(), m_cfg->getDIMSETimeout(),
			&presID, &message, NULL);

		// check if peer did release or abort, or if we have a valid message
		if (cond.good())
		{
			DcmPresentationContextInfo presInfo;
			getPresentationContextInfo(m_assoc, presID, presInfo);
			cond = handleIncomingCommand(&message, presInfo,srb);
		}
	}
	// Clean up on association termination.
	if (cond == DUL_PEERREQUESTEDRELEASE)
	{
		notifyReleaseRequest();
		ASC_acknowledgeRelease(m_assoc);
	}
	else if (cond == DUL_PEERABORTEDASSOCIATION)
	{
		notifyAbortRequest();
	}
	else
	{
		notifyDIMSEError(cond);
		ASC_abortAssociation(m_assoc);
	}
}

// ----------------------------------------------------------------------------

OFCondition D_DcmSCP::handleIncomingCommand(T_DIMSE_Message *incomingMsg,
	const DcmPresentationContextInfo &presInfo,SharedBuf& srb)
{
	OFCondition cond;
	// Handle C-ECHO for Verification SOP Class
	if ((incomingMsg->CommandField == DIMSE_C_ECHO_RQ)
		&& (presInfo.abstractSyntax == UID_VerificationSOPClass))
	{
		// Process C-ECHO request
		cond = handleECHORequest(incomingMsg->msg.CEchoRQ, presInfo.presentationContextID);
	}
	else {
		// We cannot handle this kind of message. Note that the condition will be returned
		// and that the caller is responsible to end the association if desired.
		OFString tempStr;
		DCMNET_ERROR("Cannot handle this kind of DIMSE command (0x"
			<< STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(4)
			<< OFstatic_cast(unsigned int, incomingMsg->CommandField) << ")");
		DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, *incomingMsg, DIMSE_INCOMING));
		cond = DIMSE_BADCOMMANDTYPE;
	}

	// return result
	return cond;
}

// ----------------------------------------------------------------------------

// -- C-ECHO -- 此服务在此项目中不使用

OFCondition D_DcmSCP::handleECHORequest(T_DIMSE_C_EchoRQ &reqMessage,
	const T_ASC_PresentationContextID presID)
{
	OFCondition cond;
	OFString tempStr;

	// Dump debug information
	if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
	{
		DCMNET_INFO("Received C-ECHO Request");
		DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));
		DCMNET_INFO("Sending C-ECHO Response");
	}
	else {
		DCMNET_INFO("Received C-ECHO Request (MsgID " << reqMessage.MessageID << ")");
		DCMNET_INFO("Sending C-ECHO Response (" << DU_cechoStatusString(STATUS_Success) << ")");
	}

	// Send response message
	cond = DIMSE_sendEchoResponse(m_assoc, presID, &reqMessage, STATUS_Success, NULL);
	if (cond.bad())
		DCMNET_ERROR("Cannot send C-ECHO Response: " << DimseCondition::dump(tempStr, cond));
	else
		DCMNET_DEBUG("C-ECHO Response successfully sent");

	return cond;
}

// ----------------------------------------------------------------------------

// -- C-STORE --

OFCondition D_DcmSCP::handleSTORERequest(T_DIMSE_C_StoreRQ &reqMessage,
	const T_ASC_PresentationContextID presID,
	DcmDataset *&reqDataset)
{
	// First, receive the C-STORE request
	OFCondition cond = receiveSTORERequest(reqMessage, presID, reqDataset);

	if (cond.good())
	{
		// Then, check the request message and dataset and return an DIMSE status code
		const Uint16 rspStatusCode = checkSTORERequest(reqMessage, reqDataset);
		// ... that is sent back with the C-STORE response message
		cond = sendSTOREResponse(presID, reqMessage, rspStatusCode);
	}

	return cond;
}


OFCondition D_DcmSCP::receiveSTORERequest(T_DIMSE_C_StoreRQ &reqMessage,
	const T_ASC_PresentationContextID presID,
	DcmDataset *&reqDataset)
{
	// Do some basic validity checks
	if (m_assoc == NULL)
		return DIMSE_ILLEGALASSOCIATION;

	OFCondition cond;
	OFString tempStr;
	T_ASC_PresentationContextID presIDdset;
	// Remember the passed dataset pointer
	DcmDataset *dataset = reqDataset;

	// Dump debug information
	if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
	{
		DCMNET_INFO("Received C-STORE Request");
		g_log->info("D_DcmSCP::receiveSTORERequest:Received C-STORE Request");
	}
	else
	{
		DCMNET_INFO("Received C-STORE Request (MsgID " << reqMessage.MessageID << ")");
		g_log->info("D_DcmSCP::receiveSTORERequest:Received C-STORE Request (MsgID {})",std::to_string(reqMessage.MessageID));
	}
	// Check if dataset is announced correctly
	if (reqMessage.DataSetType == DIMSE_DATASET_NULL)
	{
		DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));
		g_log->debug("D_DcmSCP::receiveSTORERequest:{}",(std::string)DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID).c_str());
		DCMNET_ERROR("Received C-STORE request but no dataset announced, aborting");
		g_log->error("D_DcmSCP::receiveSTORERequest:Received C-STORE request but no dataset announced, aborting");
		return DIMSE_BADMESSAGE;
	}

	// Receive dataset (in memory)
	cond = receiveDIMSEDataset(&presIDdset, &dataset);
	if (cond.bad())
	{
		DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));
		g_log->debug("D_DcmSCP::receiveSTORERequest:{}",(std::string)DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID).c_str());
		DCMNET_ERROR("Unable to receive C-STORE dataset on presentation context " << OFstatic_cast(unsigned int, presID));
		g_log->error("D_DcmSCP::receiveSTORERequest:Unable to receive C-STORE dataset on presentation context :{}",std::to_string(OFstatic_cast(unsigned int, presID)));
		return cond;
	}

	// Output request message only if trace level is enabled
	if (DCM_dcmnetLogger.isEnabledFor(OFLogger::TRACE_LOG_LEVEL))
	{
		DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, dataset, presID));
		g_log->debug("D_DcmSCP::receiveSTORERequest:{}",(std::string)DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, dataset, presID).c_str());
	}
	else
	{
		DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));
		g_log->debug("D_DcmSCP::receiveSTORERequest:{}", (std::string)DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID).c_str());
	}
	// Compare presentation context ID of command and data set
	if (presIDdset != presID)
	{
		DCMNET_ERROR("Presentation Context ID of command (" << OFstatic_cast(unsigned int, presID)
			<< ") and data set (" << OFstatic_cast(unsigned int, presIDdset) << ") differs");
		g_log->error("D_DcmSCP::receiveSTORERequest:Presentation Context ID of command ({})and data set ({})differs", OFstatic_cast(unsigned int, presID), OFstatic_cast(unsigned int, presIDdset));
		if (dataset != reqDataset)
		{
			// Free memory allocated by receiveDIMSEDataset()
			delete dataset;
		}
		return makeDcmnetCondition(DIMSEC_INVALIDPRESENTATIONCONTEXTID, OF_error,
			"DIMSE: Presentation Contexts of Command and Data Set differ");
	}

	// Set return value
	reqDataset = dataset;

	return cond;
}


OFCondition D_DcmSCP::receiveSTORERequest(T_DIMSE_C_StoreRQ &reqMessage,
	const T_ASC_PresentationContextID presID,
	const OFString &filename,SharedBuf& srb)
{
	// Do some basic validity checks
	if (m_assoc == NULL)
		return DIMSE_ILLEGALASSOCIATION;

	OFCondition cond;
	OFString tempStr;
	// Use presentation context ID of the command set as a default
	T_ASC_PresentationContextID presIDdset = presID;

	// Dump debug information
	if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
	{
		DCMNET_INFO("Received C-STORE Request");
		g_log->info("D_DcmSCP::receiveSTORERequest:Received C-STORE Request");
	}
	else
	{
		DCMNET_INFO("Received C-STORE Request (MsgID " << reqMessage.MessageID << ")");
		g_log->info("D_DcmSCP::receiveSTORERequest:Received C-STORE Request (MsgID :{})",std::to_string(reqMessage.MessageID));
	}
	// Check if dataset is announced correctly
	if (reqMessage.DataSetType == DIMSE_DATASET_NULL)
	{
		DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));
		g_log->debug("D_DcmSCP::receiveSTORERequest:Received C-STORE request but no dataset announced, aborting:{}",(std::string)DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID).c_str());
		DCMNET_ERROR("Received C-STORE request but no dataset announced, aborting");
		return DIMSE_BADMESSAGE;
	}
	// Receive dataset (directly to file)
	//确保存储文件的过程是原子性的
	std::unique_lock<std::mutex> lock(srb.mtx);
	g_log->info("D_DcmSCP::receiveSTORERequest:store file lock");
	cond = receiveSTORERequestDataset(&presIDdset, reqMessage, filename);
	if (cond.bad())
	{
		DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));
		DCMNET_ERROR("Unable to receive C-STORE dataset on presentation context " << OFstatic_cast(unsigned int, presID));
		g_log->debug("D_DcmSCP::receiveSTORERequest:Unable to receive C-STORE dataset on presentation context:{}:{}", OFstatic_cast(unsigned int, presID),(std::string)DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID).c_str());
		//出现异常，解锁，避免死锁
		lock.unlock();
		return cond;
	}
	g_log->info("D_DcmSCP::receiveSTORERequest:queue size:{}", srb.q.size());
	while (srb.q.size() == srb.size) //队列已满
	{
		std::cout << "receiveSTORERequest is waiting for an empty queueu...\n";
		g_log->info("D_DcmSCP::receiveSTORERequest:receiveSTORERequest is waiting for an empty queueu....");
		srb.not_full.wait(lock);
	}
	DCMNET_INFO("正在存储文件名:%s",filename);//测试用语
	g_log->info("D_DcmSCP::receiveSTORERequest:正在存储文件名:{}",(std::string)filename.c_str());
	srb.q.push((std::string)filename.c_str());
	srb.not_empty.notify_all();
	g_log->info("D_DcmSCP::receiveSTORERequest:unlock");
	lock.unlock();
	//实时地将日志写入日志文件
	g_log->flush();
	// Output request message only if trace level is enabled
	DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));
	g_log->debug("D_DcmSCP::receiveSTORERequest:{}",(std::string)DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID).c_str());
	// Compare presentation context ID of command and data set
	if (presIDdset != presID)
	{
		DCMNET_ERROR("Presentation Context ID of command (" << OFstatic_cast(unsigned int, presID)
			<< ") and data set (" << OFstatic_cast(unsigned int, presIDdset) << ") differs");
		g_log->error("D_DcmSCP::receiveSTORERequest:Presentation Context ID of command ({})and data set({})", OFstatic_cast(unsigned int, presID), OFstatic_cast(unsigned int, presIDdset));
		return makeDcmnetCondition(DIMSEC_INVALIDPRESENTATIONCONTEXTID, OF_error,
			"DIMSE: Presentation Contexts of Command and Data Set differ");
	}

	return cond;
}


OFCondition D_DcmSCP::sendSTOREResponse(const T_ASC_PresentationContextID presID,
	const T_DIMSE_C_StoreRQ &reqMessage,
	const Uint16 rspStatusCode)
{
	// Call the method doing the real work
	return sendSTOREResponse(presID, reqMessage.MessageID, reqMessage.AffectedSOPClassUID, reqMessage.AffectedSOPInstanceUID,
		rspStatusCode, NULL /* statusDetail */);
}


OFCondition D_DcmSCP::sendSTOREResponse(const T_ASC_PresentationContextID presID,
	const Uint16 messageID,
	const OFString &sopClassUID,
	const OFString &sopInstanceUID,
	const Uint16 rspStatusCode,
	DcmDataset *statusDetail)
{
	OFCondition cond;
	OFString tempStr;

	// Send back response
	T_DIMSE_Message response;
	// Make sure everything is zeroed (especially options)
	bzero((char*)&response, sizeof(response));
	T_DIMSE_C_StoreRSP &storeRsp = response.msg.CStoreRSP;
	response.CommandField = DIMSE_C_STORE_RSP;
	storeRsp.MessageIDBeingRespondedTo = messageID;
	storeRsp.DimseStatus = rspStatusCode;
	storeRsp.DataSetType = DIMSE_DATASET_NULL;
	// Always send the optional fields "Affected SOP Class UID" and "Affected SOP Instance UID"
	storeRsp.opts = O_STORE_AFFECTEDSOPCLASSUID | O_STORE_AFFECTEDSOPINSTANCEUID;
	OFStandard::strlcpy(storeRsp.AffectedSOPClassUID, sopClassUID.c_str(), sizeof(storeRsp.AffectedSOPClassUID));
	OFStandard::strlcpy(storeRsp.AffectedSOPInstanceUID, sopInstanceUID.c_str(), sizeof(storeRsp.AffectedSOPInstanceUID));

	if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
	{
		DCMNET_INFO("Sending C-STORE Response");
		g_log->info("D_DcmSCP::sendSTOREResponse:Sending C-STORE Response:{}", (std::string)DIMSE_dumpMessage(tempStr, response, DIMSE_OUTGOING, NULL, presID).c_str());
		DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, response, DIMSE_OUTGOING, NULL, presID));
	}
	else {
		DCMNET_INFO("Sending C-STORE Response (" << DU_cstoreStatusString(rspStatusCode) << ")");
		g_log->info("D_DcmSCP::sendSTOREResponse:Sending C-STORE Response ({})", (std::string)DU_cstoreStatusString(rspStatusCode));
	}

	// Send response message
	cond = sendDIMSEMessage(presID, &response, NULL /* dataObject */, statusDetail);
	if (cond.bad())
	{
		DCMNET_ERROR("Failed sending C-STORE response: " << DimseCondition::dump(tempStr, cond));
		g_log->error("D_DcmSCP::sendSTOREResponse:Failed sending C-STORE response {}",(std::string)DimseCondition::dump(tempStr, cond).c_str());
	}

	return cond;
}


Uint16 D_DcmSCP::checkSTORERequest(T_DIMSE_C_StoreRQ & /*reqMessage*/,
	DcmDataset * /*reqDataset*/)
{
	// we default to success
	return STATUS_Success;
}

// ----------------------------------------------------------------------------

// -- C-FIND --

OFCondition D_DcmSCP::receiveFINDRequest(T_DIMSE_C_FindRQ &reqMessage,
	const T_ASC_PresentationContextID presID,
	DcmDataset *&reqDataset)
{
	// Do some basic validity checks
	if (m_assoc == NULL)
		return DIMSE_ILLEGALASSOCIATION;

	OFCondition cond;
	OFString tempStr;
	T_ASC_PresentationContextID presIDdset;
	DcmDataset *dataset = NULL;

	// Dump debug information
	if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
		DCMNET_INFO("Received C-FIND Request");
	else
		DCMNET_INFO("Received C-FIND Request (MsgID " << reqMessage.MessageID << ")");

	// Check if dataset is announced correctly
	if (reqMessage.DataSetType == DIMSE_DATASET_NULL)
	{
		DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));
		DCMNET_ERROR("Received C-FIND request but no dataset announced, aborting");
		return DIMSE_BADMESSAGE;
	}

	// Receive dataset
	cond = receiveDIMSEDataset(&presIDdset, &dataset);
	if (cond.bad())
	{
		DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));
		DCMNET_ERROR("Unable to receive C-FIND dataset on presentation context " << OFstatic_cast(unsigned int, presID));
		return DIMSE_BADDATA;
	}

	// Output request message only if trace level is enabled
	if (DCM_dcmnetLogger.isEnabledFor(OFLogger::TRACE_LOG_LEVEL))
		DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, dataset, presID));
	else
		DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));

	// Compare presentation context ID of command and data set
	if (presIDdset != presID)
	{
		DCMNET_ERROR("Presentation Context ID of command (" << OFstatic_cast(unsigned int, presID)
			<< ") and data set (" << OFstatic_cast(unsigned int, presIDdset) << ") differs");
		delete dataset;
		return makeDcmnetCondition(DIMSEC_INVALIDPRESENTATIONCONTEXTID, OF_error,
			"DIMSE: Presentation Contexts of Command and Data Set differ");
	}

	// Set return value
	reqDataset = dataset;

	return cond;
}


OFCondition D_DcmSCP::sendFINDResponse(const T_ASC_PresentationContextID presID,
	const Uint16 messageID,
	const OFString &sopClassUID,
	DcmDataset *rspDataset,
	const Uint16 rspStatusCode,
	DcmDataset* statusDetail)
{
	OFCondition cond;
	OFString tempStr;

	// Send back response
	T_DIMSE_Message response;
	// Make sure everything is zeroed (especially options)
	bzero((char*)&response, sizeof(response));
	T_DIMSE_C_FindRSP &findRsp = response.msg.CFindRSP;
	response.CommandField = DIMSE_C_FIND_RSP;
	findRsp.MessageIDBeingRespondedTo = messageID;
	findRsp.DimseStatus = rspStatusCode;
	// Always send (the optional) field "Affected SOP Class UID"
	findRsp.opts = O_FIND_AFFECTEDSOPCLASSUID;
	OFStandard::strlcpy(findRsp.AffectedSOPClassUID, sopClassUID.c_str(), sizeof(findRsp.AffectedSOPClassUID));

	if (rspDataset)
		findRsp.DataSetType = DIMSE_DATASET_PRESENT;
	else
		findRsp.DataSetType = DIMSE_DATASET_NULL;

	if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
	{
		DCMNET_INFO("Sending C-FIND Response");
		DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, response, DIMSE_OUTGOING, rspDataset, presID));
	}
	else {
		DCMNET_INFO("Sending C-FIND Response (" << DU_cfindStatusString(rspStatusCode) << ")");
	}

	// Send response message with dataset
	cond = sendDIMSEMessage(presID, &response, rspDataset /* dataObject */, statusDetail);
	if (cond.bad())
	{
		DCMNET_ERROR("Failed sending C-FIND response: " << DimseCondition::dump(tempStr, cond));
		return cond;
	}
	return cond;
}


OFCondition D_DcmSCP::checkForCANCEL(T_ASC_PresentationContextID presID,
	const Uint16 messageID)
{
	return DIMSE_checkForCancelRQ(m_assoc, presID, messageID);
}

// ----------------------------------------------------------------------------

// -- C-MOVE --

OFCondition D_DcmSCP::receiveMOVERequest(T_DIMSE_C_MoveRQ &reqMessage,
	const T_ASC_PresentationContextID presID,
	DcmDataset *&reqDataset,
	OFString &moveDest)
{
	// Do some basic validity checks
	if (m_assoc == NULL)
		return DIMSE_ILLEGALASSOCIATION;

	OFCondition cond;
	OFString tempStr;
	T_ASC_PresentationContextID presIDdset;
	DcmDataset *dataset = NULL;

	// Dump debug information
	if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
		DCMNET_INFO("Received C-MOVE Request");
	else
		DCMNET_INFO("Received C-MOVE Request (MsgID " << reqMessage.MessageID << ")");

	// Check if dataset is announced correctly
	if (reqMessage.DataSetType == DIMSE_DATASET_NULL)
	{
		DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));
		DCMNET_ERROR("Received C-MOVE request but no dataset announced, aborting");
		return DIMSE_BADMESSAGE;
	}

	// Receive dataset
	cond = receiveDIMSEDataset(&presIDdset, &dataset);
	if (cond.bad())
	{
		DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));
		DCMNET_ERROR("Unable to receive C-MOVE dataset on presentation context " << OFstatic_cast(unsigned int, presID));
		return DIMSE_BADDATA;
	}

	// Output request message only if trace level is enabled
	if (DCM_dcmnetLogger.isEnabledFor(OFLogger::TRACE_LOG_LEVEL))
		DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, dataset, presID));
	else
		DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));

	// Compare presentation context ID of command and data set
	if (presIDdset != presID)
	{
		DCMNET_ERROR("Presentation Context ID of command (" << OFstatic_cast(unsigned int, presID)
			<< ") and data set (" << OFstatic_cast(unsigned int, presIDdset) << ") differs");
		delete dataset;
		return makeDcmnetCondition(DIMSEC_INVALIDPRESENTATIONCONTEXTID, OF_error,
			"DIMSE: Presentation Contexts of Command and Data Set differ");
	}

	// Set return values
	reqDataset = dataset;
	moveDest = reqMessage.MoveDestination;

	return cond;
}


OFCondition D_DcmSCP::sendMOVEResponse(const T_ASC_PresentationContextID presID,
	const Uint16 messageID,
	const OFString &sopClassUID,
	DcmDataset *rspDataset,
	const Uint16 rspStatusCode,
	DcmDataset *statusDetail,
	const Uint16 numRemain,
	const Uint16 numComplete,
	const Uint16 numFail,
	const Uint16 numWarn)
{
	OFCondition cond;
	OFString tempStr;

	// Send back response
	T_DIMSE_Message response;
	// Make sure everything is zeroed (especially options)
	bzero((char*)&response, sizeof(response));
	T_DIMSE_C_MoveRSP &moveRsp = response.msg.CMoveRSP;
	response.CommandField = DIMSE_C_MOVE_RSP;
	moveRsp.MessageIDBeingRespondedTo = messageID;
	moveRsp.DimseStatus = rspStatusCode;
	// Always send the optional field "Affected SOP Class UID"
	moveRsp.opts = O_MOVE_AFFECTEDSOPCLASSUID;
	OFStandard::strlcpy(moveRsp.AffectedSOPClassUID, sopClassUID.c_str(), sizeof(moveRsp.AffectedSOPClassUID));
	// Only send the other optional fields if needed
	if ((numRemain != 0) || (numComplete != 0) || (numFail != 0) || (numWarn != 0))
	{
		moveRsp.NumberOfRemainingSubOperations = numRemain;
		moveRsp.NumberOfCompletedSubOperations = numComplete;
		moveRsp.NumberOfFailedSubOperations = numFail;
		moveRsp.NumberOfWarningSubOperations = numWarn;
		moveRsp.opts |= O_MOVE_NUMBEROFREMAININGSUBOPERATIONS | O_MOVE_NUMBEROFCOMPLETEDSUBOPERATIONS |
			O_MOVE_NUMBEROFFAILEDSUBOPERATIONS | O_MOVE_NUMBEROFWARNINGSUBOPERATIONS;
	}

	if (rspDataset)
		moveRsp.DataSetType = DIMSE_DATASET_PRESENT;
	else
		moveRsp.DataSetType = DIMSE_DATASET_NULL;

	if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
	{
		DCMNET_INFO("Sending C-MOVE Response");
		DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, response, DIMSE_OUTGOING, rspDataset, presID));
	}
	else {
		DCMNET_INFO("Sending C-MOVE Response (" << DU_cmoveStatusString(rspStatusCode) << ")");
	}

	// Send response message with dataset
	cond = sendDIMSEMessage(presID, &response, rspDataset /* dataObject */, statusDetail);
	if (cond.bad())
	{
		DCMNET_ERROR("Failed sending C-MOVE response: " << DimseCondition::dump(tempStr, cond));
	}

	return cond;
}

// ----------------------------------------------------------------------------

// -- N-ACTION --

OFCondition D_DcmSCP::receiveACTIONRequest(T_DIMSE_N_ActionRQ &reqMessage,
	const T_ASC_PresentationContextID presID,
	DcmDataset *&reqDataset,
	Uint16 &actionTypeID)
{
	// Do some basic validity checks
	if (m_assoc == NULL)
		return DIMSE_ILLEGALASSOCIATION;

	OFCondition cond;
	OFString tempStr;
	T_ASC_PresentationContextID presIDdset;
	DcmDataset *dataset = NULL;

	// Dump debug information
	if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
		DCMNET_INFO("Received N-ACTION Request");
	else
		DCMNET_INFO("Received N-ACTION Request (MsgID " << reqMessage.MessageID << ")");

	// Check if dataset is announced correctly
	if (reqMessage.DataSetType == DIMSE_DATASET_NULL)
	{
		DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));
		DCMNET_ERROR("Received N-ACTION request but no dataset announced, aborting");
		return DIMSE_BADMESSAGE;
	}

	// Receive dataset
	cond = receiveDIMSEDataset(&presIDdset, &dataset);
	if (cond.bad())
	{
		DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));
		DCMNET_ERROR("Unable to receive N-ACTION dataset on presentation context " << OFstatic_cast(unsigned int, presID));
		return DIMSE_BADDATA;
	}

	// Output request message only if trace level is enabled
	if (DCM_dcmnetLogger.isEnabledFor(OFLogger::TRACE_LOG_LEVEL))
		DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, dataset, presID));
	else
		DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));

	// Compare presentation context ID of command and data set
	if (presIDdset != presID)
	{
		DCMNET_ERROR("Presentation Context ID of command (" << OFstatic_cast(unsigned int, presID)
			<< ") and data set (" << OFstatic_cast(unsigned int, presIDdset) << ") differs");
		delete dataset;
		return makeDcmnetCondition(DIMSEC_INVALIDPRESENTATIONCONTEXTID, OF_error,
			"DIMSE: Presentation Contexts of Command and Data Set differ");
	}

	// Set return values
	reqDataset = dataset;
	actionTypeID = reqMessage.ActionTypeID;

	return cond;
}


OFCondition D_DcmSCP::sendACTIONResponse(const T_ASC_PresentationContextID presID,
	const Uint16 messageID,
	const OFString &sopClassUID,
	const OFString &sopInstanceUID,
	const Uint16 rspStatusCode)
{
	OFCondition cond;
	OFString tempStr;

	// Send back response
	T_DIMSE_Message response;
	// Make sure everything is zeroed (especially options)
	bzero((char*)&response, sizeof(response));
	T_DIMSE_N_ActionRSP &actionRsp = response.msg.NActionRSP;
	response.CommandField = DIMSE_N_ACTION_RSP;
	actionRsp.MessageIDBeingRespondedTo = messageID;
	actionRsp.DimseStatus = rspStatusCode;
	actionRsp.DataSetType = DIMSE_DATASET_NULL;
	// Always send the optional fields "Affected SOP Class UID" and "Affected SOP Instance UID"
	actionRsp.opts = O_NACTION_AFFECTEDSOPCLASSUID | O_NACTION_AFFECTEDSOPINSTANCEUID;
	OFStandard::strlcpy(actionRsp.AffectedSOPClassUID, sopClassUID.c_str(), sizeof(actionRsp.AffectedSOPClassUID));
	OFStandard::strlcpy(actionRsp.AffectedSOPInstanceUID, sopInstanceUID.c_str(), sizeof(actionRsp.AffectedSOPInstanceUID));
	// Do not send any other optional fields, e.g. "Action Type ID"

	if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
	{
		DCMNET_INFO("Sending N-ACTION Response");
		DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, response, DIMSE_OUTGOING, NULL, presID));
	}
	else {
		DCMNET_INFO("Sending N-ACTION Response (" << DU_nactionStatusString(rspStatusCode) << ")");
	}

	// Send response message
	cond = sendDIMSEMessage(presID, &response, NULL /* dataObject */);
	if (cond.bad())
	{
		DCMNET_ERROR("Failed sending N-ACTION response: " << DimseCondition::dump(tempStr, cond));
	}

	return cond;
}

// ----------------------------------------------------------------------------

// -- N-EVENT-REPORT --

OFCondition D_DcmSCP::handleEVENTREPORTRequest(T_DIMSE_N_EventReportRQ &reqMessage,
	const T_ASC_PresentationContextID presID,
	DcmDataset *&reqDataset,
	Uint16 &eventTypeID)
{
	// Do some basic validity checks
	if (m_assoc == NULL)
		return DIMSE_ILLEGALASSOCIATION;

	OFCondition cond;
	OFString tempStr;
	T_ASC_PresentationContextID presIDdset;
	DcmDataset *dataset = NULL;
	// DcmDataset *statusDetail = NULL; // TODO: do we need this and if so, how do we get it?
	Uint16 rspStatusCode = 0;

	// Dump debug information
	if (DCM_dcmnetLogger.isEnabledFor(OFLogger::TRACE_LOG_LEVEL))
		DCMNET_INFO("Received N-EVENT-REPORT Request");
	else
		DCMNET_INFO("Received N-EVENT-REPORT Request (MsgID " << reqMessage.MessageID << ")");

	// Check if dataset is announced correctly
	if (reqMessage.DataSetType == DIMSE_DATASET_NULL)
	{
		DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));
		DCMNET_ERROR("Received N-EVENT-REPORT request but no dataset announced, aborting");
		return DIMSE_BADMESSAGE;
	}

	// Receive dataset
	cond = receiveDIMSEDataset(&presIDdset, &dataset);
	if (cond.bad())
	{
		DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));
		DCMNET_ERROR("Unable to receive N-EVENT-REPORT dataset on presentation context " << OFstatic_cast(unsigned int, presID));
		return DIMSE_BADDATA;
	}

	// Output dataset only if trace level is enabled
	if (DCM_dcmnetLogger.isEnabledFor(OFLogger::TRACE_LOG_LEVEL))
		DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, dataset, presID));
	else
		DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));

	// Compare presentation context ID of command and data set
	if (presIDdset != presID)
	{
		DCMNET_ERROR("Presentation Context ID of command (" << OFstatic_cast(unsigned int, presID)
			<< ") and data set (" << OFstatic_cast(unsigned int, presIDdset) << ") differs");
		delete dataset;
		return makeDcmnetCondition(DIMSEC_INVALIDPRESENTATIONCONTEXTID, OF_error,
			"DIMSE: Presentation Contexts of Command and Data Set differ");
	}

	// Check the request message and dataset and return the DIMSE status code to be used
	rspStatusCode = checkEVENTREPORTRequest(reqMessage, dataset);

	// Send back response
	T_DIMSE_Message response;
	// Make sure everything is zeroed (especially options)
	bzero((char*)&response, sizeof(response));
	T_DIMSE_N_EventReportRSP &eventReportRsp = response.msg.NEventReportRSP;
	response.CommandField = DIMSE_N_EVENT_REPORT_RSP;
	eventReportRsp.MessageIDBeingRespondedTo = reqMessage.MessageID;
	eventReportRsp.DimseStatus = rspStatusCode;
	eventReportRsp.DataSetType = DIMSE_DATASET_NULL;
	// Do not send any optional fields
	eventReportRsp.opts = 0;
	eventReportRsp.AffectedSOPClassUID[0] = 0;
	eventReportRsp.AffectedSOPInstanceUID[0] = 0;

	if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
	{
		DCMNET_INFO("Sending N-EVENT-REPORT Response");
		DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, response, DIMSE_OUTGOING, NULL, presID));
	}
	else {
		DCMNET_INFO("Sending N-EVENT-REPORT Response (" << DU_neventReportStatusString(rspStatusCode) << ")");
	}
	// Send response message
	cond = sendDIMSEMessage(presID, &response, NULL /* dataObject */);
	if (cond.bad())
	{
		DCMNET_ERROR("Failed sending N-EVENT-REPORT response: " << DimseCondition::dump(tempStr, cond));
		delete dataset;
		return cond;
	}

	// Set return values
	reqDataset = dataset;
	eventTypeID = reqMessage.EventTypeID;

	return cond;
}


OFCondition D_DcmSCP::sendEVENTREPORTRequest(const T_ASC_PresentationContextID presID,
	const OFString &sopInstanceUID,
	const Uint16 messageID,
	const Uint16 eventTypeID,
	DcmDataset *reqDataset,
	Uint16 &rspStatusCode)
{
	// Do some basic validity checks
	if (m_assoc == NULL)
		return DIMSE_ILLEGALASSOCIATION;
	if (sopInstanceUID.empty() || (reqDataset == NULL))
		return DIMSE_NULLKEY;

	// Prepare DIMSE data structures for issuing request
	OFCondition cond;
	OFString tempStr;
	T_ASC_PresentationContextID pcid = presID;
	T_DIMSE_Message request;
	// Make sure everything is zeroed (especially options)
	bzero((char*)&request, sizeof(request));
	T_DIMSE_N_EventReportRQ &eventReportReq = request.msg.NEventReportRQ;
	DcmDataset *statusDetail = NULL;

	request.CommandField = DIMSE_N_EVENT_REPORT_RQ;

	// Generate a new message ID (?)
	eventReportReq.MessageID = messageID;

	eventReportReq.DataSetType = DIMSE_DATASET_PRESENT;
	eventReportReq.EventTypeID = eventTypeID;

	// Determine SOP Class from presentation context
	OFString abstractSyntax, transferSyntax;

	findPresentationContext(pcid, abstractSyntax, transferSyntax);
	if (abstractSyntax.empty() || transferSyntax.empty())
		return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
	OFStandard::strlcpy(eventReportReq.AffectedSOPClassUID, abstractSyntax.c_str(), sizeof(eventReportReq.AffectedSOPClassUID));
	OFStandard::strlcpy(eventReportReq.AffectedSOPInstanceUID, sopInstanceUID.c_str(), sizeof(eventReportReq.AffectedSOPInstanceUID));

	// Send request
	if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
	{
		DCMNET_INFO("Sending N-EVENT-REPORT Request");
		// Output dataset only if trace level is enabled
		if (DCM_dcmnetLogger.isEnabledFor(OFLogger::TRACE_LOG_LEVEL))
			DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, request, DIMSE_OUTGOING, reqDataset, pcid));
		else
			DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, request, DIMSE_OUTGOING, NULL, pcid));
	}
	else {
		DCMNET_INFO("Sending N-EVENT-REPORT Request (MsgID " << eventReportReq.MessageID << ")");
	}
	cond = sendDIMSEMessage(pcid, &request, reqDataset);
	if (cond.bad())
	{
		DCMNET_ERROR("Failed sending N-EVENT-REPORT request: " << DimseCondition::dump(tempStr, cond));
		return cond;
	}
	// Receive response
	T_DIMSE_Message response;
	cond = receiveDIMSECommand(&pcid, &response, &statusDetail, NULL /* commandSet */);
	if (cond.bad())
	{
		DCMNET_ERROR("Failed receiving DIMSE response: " << DimseCondition::dump(tempStr, cond));
		return cond;
	}

	// Check command set
	if (response.CommandField == DIMSE_N_EVENT_REPORT_RSP)
	{
		if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
		{
			DCMNET_INFO("Received N-EVENT-REPORT Response");
			DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, response, DIMSE_INCOMING, NULL, pcid));
		}
		else {
			DCMNET_INFO("Received N-EVENT-REPORT Response (" << DU_neventReportStatusString(response.msg.NEventReportRSP.DimseStatus) << ")");
		}
	}
	else {
		DCMNET_ERROR("Expected N-EVENT-REPORT response but received DIMSE command 0x"
			<< STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(4)
			<< OFstatic_cast(unsigned int, response.CommandField));
		DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, response, DIMSE_INCOMING, NULL, pcid));
		delete statusDetail;
		return DIMSE_BADCOMMANDTYPE;
	}
	if (statusDetail != NULL)
	{
		DCMNET_DEBUG("Response has status detail:" << OFendl << DcmObject::PrintHelper(*statusDetail));
		delete statusDetail;
	}

	// Set return value
	T_DIMSE_N_EventReportRSP &eventReportRsp = response.msg.NEventReportRSP;
	rspStatusCode = eventReportRsp.DimseStatus;

	// Check whether there is a dataset to be received
	if (eventReportRsp.DataSetType == DIMSE_DATASET_PRESENT)
	{
		// this should never happen
		DcmDataset *tempDataset = NULL;
		T_ASC_PresentationContextID tempID;
		// Receive dataset
		cond = receiveDIMSEDataset(&tempID, &tempDataset);
		if (cond.good())
		{
			DCMNET_WARN("Received unexpected dataset after N-EVENT-REPORT response, ignoring");
			delete tempDataset;
		}
		else {
			DCMNET_ERROR("Failed receiving unexpected dataset after N-EVENT-REPORT response: "
				<< DimseCondition::dump(tempStr, cond));
			return DIMSE_BADDATA;
		}
	}

	// Check whether the message ID being responded to is equal to the message ID of the request
	if (eventReportRsp.MessageIDBeingRespondedTo != eventReportReq.MessageID)
	{
		DCMNET_ERROR("Received response with wrong message ID (" << eventReportRsp.MessageIDBeingRespondedTo
			<< " instead of " << eventReportReq.MessageID << ")");
		return DIMSE_BADMESSAGE;
	}

	return cond;
}


Uint16 D_DcmSCP::checkEVENTREPORTRequest(T_DIMSE_N_EventReportRQ & /*reqMessage*/,
	DcmDataset * /*reqDataset*/)
{
	// we default to success
	return STATUS_Success;
}


/* ************************************************************************* */
/*                         General message handling                          */
/* ************************************************************************* */

void D_DcmSCP::notifySENDProgress(const unsigned long byteCount)
{
	DCMNET_TRACE("Bytes sent: " << byteCount);
}


void D_DcmSCP::notifyRECEIVEProgress(const unsigned long byteCount)
{
	DCMNET_TRACE("Bytes received: " << byteCount);
}


/* ************************************************************************* */
/*                            Various helpers                                */
/* ************************************************************************* */

// Sends a DIMSE command and possibly also instance data to the configured peer DICOM application
OFCondition D_DcmSCP::sendDIMSEMessage(const T_ASC_PresentationContextID presID,
	T_DIMSE_Message *message,
	DcmDataset *dataObject,
	DcmDataset *statusDetail,
	DcmDataset **commandSet)
{
	if (m_assoc == NULL)
		return DIMSE_ILLEGALASSOCIATION;
	if (message == NULL)
		return DIMSE_NULLKEY;

	OFCondition cond;
	/* call the corresponding DIMSE function to sent the message */
	if (m_cfg->getProgressNotificationMode())
	{
		cond = DIMSE_sendMessageUsingMemoryData(m_assoc, presID, message, statusDetail, dataObject,
			callbackSENDProgress, this /*callbackData*/, commandSet);
	}
	else {
		cond = DIMSE_sendMessageUsingMemoryData(m_assoc, presID, message, statusDetail, dataObject,
			NULL /*callback*/, NULL /*callbackData*/, commandSet);
	}
	return cond;
}

// ----------------------------------------------------------------------------

// Receive DIMSE command (excluding dataset!) over the currently open association
OFCondition D_DcmSCP::receiveDIMSECommand(T_ASC_PresentationContextID *presID,
	T_DIMSE_Message *message,
	DcmDataset **statusDetail,
	DcmDataset **commandSet,
	const Uint32 timeout)
{
	if (m_assoc == NULL)
		return DIMSE_ILLEGALASSOCIATION;

	OFCondition cond;
	if (timeout > 0)
	{
		/* call the corresponding DIMSE function to receive the command (use specified timeout) */
		cond = DIMSE_receiveCommand(m_assoc, DIMSE_NONBLOCKING, timeout, presID,
			message, statusDetail, commandSet);
	}
	else {
		/* call the corresponding DIMSE function to receive the command (use default timeout) */
		cond = DIMSE_receiveCommand(m_assoc, m_cfg->getDIMSEBlockingMode(), m_cfg->getDIMSETimeout(), presID,
			message, statusDetail, commandSet);
	}
	return cond;
}

// ----------------------------------------------------------------------------

// Receives one dataset (of instance data) via network from another DICOM application in memory
OFCondition D_DcmSCP::receiveDIMSEDataset(T_ASC_PresentationContextID *presID,
	DcmDataset **dataObject)
{
	if (m_assoc == NULL)
		return DIMSE_ILLEGALASSOCIATION;

	OFCondition cond;
	/* call the corresponding DIMSE function to receive the dataset */
	if (m_cfg->getProgressNotificationMode())
	{
		cond = DIMSE_receiveDataSetInMemory(m_assoc, m_cfg->getDIMSEBlockingMode(), m_cfg->getDIMSETimeout(),
			presID, dataObject, callbackRECEIVEProgress, this /*callbackData*/);
	}
	else {
		cond = DIMSE_receiveDataSetInMemory(m_assoc, m_cfg->getDIMSEBlockingMode(), m_cfg->getDIMSETimeout(),
			presID, dataObject, NULL /*callback*/, NULL /*callbackData*/);
	}

	if (cond.good())
	{
		DCMNET_DEBUG("Received dataset on presentation context " << OFstatic_cast(unsigned int, *presID));
	}
	else {
		OFString tempStr;
		DCMNET_ERROR("Unable to receive dataset on presentation context "
			<< OFstatic_cast(unsigned int, *presID) << ": " << DimseCondition::dump(tempStr, cond));
	}
	return cond;
}

// ----------------------------------------------------------------------------

// Receives one C-STORE request dataset via network from another DICOM application
// (and store it directly to file)
OFCondition D_DcmSCP::receiveSTORERequestDataset(T_ASC_PresentationContextID *presID,
	T_DIMSE_C_StoreRQ &reqMessage,
	const OFString &filename)
{
	if (m_assoc == NULL)
		return DIMSE_ILLEGALASSOCIATION;
	if (filename.empty())
		return EC_InvalidFilename;

	OFString tempStr;
	DcmOutputFileStream *filestream = NULL;
	// Receive dataset over the network and write it directly to a file
	OFCondition cond = DIMSE_createFilestream(filename, &reqMessage, m_assoc, *presID,
		OFTrue /*writeMetaheader*/, &filestream);
	if (cond.good())
	{
		if (m_cfg->getProgressNotificationMode())
		{
			cond = DIMSE_receiveDataSetInFile(m_assoc, m_cfg->getDIMSEBlockingMode(), m_cfg->getDIMSETimeout(),
				presID, filestream, callbackRECEIVEProgress, this /*callbackData*/);
		}
		else {
			cond = DIMSE_receiveDataSetInFile(m_assoc, m_cfg->getDIMSEBlockingMode(), m_cfg->getDIMSETimeout(),
				presID, filestream, NULL /*callback*/, NULL /*callbackData*/);
		}
		delete filestream;
		if (cond.good())
		{
			DCMNET_DEBUG("Received dataset on presentation context " << OFstatic_cast(unsigned int, *presID)
				<< " and stored it directly to file");
		}
		else {
			DCMNET_ERROR("Unable to receive dataset on presentation context "
				<< OFstatic_cast(unsigned int, *presID) << ": " << DimseCondition::dump(tempStr, cond));
			// Delete created file in case of error
			OFStandard::deleteFile(filename);
		}

	}
	else {

		DCMNET_ERROR("Unable to receive dataset on presentation context "
			<< OFstatic_cast(unsigned int, *presID) << ": " << DimseCondition::dump(tempStr, cond));
		// Could not create the filestream, so ignore the dataset
		DIC_UL bytesRead = 0;
		DIC_UL pdvCount = 0;
		DCMNET_DEBUG("Ignoring incoming dataset and returning an error status to the SCU");
		cond = DIMSE_ignoreDataSet(m_assoc, m_cfg->getDIMSEBlockingMode(), m_cfg->getDIMSETimeout(),
			&bytesRead, &pdvCount);
		if (cond.good())
		{
			tempStr = "Cannot create file: " + filename;
			cond = makeDcmnetCondition(DIMSEC_OUTOFRESOURCES, OF_error, tempStr.c_str());
		}
	}
	return cond;
}

// ----------------------------------------------------------------------------

OFBool D_DcmSCP::addStatusDetail(DcmDataset **statusDetail,
	const DcmElement *elem)
{
	DCMNET_TRACE("Add element to status detail");
	// If no element was passed, return to the caller.
	if (elem == NULL)
		return OFFalse;

	DcmAttributeTag *at;
	DcmLongString *lo;

	// Create the container object if necessary
	if (*statusDetail == NULL)
		*statusDetail = new DcmDataset();

	if (statusDetail == NULL)
	{
		DCMNET_ERROR("Cannot create status detail object, memory exhausted!");
		return OFFalse;
	}

	// Determine the element's data type
	DcmVR vr(elem->ident());

	// Depending on the element's identification, insert different
	// types of objects into the container.
	switch (elem->ident())
	{
	case EVR_LO:
		lo = new DcmLongString(*((DcmLongString*)elem));
		if (lo->getLength() > vr.getMaxValueLength())
		{
			DCMNET_WARN("Value inside given LO attribute too large for status detail (max " << OFstatic_cast(Uint32, vr.getMaxValueLength()) << ") for " << vr.getVRName());
		}
		(*statusDetail)->insert(lo, OFTrue /*replaceOld*/);
		// Print debug information
		{
			OFOStringStream oss;
			lo->print(oss);
			OFSTRINGSTREAM_GETSTR(oss, strtemp);
			DCMNET_DEBUG("Adding LO status detail information: " << strtemp);
			OFSTRINGSTREAM_FREESTR(tmpString)
		}
		break;
	case EVR_AT:
		at = new DcmAttributeTag(*((DcmAttributeTag*)elem));
		if (at->getLength() > vr.getMaxValueLength())
		{
			DCMNET_WARN("Value inside given AT attribute too large for status detail (max " << OFstatic_cast(Uint32, vr.getMaxValueLength()) << ") for " << vr.getVRName());
		}
		(*statusDetail)->insert(at, OFTrue /*replaceOld*/);
		// Print debug information
		{
			OFOStringStream oss;
			at->print(oss);
			OFSTRINGSTREAM_GETSTR(oss, strtemp);
			DCMNET_DEBUG("Adding AT status detail information: " << strtemp);
			OFSTRINGSTREAM_FREESTR(tmpString)
		}
		break;
	default:  // other status detail is not supported
		DCMNET_ERROR("Cannot add status detail, unsupported detail attribute type: " << vr.getVRName());
		return OFFalse;
		break;
	}
	return OFTrue;
}

// ----------------------------------------------------------------------------

void D_DcmSCP::forceAssociationRefuse(const OFBool doRefuse)
{
	m_cfg->forceAssociationRefuse(doRefuse);
}

// ----------------------------------------------------------------------------

void D_DcmSCP::setMaxReceivePDULength(const Uint32 maxRecPDU)
{
	m_cfg->setMaxReceivePDULength(maxRecPDU);
}

// ----------------------------------------------------------------------------

OFCondition D_DcmSCP::setEnableVerification(const OFString &profile)
{

	OFList<OFString> xfers;
	xfers.push_back(UID_LittleEndianExplicitTransferSyntax);
	xfers.push_back(UID_BigEndianExplicitTransferSyntax);
	xfers.push_back(UID_LittleEndianImplicitTransferSyntax);
	return m_cfg->addPresentationContext(UID_VerificationSOPClass, xfers, ASC_SC_ROLE_DEFAULT, profile);
}

// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------

OFCondition D_DcmSCP::addPresentationContext(const OFString &abstractSyntax,
	const OFList<OFString> &xferSyntaxes,
	const T_ASC_SC_ROLE requestorRole,
	const OFString &profile)
{
	return m_cfg->addPresentationContext(abstractSyntax, xferSyntaxes, requestorRole, profile);
}

// ----------------------------------------------------------------------------

void D_DcmSCP::setPort(const Uint16 port)
{
	m_cfg->setPort(port);
}

// ----------------------------------------------------------------------------

void D_DcmSCP::setAETitle(const OFString &aetitle)
{
	m_cfg->setAETitle(aetitle);
}

// ----------------------------------------------------------------------------

void D_DcmSCP::setRespondWithCalledAETitle(const OFBool useCalled)
{
	m_cfg->setRespondWithCalledAETitle(useCalled);
}

// ----------------------------------------------------------------------------

OFCondition D_DcmSCP::loadAssociationCfgFile(const OFString &assocFile)
{
	return m_cfg->loadAssociationCfgFile(assocFile);
}

// ----------------------------------------------------------------------------

OFCondition D_DcmSCP::setAndCheckAssociationProfile(const OFString &profileName)
{
	return m_cfg->setAndCheckAssociationProfile(profileName);
}

// ----------------------------------------------------------------------------

void D_DcmSCP::setConnectionBlockingMode(const DUL_BLOCKOPTIONS blockingMode)
{
	m_cfg->setConnectionBlockingMode(blockingMode);
}

// ----------------------------------------------------------------------------

void D_DcmSCP::setDIMSEBlockingMode(const T_DIMSE_BlockingMode blockingMode)
{
	m_cfg->setDIMSEBlockingMode(blockingMode);
}

// ----------------------------------------------------------------------------

void D_DcmSCP::setDIMSETimeout(const Uint32 dimseTimeout)
{
	m_cfg->setDIMSETimeout(dimseTimeout);
}

// ----------------------------------------------------------------------------

void D_DcmSCP::setACSETimeout(const Uint32 acseTimeout)
{
	m_cfg->setACSETimeout(acseTimeout);
}

// ----------------------------------------------------------------------------

void D_DcmSCP::setConnectionTimeout(const Uint32 timeout)
{
	m_cfg->setConnectionTimeout(timeout);
}

// ----------------------------------------------------------------------------

void D_DcmSCP::setVerbosePCMode(const OFBool mode)
{
	m_cfg->setVerbosePCMode(mode);
}

// ----------------------------------------------------------------------------

void D_DcmSCP::setHostLookupEnabled(const OFBool mode)
{
	m_cfg->setHostLookupEnabled(mode);
}

// ----------------------------------------------------------------------------

void D_DcmSCP::setProgressNotificationMode(const OFBool mode)
{
	m_cfg->setProgressNotificationMode(mode);
}

// ----------------------------------------------------------------------------

void D_DcmSCP::setAlwaysAcceptDefaultRole(const OFBool enabled)
{
	m_cfg->setAlwaysAcceptDefaultRole(enabled);
}

// ----------------------------------------------------------------------------

/* Get methods for SCP settings and current association information */

OFBool D_DcmSCP::getRefuseAssociation() const
{
	return m_cfg->getRefuseAssociation();
}

// ----------------------------------------------------------------------------

Uint32 D_DcmSCP::getMaxReceivePDULength() const
{
	return m_cfg->getMaxReceivePDULength();
}

// ----------------------------------------------------------------------------

Uint16 D_DcmSCP::getPort() const
{
	return m_cfg->getPort();
}

// ----------------------------------------------------------------------------

const OFString &D_DcmSCP::getAETitle() const
{
	return m_cfg->getAETitle();
}

// ----------------------------------------------------------------------------

OFBool D_DcmSCP::getRespondWithCalledAETitle() const
{
	return m_cfg->getRespondWithCalledAETitle();
}

// ----------------------------------------------------------------------------

DUL_BLOCKOPTIONS D_DcmSCP::getConnectionBlockingMode() const
{
	return m_cfg->getConnectionBlockingMode();
}

// ----------------------------------------------------------------------------

T_DIMSE_BlockingMode D_DcmSCP::getDIMSEBlockingMode() const
{
	return m_cfg->getDIMSEBlockingMode();
}

// ----------------------------------------------------------------------------

Uint32 D_DcmSCP::getDIMSETimeout() const
{
	return m_cfg->getDIMSETimeout();
}

// ----------------------------------------------------------------------------

Uint32 D_DcmSCP::getConnectionTimeout() const
{
	return m_cfg->getConnectionTimeout();
}

// ----------------------------------------------------------------------------

Uint32 D_DcmSCP::getACSETimeout() const
{
	return m_cfg->getACSETimeout();
}

// ----------------------------------------------------------------------------

OFBool D_DcmSCP::getVerbosePCMode() const
{
	return m_cfg->getVerbosePCMode();
}

// ----------------------------------------------------------------------------

OFBool D_DcmSCP::getHostLookupEnabled() const
{
	return m_cfg->getHostLookupEnabled();
}

// ----------------------------------------------------------------------------

OFBool D_DcmSCP::getProgressNotificationMode() const
{
	return m_cfg->getProgressNotificationMode();
}

// ----------------------------------------------------------------------------

OFBool D_DcmSCP::isConnected() const
{
	return (m_assoc != NULL) && (m_assoc->DULassociation != NULL);
}

// ----------------------------------------------------------------------------

OFString D_DcmSCP::getPeerAETitle() const
{
	if (m_assoc == NULL)
		return "";
	return m_assoc->params->DULparams.callingAPTitle;
}

// ----------------------------------------------------------------------------

OFString D_DcmSCP::getCalledAETitle() const
{
	if (m_assoc == NULL)
		return "";
	return m_assoc->params->DULparams.calledAPTitle;
}

// ----------------------------------------------------------------------------

Uint32 D_DcmSCP::getPeerMaxPDULength() const
{
	if (m_assoc == NULL)
		return 0;
	return m_assoc->params->theirMaxPDUReceiveSize;
}

// ----------------------------------------------------------------------------

OFString D_DcmSCP::getPeerIP() const
{
	if (m_assoc == NULL)
		return "";
	return m_assoc->params->DULparams.callingPresentationAddress;
}

// ----------------------------------------------------------------------------

void D_DcmSCP::dropAndDestroyAssociation()
{

	if (m_assoc)
	{
		ASC_dropSCPAssociation(m_assoc);
		ASC_destroyAssociation(&m_assoc);
	}
}


/* ************************************************************************** */
/*                            Notify functions                                */
/* ************************************************************************** */


void D_DcmSCP::notifyAssociationRequest(const T_ASC_Parameters &params,
	DcmSCPActionType & /* desiredAction */)
{
	// Dump some information if required
	DCMNET_INFO("Association Received " << params.DULparams.callingPresentationAddress << ": "
		<< params.DULparams.callingAPTitle << " -> "
		<< params.DULparams.calledAPTitle);

	// Dump more information if required
	OFString tempStr;
	if (m_cfg->getVerbosePCMode())
		DCMNET_INFO("Incoming Association Request:" << OFendl << ASC_dumpParameters(tempStr, m_assoc->params, ASC_ASSOC_RQ));
	else
		DCMNET_DEBUG("Incoming Association Request:" << OFendl << ASC_dumpParameters(tempStr, m_assoc->params, ASC_ASSOC_RQ));
}

// ----------------------------------------------------------------------------

OFBool D_DcmSCP::checkCalledAETitleAccepted(const OFString& /*calledAETitle*/)
{
	return OFTrue;
}

// ----------------------------------------------------------------------------

OFBool D_DcmSCP::checkCallingAETitleAccepted(const OFString& /*callingAETitle*/)
{
	return OFTrue;
}

// ----------------------------------------------------------------------------

OFBool D_DcmSCP::checkCallingHostAccepted(const OFString& /*hostOrIP*/)
{
	return OFTrue;
}

// ----------------------------------------------------------------------------

void D_DcmSCP::notifyAssociationAcknowledge()
{
	DCMNET_DEBUG("DcmSCP: Association Acknowledged");
}

// ----------------------------------------------------------------------------

void D_DcmSCP::notifyReleaseRequest()
{
	DCMNET_INFO("Received Association Release Request");
}


// ----------------------------------------------------------------------------

void D_DcmSCP::notifyAbortRequest()
{
	DCMNET_INFO("Received Association Abort Request");
}

// ----------------------------------------------------------------------------

void D_DcmSCP::notifyAssociationTermination()
{
	DCMNET_DEBUG("DcmSCP: Association Terminated");
}

// ----------------------------------------------------------------------------

void D_DcmSCP::notifyConnectionTimeout()
{
	DCMNET_TRACE("Connection timeout encountered in non-blocking mode");
}

// ----------------------------------------------------------------------------

void D_DcmSCP::notifyDIMSEError(const OFCondition &cond)
{
	OFString tempStr;
	DCMNET_DEBUG("DIMSE Error, detail (if available): " << DimseCondition::dump(tempStr, cond));
}

// ----------------------------------------------------------------------------

OFBool D_DcmSCP::stopAfterCurrentAssociation()
{
	return OFFalse;
}

// ----------------------------------------------------------------------------

OFBool D_DcmSCP::stopAfterConnectionTimeout()
{
	return OFFalse;
}


/* ************************************************************************* */
/*                            Callback functions                             */
/* ************************************************************************* */

void D_DcmSCP::callbackSENDProgress(void *callbackContext,
	const unsigned long byteCount)
{
	if (callbackContext != NULL)
		OFreinterpret_cast(D_DcmSCP *, callbackContext)->notifySENDProgress(byteCount);
}


void D_DcmSCP::callbackRECEIVEProgress(void *callbackContext,
	const unsigned long byteCount)
{
	if (callbackContext != NULL)
		OFreinterpret_cast(D_DcmSCP *, callbackContext)->notifyRECEIVEProgress(byteCount);
}


/* ************************************************************************* */
/*                         Static helper functions                           */
/* ************************************************************************* */

OFBool D_DcmSCP::getPresentationContextInfo(const T_ASC_Association *assoc,
	const Uint8 presID,
	DcmPresentationContextInfo &presInfo)
{
	if (assoc != NULL)
	{
		DUL_PRESENTATIONCONTEXT *pc = findPresentationContextID(assoc->params->DULparams.acceptedPresentationContext, presID);
		if (pc != NULL)
		{
			presInfo.abstractSyntax = pc->abstractSyntax;
			presInfo.acceptedTransferSyntax = pc->acceptedTransferSyntax;
			presInfo.presentationContextID = pc->presentationContextID;
			presInfo.proposedSCRole = pc->proposedSCRole;
			presInfo.acceptedSCRole = pc->acceptedSCRole;
			return OFTrue;
		}
	}
	return OFFalse;
}
