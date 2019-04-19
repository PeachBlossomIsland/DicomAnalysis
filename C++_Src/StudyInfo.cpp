#include "pch.h"
#include "StudyInfo.h"

StudyInfo::StudyInfo(){}

StudyInfo::StudyInfo(unsigned int id,string studyRisId, string studyId, string studyInstanceId, string studyDate, string studyTime, string studyBodyPart, string studyDescription)
{
	this->id = id;
	this->studyRisId = studyRisId;
	this->studyId = studyId;
	this->studyInstanceId = studyInstanceId;
	this->studyDate = studyDate;
	this->studyTime = studyTime;
	this->studyBodyPart = studyBodyPart;
	this->studyDescription = studyDescription;
}

StudyInfo::~StudyInfo(){}

void StudyInfo::SetId(unsigned int id)
{
	this->id = id;
}

unsigned int StudyInfo::GetId()
{
	return this->id;
}

void StudyInfo::SetStudyRisId(string srid)
{
	this->studyRisId = srid;
}

string StudyInfo::GetStudyRisId()
{
	return this->studyRisId;
}

void StudyInfo::SetStudyId(string sid)
{
	this->studyId = sid;
}

string StudyInfo::GetStudyId()
{
	return this->studyId;
}

void StudyInfo::SetStudyInstanceId(string siid)
{
	this->studyInstanceId = siid;
}

string StudyInfo::GetStudyInstanceId()
{
	return this->studyInstanceId;
}

void StudyInfo::SetStudyDate(string sd)
{
	this->studyDate = sd;
}

string StudyInfo::GetStudyDate()
{
	return this->studyDate;
}

void StudyInfo::SetStudyTime(string st)
{
	this->studyTime = st;
}

string StudyInfo::GetStudyTime()
{
	return this->studyTime;
}

void StudyInfo::SetStudyBodyPart(string sbp)
{
	this->studyBodyPart = sbp;
}

string StudyInfo::GetStudyBodyPart()
{
	return this->studyBodyPart;
}

void StudyInfo::SetStudyDescription(string sdp)
{
	this->studyDescription = sdp;
}

string StudyInfo::GetStudyDescription()
{
	return this->studyDescription;
}


StudyInfo & StudyInfo::operator=(const StudyInfo &s)
{
	if (this != &s)
	{
		this->id = s.id;
		this->studyId = s.studyId;
		this->studyBodyPart = s.studyBodyPart;
		this->studyDate = s.studyDate;
		this->studyDescription = s.studyDescription;
		this->studyInstanceId = s.studyInstanceId;
		this->studyRisId = s.studyRisId;
		this->studyTime = s.studyTime;
	}
	return *this;
}

void StudyInfo::SetPatientId(unsigned int id)
{
	this->patientId = id;
}

unsigned int StudyInfo::GetPatientId()
{
	return this->patientId;
}

ostream & operator<<(ostream & os, const StudyInfo & s)
{
	os <<"ID: "<<s.id<<"\n"
		<< "studyDate:" << s.studyDate << "\n"
		<< "studyBodyPart:" << s.studyBodyPart << "\n"
		<< "studyDescription:" << s.studyDescription << "\n"
		<< "studyId:" << s.studyId << "\n"
		<< "studyInstanceId:" << s.studyInstanceId << "\n"
		<< "studyRisId:" << s.studyRisId << "\n"
		<< "studyTime:" << s.studyTime << "\n";
	return os;
}
