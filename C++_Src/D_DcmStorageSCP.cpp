#include "pch.h"
#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "D_DcmStorageSCP.h"
#include "dcmtk/dcmnet/diutil.h"
#include "global.h"
#include <string>


 // constant definitions

const char *D_DcmStorageSCP::DEF_StandardSubdirectory = "data";
const char *D_DcmStorageSCP::DEF_UndefinedSubdirectory = "undef";
const char *D_DcmStorageSCP::DEF_FilenameExtension = "";


// implementation of the main interface class

D_DcmStorageSCP::D_DcmStorageSCP()
	: D_DcmSCP(),
	OutputDirectory(),
	StandardSubdirectory(DEF_StandardSubdirectory),
	UndefinedSubdirectory(DEF_UndefinedSubdirectory),
	FilenameExtension(DEF_FilenameExtension),
	DirectoryGeneration(DGM_Default),
	FilenameGeneration(FGM_Default),
	FilenameCreator(),
	DatasetStorage(DSM_Default)
{
	// make sure that the SCP at least supports C-ECHO with default transfer syntax
	OFList<OFString> transferSyntaxes;
	transferSyntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);
	addPresentationContext(UID_VerificationSOPClass, transferSyntaxes);
}


D_DcmStorageSCP::~D_DcmStorageSCP()
{
	// clear internal state
	clear();
}


void D_DcmStorageSCP::clear()
{
	// DcmSCP::clear();
	OutputDirectory.clear();
	StandardSubdirectory = DEF_StandardSubdirectory;
	UndefinedSubdirectory = DEF_UndefinedSubdirectory;
	FilenameExtension = DEF_FilenameExtension;
	DirectoryGeneration = DGM_Default;
	FilenameGeneration = FGM_Default;
	DatasetStorage = DSM_Default;
}


// get methods

const OFString &D_DcmStorageSCP::getOutputDirectory() const
{
	return OutputDirectory;
}


D_DcmStorageSCP::E_DirectoryGenerationMode D_DcmStorageSCP::getDirectoryGenerationMode() const
{
	return DirectoryGeneration;
}


D_DcmStorageSCP::E_FilenameGenerationMode D_DcmStorageSCP::getFilenameGenerationMode() const
{
	return FilenameGeneration;
}


const OFString &D_DcmStorageSCP::getFilenameExtension() const
{
	return FilenameExtension;
}


D_DcmStorageSCP::E_DatasetStorageMode D_DcmStorageSCP::getDatasetStorageMode() const
{
	return DatasetStorage;
}


// set methods

OFCondition D_DcmStorageSCP::setOutputDirectory(const OFString &directory)
{
	OFCondition status = EC_Normal;
	if (directory.empty())
	{
		// empty directory refers to the current directory
		if (OFStandard::isWriteable("."))
			OutputDirectory.clear();
		else
			status = EC_DirectoryNotWritable;
	}
	else {
		// check whether given directory exists and is writable
		if (OFStandard::dirExists(directory))
		{
			if (OFStandard::isWriteable(directory))
				OFStandard::normalizeDirName(OutputDirectory, directory);
			else
				status = EC_DirectoryNotWritable;
		}
		else
			status = EC_DirectoryDoesNotExist;
	}
	return status;
}


void D_DcmStorageSCP::setDirectoryGenerationMode(const E_DirectoryGenerationMode mode)
{
	DirectoryGeneration = mode;
}


void D_DcmStorageSCP::setFilenameGenerationMode(const E_FilenameGenerationMode mode)
{
	FilenameGeneration = mode;
}


void D_DcmStorageSCP::setFilenameExtension(const OFString &extension)
{
	FilenameExtension = extension;
}


void D_DcmStorageSCP::setDatasetStorageMode(const E_DatasetStorageMode mode)
{
	DatasetStorage = mode;
}


// further public methods

OFCondition D_DcmStorageSCP::loadAssociationConfiguration(const OFString &filename,
	const OFString &profile)
{
	// first, try to load the configuration file
	OFCondition status = loadAssociationCfgFile(filename);
	// and then, try to select the desired profile
	if (status.good())
		status = setAndCheckAssociationProfile(profile);
	return status;
}


