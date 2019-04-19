#include "pch.h"
#include "analysisDcm.h"
#include "dcmtk/config/osconfig.h"  
#include "dcmtk/dcmdata/dctk.h"  
#include "dcmtk/dcmdata/dcrledrg.h"
#include "dcmtk/dcmimgle/dcmimage.h"  
#include "dcmtk/dcmimage/diregist.h"
#include "dcmtk/dcmjpeg/djdecode.h"
#include "global.h"
#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>

//#include "dcmtk/dcmdata/dctk.h"
//#include "dcmtk/dcmimgle/dcmimage.h"
//#include "dcmtk/dcmdata/dcfilefo.h"

using namespace std;
AnalysisDcm::AnalysisDcm(){} 

AnalysisDcm::AnalysisDcm(string path)
{
	this->dcmFilePath = path;
}

AnalysisDcm::~AnalysisDcm() 
{
	dcmFile = NULL;
	//delete dataSet;
}

bool AnalysisDcm::InitDcmFile()
{
	dataSet = nullptr;
	g_log->info("AnalysisDcm::InitDcmFile:解析文件:{}......", dcmFilePath.c_str());
	OFCondition result = dcmFile.loadFile(dcmFilePath.c_str()); // 加载文件
	if (result.bad())
	{
		g_log->info("AnalysisDcm::InitDcmFile:解析文件失败");
		return false;
	}
	dataSet = dcmFile.getDataset();
	return true;
}

ImageInfo* AnalysisDcm::GetImageInfo()
{
	OFString s;
	ImageInfo* i = new ImageInfo();
	if (dataSet->findAndGetOFString(DCM_SOPInstanceUID, s).good())
		i->SetSopInstanceId(s.data());
	if (dataSet->findAndGetOFString(DCM_ContentDate, s).good())
		i->SetImageDate(s.data());
	if (dataSet->findAndGetOFString(DCM_ContentTime, s).good())
		i->SetImageTime(s.data());
	if (dataSet->findAndGetOFString(DCM_HighBit, s).good())
		i->SetHightHit(s.data());
	if (dataSet->findAndGetOFString(DCM_WindowCenter, s).good())
		i->SetWindowCenter(s.data());
	if (dataSet->findAndGetOFString(DCM_WindowWidth, s).good())
		i->SetWindowWidth(s.data());
	return i;
}

SeriesInfo* AnalysisDcm::GetSeriesInfo()
{
	SeriesInfo* si = new SeriesInfo();
	OFString s;
	if (dataSet->findAndGetOFString(DCM_SeriesNumber, s).good())
		si->SetSeriesNum(s.data());
	if (dataSet->findAndGetOFString(DCM_SeriesDate, s).good())
		si->SetSeriesDate(s.data());
	if (dataSet->findAndGetOFString(DCM_SeriesTime, s).good())
		si->SetSeriesTime(s.data());
	if (dataSet->findAndGetOFString(DCM_SeriesInstanceUID, s).good())
		si->SetSeriesInstanceId(s.data());
	if (dataSet->findAndGetOFString(DCM_SeriesDate, s).good())
		si->SetSeriesDescription(s.data());
	if (dataSet->findAndGetOFString(DCM_SliceLocation, s).good())
		si->SetSliceLocation(s.data());
	if (dataSet->findAndGetOFString(DCM_SliceThickness, s).good())
		si->SetSliceThickness(s.data());
	if (dataSet->findAndGetOFString(DCM_SpacingBetweenSlices, s).good())
		si->SetSpacingBetweenSlices(s.data());
	if (dataSet->findAndGetOFString(DCM_Modality, s).good())
		si->SetStudyModality(s.data());
	return si;
}

StudyInfo* AnalysisDcm::GetStudyInfo()
{
	OFString s;
	StudyInfo* si = new StudyInfo();
	if (dataSet->findAndGetOFString(DCM_BodyPartExamined, s).good())
		si->SetStudyBodyPart(s.data());
	if (dataSet->findAndGetOFString(DCM_StudyDate, s).good())
		si->SetStudyDate(s.data());
	if (dataSet->findAndGetOFString(DCM_StudyDescription, s).good())
		si->SetStudyDescription(s.data());
	if (dataSet->findAndGetOFString(DCM_StudyID, s).good())
		si->SetStudyId(s.data());
	if (dataSet->findAndGetOFString(DCM_StudyInstanceUID, s).good())
		si->SetStudyInstanceId(s.data());
	if (dataSet->findAndGetOFString(DCM_AccessionNumber, s).good())
		si->SetStudyRisId(s.data());
	if (dataSet->findAndGetOFString(DCM_StudyTime, s).good())
		si->SetStudyTime(s.data());
	return si;
}

PatientInfo* AnalysisDcm::GetPatientInfo()
{
	PatientInfo* p = new PatientInfo();
	OFString s;
	if (dataSet->findAndGetOFString(DCM_PatientName, s).good())
		p->SetPatientName(s.data());
	if (dataSet->findAndGetOFString(DCM_PatientBirthDate, s).good())
		p->SetPatientBirthDate(s.data());
	if (dataSet->findAndGetOFString(DCM_PatientAge, s).good())
		p->SetPatientAge(s.data());
	if (dataSet->findAndGetOFString(DCM_PatientBirthTime, s).good())
		p->SetPatientBirthTime(s.data());
	if (dataSet->findAndGetOFString(DCM_PatientID, s).good())
		p->SetPatientId(s.data());
	if (dataSet->findAndGetOFString(DCM_PatientSex, s).good())
		p->SetPatientSex(s.data());
	if (dataSet->findAndGetOFString(DCM_PatientAddress, s).good())
		p->SetPatientAddress(s.data());
	if (dataSet->findAndGetOFString(DCM_PatientWeight, s).good())
		p->SetPatientWeight(s.data());
	return p;
}

