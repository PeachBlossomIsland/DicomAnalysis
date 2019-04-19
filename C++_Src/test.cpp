#include "pch.h"
//mysql数据库的测试：

//#include <iostream>  
//#include <stdio.h>  
//#include <stdlib.h>  
//#include <string.h>  
//
////下面两个头文件的顺序不能颠倒  
//#include "winsock.h"  
//#include "windows.h"  
//#include "mysql.h"
//
//using namespace std;
//
//int main()
//{
//	const char host[] = "localhost";    //MySQL服务器IP地址；若是本地可填写“localhost”或127.0.0.1  
//	const char user[] = "root";         //MySQL的用户名  
	//const char pswd[] = "525369";         //密码  
	//const char table[] = "we";        //数据库名称  
	//unsigned int port = 3306;           //MySQL服务端口号，默认是3306          
	//MYSQL myCont;//创建MYSQL对象，定义数据库连接句柄  
	//MYSQL_RES *result=NULL;//查询结果集，存放查询结果  
	//MYSQL_ROW sql_row;//存放一行查询结果的字符串数组  
	//MYSQL_FIELD *fd; // 包含字段信息的结构
	//char column[32][32];
	//int res;
	//mysql_library_init(0, NULL, NULL);//初始化MySQL库  
	//mysql_init(&myCont);//初始化连接处理程序  
	//if (mysql_real_connect(&myCont, host, user, pswd, table, port, NULL, 0))
	//{//通过调用mysql_real_connect()连接到服务器  
	//	cout << "connect succeed!" << endl;
	//	mysql_query(&myCont, "SET NAMES GBK"); //设置编码格式,否则在cmd下无法显示中文  
	//	res = mysql_query(&myCont, "select * from w1");//执行查询语句，mysql_query如果查询成功，零；如果出现一个错误，非零。  
	//	if (!res)
	//	{
			//result = mysql_store_result(&myCont);//保存查询到的数据到result  
			//if (result)
			//{
			//	int i, j;
			//	cout << "number of result: " << (unsigned long)mysql_num_rows(result) << endl;
			//	for (i = 0; fd = mysql_fetch_field(result); i++)//获取列名  
			//	{
			//		strcpy_s(column[i], fd->name);
			//	}
			//	j = mysql_num_fields(result);
			//	for (i = 0; i < j; i++)
			//	{
			//		printf("%s\t", column[i]);
			//	}
			//	printf("\n");
			//	while (sql_row = mysql_fetch_row(result))//获取具体的数据  
			//	{
			//		for (i = 0; i < j; i++)
					/*{
						printf("%s\n", sql_row[i]);
					}
					printf("\n");
				}
			}
		}
		else
		{
			cout << "query sql failed!" << endl;
		}
	}*/
	//	else
	//	{
	//		cout << "connect failed!" << endl;
	//	}
	//	//注意用完数据库要及时回收资源  
	//	if (result != NULL) mysql_free_result(result);//释放结果资源  
	//	mysql_close(&myCont);//关闭MySQL连接  
	//	mysql_library_end();//关闭MySQL库  
	//	return 0;
	//}



	//dcm文件解析及存储病历图片的测试：

	//#include "dcmtk/dcmdata/dctk.h"
	//#include "dcmtk/dcmimgle/dcmimage.h"
	//#include "dcmtk/dcmdata/dcfilefo.h"
	//#include <opencv2/opencv.hpp>
	//using namespace std;
	//int main()
	//{
	//	DcmFileFormat dfile;
	//	string path = "C:\\Users\\zhujianghui\\Downloads\\111.dcm";
	//	OFCondition result = dfile.loadFile(path.c_str()); // 加载文件
	//	if (result.bad())
	//		return ERROR;
	//	DcmDataset *data = dfile.getDataset();
	//	DcmElement *element = NULL;
	//	result = data->findAndGetElement(DCM_PixelData, element);
	//	if (result.bad())
	//		return ERROR;
	//	Uint16* pixData16;
	//	//Uint32* pixDate32;
	//	result = element->getUint16Array(pixData16);
	//	//result = element->getUint16Array(pixData16);
	//	if (result.bad())
	//		return ERROR;
	//	cv::Mat imag = cv::Mat(512, 512, CV_16U, pixData16);
	//	cv::imwrite("./output1.jpg", imag);
	//	//IplImage qImg;
	//	//qImg = IplImage(imag); // cv::Mat -> IplImage
	//	//cvSaveImage("./out.jpg", &qImg);
	//	//cv::imshow("image", imag);
	//	//cv::waitKey();
	//}





	//dcmtk包的测试：

	//#include"dcmtk/config/osconfig.h"
	//
	//#include"dcmtk/dcmdata/dctk.h"
	//
	//#include<iostream>
	//
	//#include <typeinfo>

	//using namespace std;

	//int main()
	//
	//{
	//
	//	DcmFileFormat fileformat;
	//
	//	OFCondition oc = fileformat.loadFile("C:\\Users\\zhujianghui\\Downloads\\111.dcm");
	//
	//	if (oc.good()) {
	//
	//		OFString patientName,patientData,patientDataTime;
	//
	//		if (fileformat.getDataset()->findAndGetOFString(DCM_PatientName, patientName).good())
	//
	//		{
	//
	//			cout << "Patient Name:" << patientName.data() << endl;
	//			cout << typeid(patientName.data()).name() << endl;
	//			string r = patientName.data();
	//			cout << r << endl;
	//			
	//		}
	//		if (fileformat.getDataset()->findAndGetOFString(DCM_PatientWeight, patientData).good())
	//		{
	//			cout << "Patient Name:" << patientData.data() << endl;
	//			cout << typeid(patientData.data()).name() << endl;
	//		}
	//		/*if (fileformat.getDataset()->findAndGetOFString(DCM_AcquisitionDateTime, patientDataTime).good())
	//		{
	//			cout << "Patient Name:" << patientDataTime.data() << endl;
	//		}
	//		if (fileformat.getDataset()->findAndGetOFString(DCM_PatientBirthName, patientDataTime).good())
	//		{
	//			cout << "Patient Name:" << patientDataTime.data() << endl;
	//		}
	//		if (fileformat.getDataset()->findAndGetOFString(DCM_AcquisitionDateTime, patientDataTime).good())
	//		{
	//			cout << "Patient Name:" << patientDataTime.data() << endl;
	//		}*/
	//	}
	//	system("pause");
	//	return 0;
	//
	//}
	//opencv包的测试：

	//#include "cv.h";
	//#include "highgui.h";
	//#include "cxcore.h";
	//int main(int argc, char *argv[])
	//{
	//	IplImage* src = cvLoadImage("C:\\Users\\zhujianghui\\Pictures\\Camera Roll\\622424199610153719朱江辉.jpg");
	//	cvNamedWindow("win");
	//	cvShowImage("win", src);
	//	cvWaitKey(0);
	//	cvReleaseImage(&src);
	//	cvDestroyWindow("win");
	//	return 0;
	//}



	//dcm文件解析测试：
	//#include <boost/lexical_cast.hpp>       
	//#include <iostream>

	//using namespace std;

	//int main()
	//{
	//	using boost::lexical_cast;
	//	int a = lexical_cast<int>("123");
	//	double b = lexical_cast<double>("123.0123456789");
	//	string s0 = lexical_cast<string>(a);
	//	string s1 = lexical_cast<string>(b);
	//	cout << "number: " << a << "  " << b << endl;
	//	cout << "string: " << s0 << "  " << s1 << endl;
	//	int c = 0;
	//	try {
	//		c = lexical_cast<int>("abcd");
	//	}
	//	catch (boost::bad_lexical_cast& e) {
	//		cout << e.what() << endl;
	//	}
	//
	//	return 0;
	//}

	//cinatra框架的测试：
	//#include "cinatra/http_server.hpp"
	//using namespace cinatra;
	//int main() {
	//	int max_thread_num = std::thread::hardware_concurrency();
	//	http_server server(max_thread_num);
	//	server.listen("0.0.0.0", "8080");
	//	server.set_http_handler<GET, POST>("/", [](request& req, response& res) {
	//		res.set_status_and_content(status_type::ok, "hello world");
	//	});
	//	server.run();
	//	return 0;
	//}


	//WT框架的测试：
	//#include "database.h"
	//#include "MysqlResToT.h"
	//#include <Wt/WApplication.h>
	//#include <Wt/WBreak.h>
	//#include <Wt/WContainerWidget.h>
	//#include <Wt/WLineEdit.h>
	//#include <Wt/WPushButton.h>
	//#include <Wt/WText.h>

	//class HelloApplication : public Wt::WApplication
	//{
	//public:
	//	HelloApplication(const Wt::WEnvironment& env);
	//
	//private:
	//	Wt::WLineEdit *nameEdit_;
	//	Wt::WText *greeting_;
	//};

	////	: Wt::WApplication(env)
	//{


	//	setTitle("Hello world");
	//	root()->addWidget(std::make_unique<Wt::WText>("Your name, please? "));
	//	nameEdit_ = root()->addWidget(std::make_unique<Wt::WLineEdit>());
	//	Wt::WPushButton *button = root()->addWidget(std::make_unique<Wt::WPushButton>("Greet me."));
	//	root()->addWidget(std::make_unique<Wt::WBreak>());
	//	greeting_ = root()->addWidget(std::make_unique<Wt::WText>());
	//
	//	auto greet = [this] {
	//		
	//		//cout << "insertImage:" << insertImage << endl;
	//		greeting_->setText("Hello there, " + nameEdit_->text());
	//	};
	//	button->clicked().connect(greet);
	//}

	//int main(int argc, char **argv)
	//{
	//	return Wt::WRun(argc, argv, [](const Wt::WEnvironment& env) {
	//		return std::make_unique<HelloApplication>(env);
	//	});
	//}