// protected methods

OFCondition D_DcmStorageSCP::handleIncomingCommand(T_DIMSE_Message *incomingMsg,
	const DcmPresentationContextInfo &presInfo,SharedBuf& srb)
{
	OFCondition status = EC_IllegalParameter;
	if (incomingMsg != NULL)
	{
		// check whether we've received a supported command
		if (incomingMsg->CommandField == DIMSE_C_ECHO_RQ)
		{
			// handle incoming C-ECHO request
			status = handleECHORequest(incomingMsg->msg.CEchoRQ, presInfo.presentationContextID);
		}
		else if (incomingMsg->CommandField == DIMSE_C_STORE_RQ)
		{
			// handle incoming C-STORE request
			T_DIMSE_C_StoreRQ &storeReq = incomingMsg->msg.CStoreRQ;
			Uint16 rspStatusCode = STATUS_STORE_Error_CannotUnderstand;
			// special case: bit preserving mode
			if (DatasetStorage == DGM_StoreBitPreserving)
			{
				OFString filename;
				// generate filename with full path (and create subdirectories if needed)
				status = generateSTORERequestFilename(storeReq, filename);
				if (status.good())
				{
					if (OFStandard::fileExists(filename))
					{
						DCMNET_WARN("file already exists, overwriting: " << filename);
						g_log->info("D_DcmStorageSCP::handleIncomingCommand:文件已经存在，重写入:{}", (std::string)filename.c_str());
					}
					// receive dataset directly to file
					status = receiveSTORERequest(storeReq, presInfo.presentationContextID, filename,srb);
					if (status.good())
					{
						// call the notification handler (default implementation outputs to the logger)
						notifyInstanceStored(filename, storeReq.AffectedSOPClassUID, storeReq.AffectedSOPInstanceUID);
						rspStatusCode = STATUS_Success;
					}
				}
			}
			else {
				DcmFileFormat fileformat;
				DcmDataset *reqDataset = fileformat.getDataset();
				// receive dataset in memory
				status = receiveSTORERequest(storeReq, presInfo.presentationContextID, reqDataset);
				if (status.good())
				{
					// do we need to store the received dataset at all?
					if (DatasetStorage == DSM_Ignore)
					{
						// output debug message that dataset is not stored
						DCMNET_DEBUG("received dataset is not stored since the storage mode is set to 'ignore'");
						g_log->debug("D_DcmStorageSCP::handleIncomingCommand:存储模式设置为ignore,无法保存数据");
						rspStatusCode = STATUS_Success;
					}
					else {
						// check and process C-STORE request
						rspStatusCode = checkAndProcessSTORERequest(storeReq, fileformat,srb);
					}
				}
			}
			// send C-STORE response (with DIMSE status code)
			if (status.good())
				status = sendSTOREResponse(presInfo.presentationContextID, storeReq, rspStatusCode);
			else if (status == DIMSE_OUTOFRESOURCES)
			{
				// do not overwrite the previous error status
				sendSTOREResponse(presInfo.presentationContextID, storeReq, STATUS_STORE_Refused_OutOfResources);
			}
		}
		else {
			// unsupported command
			OFString tempStr;
			DCMNET_ERROR("cannot handle this kind of DIMSE command (0x"
				<< STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(4)
				<< OFstatic_cast(unsigned int, incomingMsg->CommandField)
				<< "), we are a Storage SCP only");
			DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, *incomingMsg, DIMSE_INCOMING));
			// TODO: provide more information on this error?
			g_log->error("D_DcmStorageSCP::handleIncomingCommand:是其他命令，无法响应（除C-STORE和C-GET）");
			status = DIMSE_BADCOMMANDTYPE;
		}
	}
	return status;
}


