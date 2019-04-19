#include "pch.h"
#include "dcmInfo.h"

DcmInfo::DcmInfo(){}

DcmInfo::~DcmInfo(){}

DcmInfo::DcmInfo(unsigned int id, unsigned int pid, unsigned int sid, unsigned int siid, unsigned int iid, string imagePath)
{
	this->id = id;
	this->patienId = pid;
	this->studyId = sid;
	this->seriesId = siid;
	this->imageId = iid;
	this->imagePath = imagePath;
}
void DcmInfo::SetId(unsigned int id)
{
	this->id = id;
}
unsigned int DcmInfo::GetId()
{
	return this->id;
}
void DcmInfo::SetPatientId(unsigned int id)
{
	this->patienId = id;
}
unsigned int DcmInfo::GetPatientId()
{
	return this->patienId;
}

void DcmInfo::SetStudyId(unsigned int id)
{
	this->studyId = id;
}

unsigned int DcmInfo::GetStudyId()
{
	return this->studyId;
}

void DcmInfo::SetSeriesId(unsigned int id)
{
	this->seriesId = id;
}

unsigned int DcmInfo::GetSeriesId()
{
	return this->seriesId;
}

void DcmInfo::SetImageId(unsigned int id)
{
	this->imageId = id;
}

unsigned int DcmInfo::GetImageId()
{
	return this->imageId;
}

void DcmInfo::SetImagePath(string path)
{
	this->imagePath = path;
}

string DcmInfo::GetImagePath()
{
	return this->imagePath;
}

ostream & operator<<(ostream & os, const DcmInfo & d)
{
	os << "ID£º "<<d.id<<"\n"
		<<"patientId:" << d.patienId << "\n"
		<< "studyId:" << d.studyId << "\n"
		<< "seriesId:" << d.seriesId << "\n"
		<< "imageId:" << d.imageId << "\n"
		<< "imagePath:" << d.imagePath << "\n";
	return os;
}