bool AnalysisDcm::SaveDcmToPng(string path)
{
		DcmDataset *dataset = this->dcmFile.getDataset();						//得到Dicom的数据集
		E_TransferSyntax xfer = dataset->getOriginalXfer();							//得到传输语法

		unsigned short bit_count(0);
		dataset->findAndGetUint16(DCM_BitsStored, bit_count);						//获取像素的位数 bit

		OFString isRGB;
		dataset->findAndGetOFString(DCM_PhotometricInterpretation, isRGB);			//DCM图片的图像模式
		g_log->info("AnalysisDcm::SaveDcmToPng:图片颜色：{}", isRGB.c_str());
		unsigned short m_width;														//获取图像的窗宽高
		unsigned short m_height;
		dataset->findAndGetUint16(DCM_Rows, m_height);
		dataset->findAndGetUint16(DCM_Columns, m_width);

		const char*	transferSyntax = NULL;
		this->dcmFile.getMetaInfo()->findAndGetString(DCM_TransferSyntaxUID, transferSyntax);		//获得传输语法字符串
		string losslessTransUID = "1.2.840.10008.1.2.4.70";
		string lossTransUID = "1.2.840.10008.1.2.4.51";
		string losslessP14 = "1.2.840.10008.1.2.4.57";
		string lossyP1 = "1.2.840.10008.1.2.4.50";
		string lossyRLE = "1.2.840.10008.1.2.5";
		if (transferSyntax == losslessTransUID || transferSyntax == lossTransUID ||
			transferSyntax == losslessP14 || transferSyntax == lossyP1)
		{
			DJDecoderRegistration::registerCodecs();
			dataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL);						//对压缩的图像像素进行解压
			DJDecoderRegistration::cleanup();
		}
		else if (transferSyntax == lossyRLE)
		{
			DcmRLEDecoderRegistration::registerCodecs();
			dataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL);
			DcmRLEDecoderRegistration::cleanup();
		}
		else
		{
			dataset->chooseRepresentation(xfer, NULL);
		}
		g_log->info("AnalysisDcm::SaveDcmToPng:bit_count:{}", bit_count);
		g_log->info("AnalysisDcm::SaveDcmToPng:图片高:{},图片宽:{}",m_height,m_width);
		if (strcmp(isRGB.c_str(),"RGB")==0)		//彩色
		{
			cv::Mat dst(m_height, m_width, CV_8UC3, cv::Scalar::all(0));
			g_log->info("AnalysisDcm::SaveDcmToPng:存储彩色图片");
			unsigned short* data = nullptr;
			DicomImage* m_dcmImage = new DicomImage((DcmObject*)dataset, xfer); //利用dataset生成DicomImage，需要上面的解压方法；
			unsigned char *pixelData = (unsigned char*)(m_dcmImage->getOutputData(8, 0, 0));	//获得16位的图像数据指针
			for (int i = 0; i < m_height; i++)
			{
				for (int j = 0; j < m_width; j++)
				{	
					dst.at<cv::Vec3b>(i, j)[0] = *(pixelData + i * m_width*3  + j*3+2 );	//B	channel
					dst.at<cv::Vec3b>(i, j)[1] = *(pixelData + i * m_width*3  + j*3+1 );	//G channel
					dst.at<cv::Vec3b>(i, j)[2] = *(pixelData + i * m_width*3  + j*3 );		//R channel
							
				}
			}
			vector<int> compression_params;
			compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION); //PNG格式图片的压缩级别  
			compression_params.push_back(9);  //这里设置保存的图像质量级别
			g_log->info("AnalysisDcm::SaveDcmToPng: 开始图像保存");
			cv::imwrite(path, dst, compression_params);
			g_log->info("AnalysisDcm::SaveDcmToPng: 图像保存成功：{}", path);
		}
		else		//灰色
		{
			cv::Mat dst(m_height, m_width, CV_8UC1, cv::Scalar::all(0));
			g_log->info("AnalysisDcm::SaveDcmToPng:存储灰色图片");
			unsigned char* data = nullptr;
			Uint8* image_8data=nullptr;
			Uint16* image_16data= nullptr;
			DcmElement* element = NULL;
			OFCondition result = dataset->findAndGetElement(DCM_PixelData, element);	//获取图像数据
			if (result.bad() || element == NULL)
				return false;
			if (8==bit_count)
			{
				result = element->getUint8Array(image_8data);		//获取8位的图像数据
			}
			else {
				result = element->getUint16Array(image_16data);		//获取8位的图像数据
			}
			for (int i = 0; i < m_height; i++)
			{
				data = dst.ptr<unsigned char>(i);	//取得每一行的头指针 也可使用dst2.at<unsigned short>(i, j) = ?
				for (int j = 0; j < m_width; j++)
				{
					if (bit_count == 8)
					{
						*data++ = (unsigned char)((float)image_8data[i*m_width + j]);//* 255.0 / std::pow(2.0, bit_count) + 0.5)
					}
					else {
						*data++ = (unsigned char)((float)image_16data[i*m_width + j]) ;//* 255.0 / std::pow(2.0, bit_count) + 0.5)
					}
				}
			}
			vector<int> compression_params;
			compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION); //PNG格式图片的压缩级别  
			compression_params.push_back(9);  //这里设置保存的图像质量级别
			g_log->info("AnalysisDcm::SaveDcmToPng: 开始图像保存");
			cv::imwrite(path, dst, compression_params);
			g_log->info("AnalysisDcm::SaveDcmToPng: 图像保存成功：{}", path);	
		}
		return true;
}