// b2.cpp : 此文件包含 "main" 函数
#include "pch.h"
//DcmTest文件的测试：
#include <string>
#include <iostream>
#include <thread>
#include <memory>
#include <time.h>
#include "dcmtk/config/osconfig.h"/* make sure OS specific configuration is included first */ 
#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmnet/assoc.h"
#include "dcmtk/dcmnet/diutil.h" 
#include "D_DcmStorageSCP.h"
#include "DcmTest.h" 
#include "analysisDcm.h"
#include "database.h"
#include "MysqlResToT.h"
#include "global.h"
using namespace std;

static const string OFFIS_CONSOLE_APPLICATION = "testscu";
// our application entity title used for calling the peer machine 
static const string  APPLICATIONTITLE = "TEST-SCU";
// host name of the peer machine 
static const string PEERHOSTNAME = "localhost";
// TCPIP port to connect to the peer machine 
static const unsigned int PEERPORT = 9999;
//scp AE title
static const string PEERAPPLICATIONTITLE = "MOVESCP";
// MOVE destination AE Title 
static const string MOVEAPPLICATIONTITLE = "TEST-SCU";
//DIMSETimeout
static const int DIMSETimeout = 10000;
//ACSETimeout
static const int ACSETimeout = -1;
//MYSQL
static const string host = "localhost";
static const string root = "root";
static const string passwd = "525369";
static const string db = "b2";
//存储图片的文件夹路径
static const string dirpath = "C:/Users/zhujianghui/Downloads/dcm-images/";
//存储接收到的dcm信息的文件夹路径
static const OFString dir = "E:/DCM/";
//共享队列的大小：
static const unsigned int len = 5;

static Uint8 findUncompressedPC(const OFString& sopClass,
	DcmSCU& scu)
{
	Uint8 pc;
	pc = scu.findPresentationContextID(sopClass, UID_LittleEndianExplicitTransferSyntax);
	if (pc == 0)
		pc = scu.findPresentationContextID(sopClass, UID_BigEndianExplicitTransferSyntax);
	if (pc == 0)
		pc = scu.findPresentationContextID(sopClass, UID_LittleEndianImplicitTransferSyntax);
	return pc;
}

static shared_ptr<DataBase> initMysqlConn()
{
	static shared_ptr<DataBase> myconn(new DataBase(host, root, passwd, db));
	if (myconn->InitConnection())
	{
		g_log->info("main initMysqlConn():数据库初始化成功");
		cout << "数据库初始化成功" << endl;
	}
	else {
		g_log->debug("main initMysqlConn():数据库初始化失败");
		cout << "数据库初始化失败" << endl;
	}
	return myconn;
}
int testScu(string filepath)
{
	//cout << "testScu" << endl;
	g_log->info("main testScu:进入函数");
	DcmTestSCU  scu;
	// set AE titles 
	scu.setAETitle(APPLICATIONTITLE.c_str());
	scu.setPeerHostName(PEERHOSTNAME.c_str());
	scu.setPeerPort(PEERPORT);
	scu.setPeerAETitle(PEERAPPLICATIONTITLE.c_str());
	// Use presentation context for FIND/MOVE in study root, propose all uncompressed transfer syntaxes 
	OFList<OFString> ts;
	ts.push_back(UID_LittleEndianExplicitTransferSyntax);
	ts.push_back(UID_BigEndianExplicitTransferSyntax);
	ts.push_back(UID_LittleEndianImplicitTransferSyntax);
	scu.addPresentationContext(UID_CTImageStorage, ts);
	scu.addPresentationContext(UID_MRImageStorage, ts);
	/* Initialize network */
	OFCondition result = scu.initNetwork();
	if (result.bad())
	{
		DCMNET_ERROR("Unable to set up the network: " << result.text());
		g_log->error("main testScu: Unable to set up the network:{}",(string)result.text());
		return 1;
	}

	/* Negotiate Association */
	result = scu.negotiateAssociation();
	if (result.bad())
	{
		DCMNET_ERROR("Unable to negotiate association: " << result.text());
		g_log->error("main testScu:Unable to negotiate association:{}",result.text());
		return 1;
	}

	//C-STORE by myself;
	T_ASC_PresentationContextID presID = findUncompressedPC(UID_CTImageStorage, scu);
	if (presID == 0)
	{
		DCMNET_ERROR("UID_CTImageStorage fail");
		g_log->error("main testScu:UID_CTImageStorage fail");
		return 1;
	}
	Uint16 rspStatusCode;
	OFString file =(OFString)filepath.c_str();
	result = scu.sendSTORERequest(presID, file, NULL, rspStatusCode);
	if (result.bad())
	{
		DCMNET_ERROR("C-STORE FAIL");
		g_log->error("main testScu:C-STORE FAIL");
		return 1;
	}
	if (rspStatusCode != 0)
	{
		cout << "rspStatusCode:" << rspStatusCode;
		g_log->debug("main testScu:rspStatusCode:{}", rspStatusCode);
	}
	scu.closeAssociation(DCMSCU_RELEASE_ASSOCIATION);
	return 0;
}