Uint16 D_DcmStorageSCP::checkAndProcessSTORERequest(const T_DIMSE_C_StoreRQ &reqMessage,
	DcmFileFormat &fileformat,SharedBuf& srb)
{
	DCMNET_DEBUG("checking and processing C-STORE request");
	Uint16 statusCode = STATUS_STORE_Error_CannotUnderstand;
	DcmDataset *dataset = fileformat.getDataset();
	// perform some basic checks on the request dataset
	if ((dataset != NULL) && !dataset->isEmpty())
	{
		OFString filename;
		OFString directoryName;
		OFString sopClassUID = reqMessage.AffectedSOPClassUID;
		OFString sopInstanceUID = reqMessage.AffectedSOPInstanceUID;
		// generate filename with full path
		//上锁，保证存储数据的过程是完整的
		std::unique_lock<std::mutex> lock(srb.mtx);
		g_log->info("D_DcmStorageSCP::checkAndProcessSTORERequest:生成文件上锁");
		OFCondition status = generateDirAndFilename(filename, directoryName, sopClassUID, sopInstanceUID, dataset);
		if (status.good())
		{
			DCMNET_DEBUG("generated filename for received object: " << filename);
			g_log->debug("D_DcmStorageSCP::checkAndProcessSTORERequest:为接收的对象产生新的文件:{}", (std::string)filename.c_str());
			// create the output directory (if needed)
			status = OFStandard::createDirectory(directoryName, OutputDirectory /* rootDir */);
			if (status.good())
			{
				if (OFStandard::fileExists(filename))
				{
					DCMNET_WARN("file already exists, overwriting: " << filename);
					g_log->debug("D_DcmStorageSCP::checkAndProcessSTORERequest:文件已经存在，重写入:{}", (std::string)filename.c_str());
					// store the received dataset to file (with default settings)
				}
				g_log->info("D_DcmStorageSCP::checkAndProcessSTORERequest:正在往文件中:{}存储数据:",(std::string)filename.c_str());
				status = fileformat.saveFile(filename);
				if (status.good())
				{
					// call the notification handler (default implementation outputs to the logger)
					notifyInstanceStored(filename, sopClassUID, sopInstanceUID, dataset);
					statusCode = STATUS_Success;
					//将文件名保存到队列中
					g_log->info("D_DcmStorageSCP::checkAndProcessSTORERequest:队列大小:{}", srb.q.size());
					while (srb.q.size() == srb.size) //队列已满
					{
						g_log->info("D_DcmStorageSCP::checkAndProcessSTORERequest:队列已满，挂起等待......");
						std::cout << "checkAndProcessSTORERequest is waiting for an empty queueu...\n";
						srb.not_full.wait(lock);
					}
					g_log->info("D_DcmStorageSCP::checkAndProcessSTORERequest:将文件路径:{}保存到队列", (std::string)filename.c_str());
					srb.q.push((std::string)filename.c_str());
					srb.not_empty.notify_all();
				}
				else {

					DCMNET_ERROR("cannot store received object: " << filename << ": " << status.text());
					g_log->error("D_DcmStorageSCP::checkAndProcessSTORERequest:没法保存接收的对象:{}:{}", (std::string)filename.c_str(), (std::string)status.text());
					statusCode = STATUS_STORE_Refused_OutOfResources;
					OFStandard::deleteFile(filename);
				}
			}
			else {
				DCMNET_ERROR("cannot create directory for received object: " << directoryName << ": " << status.text());
				g_log->error("D_DcmStorageSCP::checkAndProcessSTORERequest：无法创建文件夹：{}:{}", (std::string)directoryName.c_str(), (std::string)status.text());
				statusCode = STATUS_STORE_Refused_OutOfResources;
			}
		}
		else
		{
			DCMNET_ERROR("cannot generate directory or file name for received object: " << status.text());
			g_log->error("D_DcmStorageSCP::checkAndProcessSTORERequest:无法为生成文件夹或者文件:{}", (std::string)status.text());
		}
		g_log->info("D_DcmStorageSCP::checkAndProcessSTORERequest:存储数据过程完成解锁");
		lock.unlock();
		//确保日志信息实时地写入日志文件
		g_log->flush();
	}
	return statusCode;
}


