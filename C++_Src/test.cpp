#include "pch.h"
//mysql���ݿ�Ĳ��ԣ�

//#include <iostream>  
//#include <stdio.h>  
//#include <stdlib.h>  
//#include <string.h>  
//
////��������ͷ�ļ���˳���ܵߵ�  
//#include "winsock.h"  
//#include "windows.h"  
//#include "mysql.h"
//
//using namespace std;
//
//int main()
//{
//	const char host[] = "localhost";    //MySQL������IP��ַ�����Ǳ��ؿ���д��localhost����127.0.0.1  
//	const char user[] = "root";         //MySQL���û���  
	//const char pswd[] = "525369";         //����  
	//const char table[] = "we";        //���ݿ�����  
	//unsigned int port = 3306;           //MySQL����˿ںţ�Ĭ����3306          
	//MYSQL myCont;//����MYSQL���󣬶������ݿ����Ӿ��  
	//MYSQL_RES *result=NULL;//��ѯ���������Ų�ѯ���  
	//MYSQL_ROW sql_row;//���һ�в�ѯ������ַ�������  
	//MYSQL_FIELD *fd; // �����ֶ���Ϣ�Ľṹ
	//char column[32][32];
	//int res;
	//mysql_library_init(0, NULL, NULL);//��ʼ��MySQL��  
	//mysql_init(&myCont);//��ʼ�����Ӵ������  
	//if (mysql_real_connect(&myCont, host, user, pswd, table, port, NULL, 0))
	//{//ͨ������mysql_real_connect()���ӵ�������  
	//	cout << "connect succeed!" << endl;
	//	mysql_query(&myCont, "SET NAMES GBK"); //���ñ����ʽ,������cmd���޷���ʾ����  
	//	res = mysql_query(&myCont, "select * from w1");//ִ�в�ѯ��䣬mysql_query�����ѯ�ɹ����㣻�������һ�����󣬷��㡣  
	//	if (!res)
	//	{
			//result = mysql_store_result(&myCont);//�����ѯ�������ݵ�result  
			//if (result)
			//{
			//	int i, j;
			//	cout << "number of result: " << (unsigned long)mysql_num_rows(result) << endl;
			//	for (i = 0; fd = mysql_fetch_field(result); i++)//��ȡ����  
			//	{
			//		strcpy_s(column[i], fd->name);
			//	}
			//	j = mysql_num_fields(result);
			//	for (i = 0; i < j; i++)
			//	{
			//		printf("%s\t", column[i]);
			//	}
			//	printf("\n");
			//	while (sql_row = mysql_fetch_row(result))//��ȡ���������  
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
	//	//ע���������ݿ�Ҫ��ʱ������Դ  
	//	if (result != NULL) mysql_free_result(result);//�ͷŽ����Դ  
	//	mysql_close(&myCont);//�ر�MySQL����  
	//	mysql_library_end();//�ر�MySQL��  
	//	return 0;
	//}



	//dcm�ļ��������洢����ͼƬ�Ĳ��ԣ�

	//#include "dcmtk/dcmdata/dctk.h"
	//#include "dcmtk/dcmimgle/dcmimage.h"
	//#include "dcmtk/dcmdata/dcfilefo.h"
	//#include <opencv2/opencv.hpp>
	//using namespace std;
	//int main()
	//{
	//	DcmFileFormat dfile;
	//	string path = "C:\\Users\\zhujianghui\\Downloads\\111.dcm";
	//	OFCondition result = dfile.loadFile(path.c_str()); // �����ļ�
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





	//dcmtk���Ĳ��ԣ�

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
	//opencv���Ĳ��ԣ�

	//#include "cv.h";
	//#include "highgui.h";
	//#include "cxcore.h";
	//int main(int argc, char *argv[])
	//{
	//	IplImage* src = cvLoadImage("C:\\Users\\zhujianghui\\Pictures\\Camera Roll\\622424199610153719�콭��.jpg");
	//	cvNamedWindow("win");
	//	cvShowImage("win", src);
	//	cvWaitKey(0);
	//	cvReleaseImage(&src);
	//	cvDestroyWindow("win");
	//	return 0;
	//}



	//dcm�ļ��������ԣ�
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

	//cinatra��ܵĲ��ԣ�
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


	//WT��ܵĲ��ԣ�
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
	//�³��Ե�һ�ַ�ʽ��
	DcmDataset *dataset = this->dcmFile.getDataset();
	dataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL);
	unsigned short m_width;														//��ȡͼ��Ĵ����
	unsigned short m_height;
	dataset->findAndGetUint16(DCM_Rows, m_height);
	dataset->findAndGetUint16(DCM_Columns, m_width);
	E_TransferSyntax xfer = dataset->getOriginalXfer();
	DicomImage* m_dcmImage = new DicomImage((DcmObject*)dataset, xfer); //����dataset����DicomImage����Ҫ����Ľ�ѹ������
	unsigned char *pixelData = (unsigned char*)(m_dcmImage->getOutputData(8 , 0, 0));	//���16λ��ͼ������ָ��
	if (pixelData != NULL)
	{
		cv::Mat imag(m_height, m_width, CV_8UC3, cv::Scalar::all(0));
		unsigned short* data = nullptr;
		for (int i = 0; i < m_height; i++)
		{
			//data = dst2.ptr<unsigned short>(i);	//ȡ��ÿһ�е�ͷָ�� Ҳ��ʹ��dst2.at<unsigned short>(i, j) = ?
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
		compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION); //PNG��ʽͼƬ��ѹ������
		compression_params.push_back(9);  //�������ñ����ͼ����������
		g_log->info("AnalysisDcm::SaveDcmToPng: ��ʼͼ�񱣴�");
		cv::imwrite(path, imag, compression_params);
		g_log->info("AnalysisDcm::SaveDcmToPng: ͼ�񱣴�ɹ���{}", path);
		g_log->flush();
		return true;
	}
	*/
	//���쳣�Ķ�ȡ��ʽ��
	/*
		DcmDataset *data = this->dcmFile.getDataset();
		//���ѹ���Ļ�����ѹ
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
		//�˴��б��쳣�����������ʽ��https://cloud.tencent.com/developer/article/1010015
		cv::Mat imag = cv::Mat(512, 512, CV_16U, pixData16);
		vector<int> compression_params;
		compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION); //PNG��ʽͼƬ��ѹ������
		compression_params.push_back(9);  //�������ñ����ͼ����������
		g_log->info("AnalysisDcm::SaveDcmToPng: ��ʼͼ�񱣴�");
		cv::imwrite(path, imag, compression_params);
		g_log->info("AnalysisDcm::SaveDcmToPng: ͼ�񱣴�ɹ���{}", path);
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
		g_log->info("main anasyDcmfile:����dcminfo:{}",insertDcm);
		cout << "����dcm��Ϣsql��䣺" << insertDcm << endl;
		if (myconn->DoInsert(insertDcm))
		{
			cout << "����dcm��Ϣ�ɹ�" << endl;
			g_log->info("main anasyDcmfile:����dcminfo�ɹ�");
			int id = myconn->GetTableId();
			if (id == -1)
			{
				g_log->error("main anasyDcmfile:��ѯdcminfo��idʧ��");
				cout << "��ѯid�������ʧ��" << endl;
			}
			di->SetId(id);
			cout << (*di) << endl;
		}
		else {
			g_log->error("main anasyDcmfile:����dcmInfoʧ��");
			cout << "����dcm��Ϣʧ��" << endl;
		}
		*/