//	/* Let's look whether the server is listening:
//	   Assemble and send C-ECHO request
//	 */
//	/*result = scu.sendECHORequest(0);
//	if (result.bad())
//	{
//		DCMNET_ERROR("Could not process C-ECHO with the server: " << result.text());
//		return 1;
//	}*/
//
//	/* Assemble and send C-FIND request */
//	/*OFList<QRResponse*> findResponses;
//	DcmDataset req;
//	req.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "STUDY");
//	req.putAndInsertOFStringArray(DCM_StudyInstanceUID, "");
//	T_ASC_PresentationContextID presID = findUncompressedPC(UID_FINDStudyRootQueryRetrieveInformationModel, scu);
//	if (presID == 0)
//	{
//		DCMNET_ERROR("There is no uncompressed presentation context for Study Root FIND");
//		return 1;
//	}
//	result = scu.sendFINDRequest(presID, &req, &findResponses);
//	if (result.bad())
//		return 1;
//	else
//		DCMNET_INFO("There are " << findResponses.size() << " studies available");
//*/
//	/* Assemble and send C-MOVE request, for each study identified above*/
//	//presID = findUncompressedPC(UID_MOVEStudyRootQueryRetrieveInformationModel, scu);
//	//if (presID == 0)
//	//{
//	//	DCMNET_ERROR("There is no uncompressed presentation context for Study Root MOVE");
//	//	return 1;
//	//}
//	//OFListIterator(QRResponse*) study = findResponses.begin();
//	//Uint32 studyCount = 1;
//	//OFBool failed = OFFalse;
//	//// Every while loop run will get all image for a specific study 
//	//while (study != findResponses.end() && result.good())
//	//{
//	//	// be sure we are not in the last response which does not have a dataset 
//	//	if ((*study)->m_dataset != NULL)
//	//	{
//	//		OFString studyInstanceUID;
//	//		result = (*study)->m_dataset->findAndGetOFStringArray(DCM_StudyInstanceUID, studyInstanceUID);
//	//		// only try to get study if we actually have study instance uid, otherwise skip it 
//	//		if (result.good())
//	//		{
//	//			req.putAndInsertOFStringArray(DCM_StudyInstanceUID, studyInstanceUID);
//	//			// fetches all images of this particular study 
//	//			result = scu.sendMOVERequest(presID, MOVEAPPLICATIONTITLE, &req, NULL /* we are not interested into responses*/);
//	//			if (result.good())
//	//			{
//	//				DCMNET_INFO("Received study #" << std::setw(7) << studyCount << ": " << studyInstanceUID);
//	//				studyCount++;
//	//			}
//	//		}
//	//	}
//	//	study++;
//	//}
//	//if (result.bad())
//	//{
//	//	DCMNET_ERROR("Unable to retrieve all studies: " << result.text());
//	//}
//	//while (!findResponses.empty())
//	/*{
//		delete findResponses.front();
//		findResponses.pop_front();
//	}*/
//
//	/* Release association */
//	