void testScp(SharedBuf& srb)
{
	g_log->info("main testScp:进入函数");
	cout << "testScp" << endl;
	D_DcmStorageSCP mStoreSCP;
	mStoreSCP.setAETitle(PEERAPPLICATIONTITLE.c_str());
	mStoreSCP.setPort(PEERPORT);
	mStoreSCP.setVerbosePCMode(true);
	mStoreSCP.setACSETimeout(ACSETimeout);
	mStoreSCP.setDIMSETimeout(DIMSETimeout);
	mStoreSCP.setDIMSEBlockingMode(DIMSE_NONBLOCKING);
	OFList< OFString > transferSyntaxes;
	transferSyntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
	transferSyntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);
	transferSyntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
	mStoreSCP.addPresentationContext(UID_CTImageStorage, transferSyntaxes);
	mStoreSCP.addPresentationContext(UID_MRImageStorage, transferSyntaxes);
	const OFString extension = ".dcm";
	mStoreSCP.setFilenameExtension(extension);
	mStoreSCP.setOutputDirectory(dir);
	mStoreSCP.listen(srb);
}
void anasyDcmfile(SharedBuf& srb)
{
	while (true)
	{
		g_log->info("main anasyDcmfile:进入函数");
		cout << "anasyDcmfile" << endl;
		unique_lock<mutex> lock(srb.mtx);
		g_log->info("main anasyDcmfile:上锁");
		cout << "anasyDcmfile: 上锁" << endl;
		while (srb.q.empty())
		{
			cout << "empty" << endl;
			g_log->info("main anasyDcmfile:共享队列为空，等待.....");
			srb.not_empty.wait(lock);
		}
		string filepath = srb.q.front();
		srb.q.pop();
		g_log->info("main anasyDcmfile:取出队列中的文件路径;{}", filepath);
		srb.not_full.notify_all();
		cout << "anasyDcmfile: 解锁" << endl;
		g_log->info("main anasyDcmfile:解锁");
		lock.unlock();
		AnalysisDcm* dcm = new AnalysisDcm(filepath);
		dcm->InitDcmFile();
		PatientInfo *p = dcm->GetPatientInfo();
		StudyInfo *s = dcm->GetStudyInfo();
		SeriesInfo *ss = dcm->GetSeriesInfo();
		ImageInfo* i = dcm->GetImageInfo();
		shared_ptr<DataBase> myconn = initMysqlConn();
		MysqlResToT *t = new MysqlResToT();

		string insertPatient = t->PatientInfoToSql((*p));
		cout << "插入病人信息SQL：" << insertPatient << endl;
		g_log->info("main anasyDcmfile:插入patientinfo：{}", insertPatient);
		if (myconn->DoInsert(insertPatient))
		{
			cout << "插入病人信息成功" << endl;
			g_log->info("main anasyDcmfile:插入patientinfo成功");
			int id = myconn->GetTableId();
			if (id == -1)
			{
				g_log->debug("main anasyDcmfile:查询patientinfo的id失败");
				cout << "查询id出错或者失败" << endl;
			}
			//设置刚插入产生的自增的id
			p->SetId(id);
			//设置studyinfo表中的patientid;
			s->SetPatientId(id);
			cout << (*p) << endl;
		}
		else {
			cout << "插入病人信息失败" << endl;
			g_log->error("main anasyDcmfile:插入patientinfo失败");
		}

		string insertStudy = t->StudyInfoToSql((*s));
		g_log->info("main anasyDcmfile:插入studyinfo:{}", insertStudy);
		if (myconn->DoInsert(insertStudy))
		{
			g_log->info("main anasyDcmfile:插入studyinfo成功");
			cout << "插入检查信息成功" << endl;
			int id = myconn->GetTableId();
			if (id == -1)
			{
				g_log->error("main anasyDcmfile:查询studyinfo的id失败");
				cout << "查询id出错或者失败" << endl;
			}
			//设置刚插入产生的自增的id
			s->SetId(id);
			//设置seriesinfo表中的studyid;
			ss->SetStudyId(id);
			cout << (*s) << endl;
		}
		else {
			g_log->error("main anasyDcmfile:插入studyinfo失败");
			cout << "插入检查信息失败" << endl;
		}


		string insertSeries = t->SeriesInfoToSql((*ss));
		g_log->info("main anasyDcmfile:插入seriesinfo:{}", insertSeries);
		cout << "插入序列信息SQL：" << insertSeries << endl;
		if (myconn->DoInsert(insertSeries))
		{
			g_log->info("main anasyDcmfile:插入seriesinfo成功");
			cout << "插入序列信息成功" << endl;
			int id = myconn->GetTableId();
			if (id == -1)
			{
				g_log->error("main anasyDcmfile:查询seriesinfo的id失败");
				cout << "查询id出错或者失败" << endl;
			}
			//设置刚插入产生的自增的id
			ss->SetId(id);
			//设置imageinfo表中的seriesid;
			i->SetSeriesId(id);
			cout << (*ss) << endl;
		}
		else {
			g_log->error("main anasyDcmfile:插入seriesinfo失败");
			cout << "插入序列信息失败" << endl;
		}
		//保存图片
		dcm->SaveDcmToPng(dirpath +to_string(ss->GetId()) +".png");
		i->SetImagePath(dirpath + to_string(ss->GetId()) + ".png");
		string insertImage = t->ImageInfoToSql((*i));
		cout << "插入图像信息SQL：" << insertImage << endl;
		g_log->info("main anasyDcmfile:插入图像信息：{}",insertImage);
		if (myconn->DoInsert(insertImage))
		{
			cout << "插入图像信息成功" << endl;
			g_log->info("main anasyDcmfile:插入imageinfo成功");
			int id = myconn->GetTableId();
			if (id == -1)
			{
				g_log->debug("main anasyDcmfile：查询imageinfo的id失败");
				cout << "查询id出错或者失败" << endl;
			}
			i->SetId(id);
			cout << (*i) << endl;
		}
		else {
			g_log->info("main anasyDcmfile:插入imageinfo信息失败");
			cout << "插入图像信息失败" << endl;
		}
		g_log->flush();
		delete dcm;
		delete t;
	}
}
int main(int argc, char* argv[])
{
	g_log->set_level(spdlog::level::debug);
	SharedBuf srb;
	srb.size = len;
	thread p(testScp, std::ref(srb));
	thread c(testScu, "C:\\Users\\zhujianghui\\Downloads\\dcm文件\\126.DCM");
	thread c1(testScu,"C:\\Users\\zhujianghui\\Downloads\\dcm文件\\121.DCM");
	thread c2(testScu,"C:\\Users\\zhujianghui\\Downloads\\dcm文件\\125.DCM");
	thread c3(testScu, "C:\\Users\\zhujianghui\\Downloads\\dcm文件\\111.DCM");
	thread c4(testScu, "C:\\Users\\zhujianghui\\Downloads\\dcm文件\\120.DCM");
	thread c5(testScu, "C:\\Users\\zhujianghui\\Downloads\\dcm文件\\124.DCM");

	thread a(anasyDcmfile, std::ref(srb));
	c1.join();
	c2.join();
	c3.join();
	p.join();
	c.join();
	c4.join();
	c5.join();
	a.join();
	return 0;
}