OFCondition D_DcmStorageSCP::generateSTORERequestFilename(const T_DIMSE_C_StoreRQ &reqMessage,
	OFString &filename)
{
	OFString directoryName;
	OFString sopClassUID = reqMessage.AffectedSOPClassUID;
	OFString sopInstanceUID = reqMessage.AffectedSOPInstanceUID;
	// generate filename (with full path)
	OFCondition status = generateDirAndFilename(filename, directoryName, sopClassUID, sopInstanceUID);
	if (status.good())
	{
		DCMNET_DEBUG("generated filename for object to be received: " << filename);
		g_log->debug("D_DcmStorageSCP::generateSTORERequestFilename:生成文件:{}", (std::string)filename.c_str());
		// create the output directory (if needed)
		status = OFStandard::createDirectory(directoryName, OutputDirectory /* rootDir */);
		if (status.bad())
		{
			DCMNET_ERROR("cannot create directory for object to be received: " << directoryName << ": " << status.text());
			g_log->error("D_DcmStorageSCP::generateSTORERequestFilename:无法创建文件夹：{}:{}", (std::string)directoryName.c_str(), (std::string)status.text());
		}
	}
	else
	{
		DCMNET_ERROR("cannot generate directory or file name for object to be received: " << status.text());
		g_log->error("D_DcmStorageSCP::generateSTORERequestFilename:无法生成文件夹或者文件：{}", (std::string)status.text());
	}
	return status;
}


void D_DcmStorageSCP::notifyInstanceStored(const OFString &filename,
	const OFString & /*sopClassUID*/,
	const OFString & /*sopInstanceUID*/,
	DcmDataset * /*dataset*/) const
{
	// by default, output some useful information
	DCMNET_INFO("Stored received object to file: " << filename);
	g_log->info("D_DcmStorageSCP::notifyInstanceStored:保存对象到文件:{}", (std::string)filename.c_str());
}


