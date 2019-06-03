//// b2.cpp : 此文件包含 "main" 函数
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
static const OFString dir = "C:/Users/zhujianghui/Downloads/接收后的DCM文件/";
//源文件
static const string sourcepath = "C:\\Users\\zhujianghui\\Downloads\\dcm文件\\";
//共享队列的大小：
static const unsigned int len = 5;
/*
从客户端和服务端协商来判断服务端是否支持服务端所声明的传输语法
返回0表示不符合。
*/
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
	// 设置实体
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
	//设置服务端实体名称和服务端的超时时间和阻塞模式
	D_DcmStorageSCP mStoreSCP;
	mStoreSCP.setAETitle(PEERAPPLICATIONTITLE.c_str());
	mStoreSCP.setPort(PEERPORT);
	mStoreSCP.setVerbosePCMode(true);
	mStoreSCP.setACSETimeout(ACSETimeout);
	mStoreSCP.setDIMSETimeout(DIMSETimeout);
	mStoreSCP.setDIMSEBlockingMode(DIMSE_NONBLOCKING);
	//设置传输语法，主要是压缩算法的协商
	OFList< OFString > transferSyntaxes;
	transferSyntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
	transferSyntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);
	transferSyntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
	mStoreSCP.addPresentationContext(UID_CTImageStorage, transferSyntaxes);
	mStoreSCP.addPresentationContext(UID_MRImageStorage, transferSyntaxes);
	//设置保存接收的数据的保存的文件的格式和路径
	const OFString extension = ".dcm";
	mStoreSCP.setFilenameExtension(extension);
	mStoreSCP.setOutputDirectory(dir);
	//进行监听
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
	//日志记录的全局变量 g_log,等级为debug；此变量在global.h中已经定义了。
	g_log->set_level(spdlog::level::debug);
	/*
	这是生产者-消费者模型中的共享内存；用于存储需要解析文件的绝对路径，使用了锁机制
	在SharedBuf.h中进行了声明；此共享内存的长度有len指定
	*/
	SharedBuf srb;
	srb.size = len;
	//服务进程，用于接收网络的DICOM协议内容；并且保存成文件，并将文件的绝对路径写到了共享内存中
	thread p(testScp, std::ref(srb));
	//模拟的客户端连接
	thread c(testScu, sourcepath+"118.DCM");
	thread c1(testScu, sourcepath + "119.DCM");
	thread c2(testScu, sourcepath + "120.DCM");
	thread c3(testScu, sourcepath + "121.DCM");
	thread c4(testScu, sourcepath + "124.DCM");
	thread c5(testScu, sourcepath + "125.DCM");
	thread c6(testScu, sourcepath + "126.DCM");
	thread c7(testScu, sourcepath + "230.DCM");
	thread c8(testScu, sourcepath + "231.DCM");
	thread c9(testScu, sourcepath + "291.DCM");
	thread c10(testScu, sourcepath + "292.DCM");
	thread c11(testScu, sourcepath + "293.DCM");
	thread c12(testScu, sourcepath + "294.DCM");
	thread c13(testScu, sourcepath + "295.DCM");
	thread c14(testScu, sourcepath + "296.DCM");
	thread c15(testScu, sourcepath + "297.DCM");
	thread c16(testScu, sourcepath + "298.DCM");
	thread c17(testScu, sourcepath + "299.DCM");
	//生产者-消费者模式中的消费者的角色；专门用于解析DCM文件；文件的路径是在共享内存中读取出来。
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



//WT框架的测试：
//#include "database.h"
//#include "MysqlResToT.h"
//#include <Wt/WApplication.h>
//#include <Wt/WBreak.h>
//#include <Wt/WContainerWidget.h>
//#include <Wt/WLineEdit.h>
//#include <Wt/WPushButton.h>
//#include <Wt/WText.h>
//#include <Wt/WContainerWidget.h>
//#include <Wt/WFileUpload.h>
//#include <Wt/WProgressBar.h>
//#include <Wt/WPushButton.h>
//
//
//class HelloApplication : public Wt::WApplication
//{
//	public:
//	HelloApplication(const Wt::WEnvironment& env);
//};
////
//HelloApplication::HelloApplication(const Wt::WEnvironment& env): Wt::WApplication(env)
//{
//	setTitle("文件上传");
//	auto container = root()->addWidget(Wt::cpp14::make_unique<Wt::WContainerWidget>());
//	Wt::WFileUpload *fu = container->addWidget(Wt::cpp14::make_unique<Wt::WFileUpload>());
//	fu->setFileTextSize(10000); // Set the maximum file size to 10000 kB.
//	fu->setProgressBar(Wt::cpp14::make_unique<Wt::WProgressBar>());
//	fu->setMargin(10, Wt::Side::Right);
//	cout << "设置的大小：" << endl;
//	cout<<fu->fileTextSize() << endl;
//	// Provide a button to start uploading.
//	Wt::WPushButton *uploadButton =
//		container->addWidget(Wt::cpp14::make_unique<Wt::WPushButton>("Send"));
//	uploadButton->setMargin(10, Wt::Side::Left | Wt::Side::Right);
//
//	Wt::WText *out = container->addWidget(Wt::cpp14::make_unique<Wt::WText>());
//		
//	// Upload automatically when the user entered a file.
//	fu->changed().connect([=] {
//		fu->upload();
//		uploadButton->disable();
//		out->setText("File upload is changed.");
//	});
//
//	// React to a succesfull upload.
//	fu->uploaded().connect([=] {
//		out->setText("File upload is finished.");
//		cout << "文件名:" << endl;
//		cout<<fu->clientFileName();
//		cout << "文件大小：" << endl;
//		cout << fu->fileTextSize() << endl;
//		
//			
//	});
//	fu->dataReceived().connect([=] {
//		cout << "文件上传成功了" << endl;
//		
//	});
//	// React to a file upload problem.
//	fu->fileTooLarge().connect([=] {
//		out->setText("File is too large.");
//	});
//
//}
//int main(int argc, char **argv)
//{
//	int num = 7;
//	//强制的类型转化；
//	char *pr[7] = {const_cast<char*>("b2.exe"),const_cast<char*>("--docroot"),const_cast<char*>("."),const_cast<char*>("--http-address"),const_cast<char*>("0.0.0.0"),const_cast<char*>("--http-port"),const_cast<char*>("9090")};
//	return Wt::WRun(num, pr, [](const Wt::WEnvironment& env) {
//		return std::make_unique<HelloApplication>(env);
//	});
//}