/*
	//新尝试的一种方式：
	DcmDataset *dataset = this->dcmFile.getDataset();
	dataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL);
	unsigned short m_width;														//获取图像的窗宽高
	unsigned short m_height;
	dataset->findAndGetUint16(DCM_Rows, m_height);
	dataset->findAndGetUint16(DCM_Columns, m_width);
	E_TransferSyntax xfer = dataset->getOriginalXfer();
	DicomImage* m_dcmImage = new DicomImage((DcmObject*)dataset, xfer); //利用dataset生成DicomImage，需要上面的解压方法；
	unsigned char *pixelData = (unsigned char*)(m_dcmImage->getOutputData(8 , 0, 0));	//获得16位的图像数据指针
	if (pixelData != NULL)
	{
		cv::Mat imag(m_height, m_width, CV_8UC3, cv::Scalar::all(0));
		unsigned short* data = nullptr;
		for (int i = 0; i < m_height; i++)
		{
			//data = dst2.ptr<unsigned short>(i);	//取得每一行的头指针 也可使用dst2.at<unsigned short>(i, j) = ?
			for (int j = 0; j < m_width; j++)
			{
				//imag.at<cv::Vec3b>(i, j)[0] = *(pixelData + i * m_width * 3 + j * 3 + 2);	//B	channel
				//imag.at<cv::Vec3b>(i, j)[1] = *(pixelData + i * m_width * 3 + j * 3 + 1);	//G channel
				//imag.at<cv::Vec3b>(i, j)[2] = *(pixelData + i * m_width * 3 + j * 3);		//R channel
				imag.at<cv::Vec3b>(i, j)[0] = *(pixelData + i * m_width  + j );	//B	channel
				imag.at<cv::Vec3b>(i, j)[1] = *(pixelData + i * m_width  + j );	//G channel
				imag.at<cv::Vec3b>(i, j)[2] = *(pixelData + i * m_width  + j );		//R channel
			}
		}
		vector<int> compression_params;
		compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION); //PNG格式图片的压缩级别
		compression_params.push_back(9);  //这里设置保存的图像质量级别
		g_log->info("AnalysisDcm::SaveDcmToPng: 开始图像保存");
		cv::imwrite(path, imag, compression_params);
		g_log->info("AnalysisDcm::SaveDcmToPng: 图像保存成功：{}", path);
		g_log->flush();
		return true;
	}
	*/
	//有异常的读取方式：
	/*
		DcmDataset *data = this->dcmFile.getDataset();
		//如果压缩的话，解压
		data->chooseRepresentation(EXS_LittleEndianExplicit, NULL);
		DcmElement *element = NULL;
		OFCondition result = data->findAndGetElement(DCM_PixelData, element);
		if (result.bad())
			return false;
		Uint16* pixData16;
		result=element->getUint16Array(pixData16);
		if (result.bad())
			return false;
		//cv::Mat imag = cv::Mat(1024,1024,CV_64F, pixData64);
		//此处有报异常的情况：处理方式：https://cloud.tencent.com/developer/article/1010015
		cv::Mat imag = cv::Mat(512, 512, CV_16U, pixData16);
		vector<int> compression_params;
		compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION); //PNG格式图片的压缩级别
		compression_params.push_back(9);  //这里设置保存的图像质量级别
		g_log->info("AnalysisDcm::SaveDcmToPng: 开始图像保存");
		cv::imwrite(path, imag, compression_params);
		g_log->info("AnalysisDcm::SaveDcmToPng: 图像保存成功：{}", path);
		g_log->flush();
		return true;

	*/


	/*
		dcm->SaveDcmToPng(dirpath + to_string(i->GetId()) + ".png");
		DcmInfo *di = new DcmInfo();
		di->SetImageId(i->GetId());
		di->SetPatientId(p->GetId());
		di->SetSeriesId(ss->GetId());
		di->SetStudyId(s->GetId());
		di->SetImagePath(dirpath + to_string(i->GetId()) + ".png");
		string insertDcm = t->DcmInfoToSql((*di));
		g_log->info("main anasyDcmfile:插入dcminfo:{}",insertDcm);
		cout << "插入dcm信息sql语句：" << insertDcm << endl;
		if (myconn->DoInsert(insertDcm))
		{
			cout << "插入dcm信息成功" << endl;
			g_log->info("main anasyDcmfile:插入dcminfo成功");
			int id = myconn->GetTableId();
			if (id == -1)
			{
				g_log->error("main anasyDcmfile:查询dcminfo的id失败");
				cout << "查询id出错或者失败" << endl;
			}
			di->SetId(id);
			cout << (*di) << endl;
		}
		else {
			g_log->error("main anasyDcmfile:插入dcmInfo失败");
			cout << "插入dcm信息失败" << endl;
		}
		*/