#pragma once
#ifndef AnalysisDcm_H
#define AnalysisDcm_H
#include<string>
#include "StudyInfo.h"
#include "SeriesInfo.h"
#include "ImageInfo.h"
#include "PatientInfo.h"
#include "dcmtk/dcmdata/dctk.h"
using namespace std;
class AnalysisDcm
{
public:
	AnalysisDcm();
	AnalysisDcm(string path);
	~AnalysisDcm();
	bool InitDcmFile();
	ImageInfo* GetImageInfo();
	SeriesInfo* GetSeriesInfo();
	StudyInfo* GetStudyInfo();
	PatientInfo* GetPatientInfo();
	bool SaveDcmToPng(string path);
private:
	string dcmFilePath;
	DcmFileFormat dcmFile;
	DcmDataset* dataSet;
};
#endif // !AnalysisDcm_H