OFCondition D_DcmStorageSCP::generateDirAndFilename(OFString &filename,
	OFString &directoryName,
	OFString &sopClassUID,
	OFString &sopInstanceUID,
	DcmDataset *dataset)
{
	OFCondition status = EC_Normal;
	// get SOP class and instance UID (if not yet known from the command set)
	if (dataset != NULL)
	{
		if (sopClassUID.empty())
			dataset->findAndGetOFString(DCM_SOPClassUID, sopClassUID);
		if (sopInstanceUID.empty())
			dataset->findAndGetOFString(DCM_SOPInstanceUID, sopInstanceUID);
	}
	// generate directory name
	OFString generatedDirName;
	switch (DirectoryGeneration)
	{
	case DGM_NoSubdirectory:
		// do nothing (default)
		break;
		// use series date (if available) for subdirectory structure
	case DGM_SeriesDate:
		if (dataset != NULL)
		{
			OFString seriesDate;
			DcmElement *element = NULL;
			// try to get the series date from the dataset
			if (dataset->findAndGetElement(DCM_SeriesDate, element).good() && (element->ident() == EVR_DA))
			{
				OFString dateValue;
				DcmDate *dateElement = OFstatic_cast(DcmDate *, element);
				// output ISO format is: YYYY-MM-DD
				if (dateElement->getISOFormattedDate(dateValue).good() && (dateValue.length() == 10))
				{
					OFOStringStream stream;
					stream << StandardSubdirectory << PATH_SEPARATOR
						<< dateValue.substr(0, 4) << PATH_SEPARATOR
						<< dateValue.substr(5, 2) << PATH_SEPARATOR
						<< dateValue.substr(8, 2) << OFStringStream_ends;
					OFSTRINGSTREAM_GETSTR(stream, tmpString)
						generatedDirName = tmpString;
					OFSTRINGSTREAM_FREESTR(tmpString);
				}
			}
			// alternatively, if that fails, use the current system date
			if (generatedDirName.empty())
			{
				OFString currentDate;
				status = DcmDate::getCurrentDate(currentDate);
				if (status.good())
				{
					OFOStringStream stream;
					stream << UndefinedSubdirectory << PATH_SEPARATOR
						<< currentDate << OFStringStream_ends;
					OFSTRINGSTREAM_GETSTR(stream, tmpString)
						generatedDirName = tmpString;
					OFSTRINGSTREAM_FREESTR(tmpString);
				}
			}
		}
		else {
			DCMNET_DEBUG("received dataset is not available in order to determine the SeriesDate "
				<< DCM_SeriesDate << ", are you using the bit preserving mode?");
			// no DICOM dataset given, so we cannot determine the series date
			status = EC_CouldNotGenerateDirectoryName;
		}
		break;
	}
	if (status.good())
	{
		// combine the generated directory name with the output directory
		OFStandard::combineDirAndFilename(directoryName, OutputDirectory, generatedDirName);
		// generate filename
		OFString generatedFileName;
		switch (FilenameGeneration)
		{
			// use modality prefix and SOP instance UID (default)
		case FGM_SOPInstanceUID:
		{
			if (sopClassUID.empty())
				status = NET_EC_InvalidSOPClassUID;
			else if (sopInstanceUID.empty())
				status = NET_EC_InvalidSOPInstanceUID;
			else {
				OFOStringStream stream;
				stream << dcmSOPClassUIDToModality(sopClassUID.c_str(), "UNKNOWN")
					<< '.' << sopInstanceUID << FilenameExtension << OFStringStream_ends;
				OFSTRINGSTREAM_GETSTR(stream, tmpString)
					generatedFileName = tmpString;
				OFSTRINGSTREAM_FREESTR(tmpString);
				// combine the generated file name with the directory name
				OFStandard::combineDirAndFilename(filename, directoryName, generatedFileName);
			}
			break;
		}
		// unique filename based on modality prefix and newly generated UID
		case FGM_UniqueFromNewUID:
		{
			char uidBuffer[70];
			dcmGenerateUniqueIdentifier(uidBuffer);
			OFOStringStream stream;
			stream << dcmSOPClassUIDToModality(sopClassUID.c_str(), "UNKNOWN")
				<< ".X." << uidBuffer << FilenameExtension << OFStringStream_ends;
			OFSTRINGSTREAM_GETSTR(stream, tmpString)
				generatedFileName = tmpString;
			OFSTRINGSTREAM_FREESTR(tmpString);
			// combine the generated file name with the directory name
			OFStandard::combineDirAndFilename(filename, directoryName, generatedFileName);
			break;
		}
		// unique pseudo-random filename (also checks for existing files, so we need some special handling)
		case FGM_ShortUniquePseudoRandom:
		{
			OFString prefix = dcmSOPClassUIDToModality(sopClassUID.c_str(), "XX");
			prefix += '_';
			// TODO: we might want to use a more appropriate seed value
			unsigned int seed = OFstatic_cast(unsigned int, time(NULL));
			if (!FilenameCreator.makeFilename(seed, directoryName.c_str(), prefix.c_str(), FilenameExtension.c_str(), filename))
				status = EC_CouldNotGenerateFilename;
			break;
		}
		// use current system time and modality suffix for filename
		case FGM_CurrentSystemTime:
		{
			OFString timeStamp;
			// get the date/time as: YYYYMMDDHHMMSS.FFFFFF
			if (DcmDateTime::getCurrentDateTime(timeStamp, OFTrue /* seconds */, OFTrue /* fraction */).good())
			{
				OFOStringStream stream;
				stream << timeStamp << '.' << dcmSOPClassUIDToModality(sopClassUID.c_str(), "UNKNOWN")
					<< FilenameExtension << OFStringStream_ends;
				OFSTRINGSTREAM_GETSTR(stream, tmpString)
					generatedFileName = tmpString;
				OFSTRINGSTREAM_FREESTR(tmpString);
				// combine the generated file name
				OFStandard::combineDirAndFilename(filename, directoryName, generatedFileName);
			}
			else
				status = EC_CouldNotGenerateFilename;
			break;
		}

		}
	}
	return status;
}
