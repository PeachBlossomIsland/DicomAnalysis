#pragma once
#ifndef DCMINFO_H
#define DCMINFO_H
#include <string>
using namespace std;
class DcmInfo
{
public:
	DcmInfo();
	~DcmInfo();
	DcmInfo(unsigned int id,unsigned int pid,unsigned int sid,unsigned int siid,unsigned int iid,string imagePath);
	void SetId(unsigned int id);
	unsigned int GetId();
	void SetPatientId(unsigned int id);
	unsigned int GetPatientId();
	void SetStudyId(unsigned int id);
	unsigned int GetStudyId();
	void SetSeriesId(unsigned int id);
	unsigned int GetSeriesId();
	void SetImageId(unsigned int id);
	unsigned int GetImageId();
	void SetImagePath(string path);
	string GetImagePath();
	friend ostream & operator<<(ostream & os, const DcmInfo& d);
private:
	unsigned int id;
	unsigned int patienId;
	unsigned int studyId;
	unsigned int seriesId;
	unsigned int imageId;
	string imagePath;
};
#endif // !DCMINFO_H

