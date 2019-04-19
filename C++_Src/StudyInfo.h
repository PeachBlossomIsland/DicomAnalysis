#pragma once
#ifndef StudyInfo_H
#define StudyInfo_H
#include<string>
using namespace std;
class StudyInfo
{
public:
	StudyInfo();
	StudyInfo(unsigned int,string, string, string, string, string, string, string);
	~StudyInfo();
	void SetId(unsigned int id);
	unsigned int GetId();
	void SetStudyRisId(string srid);
	string GetStudyRisId();
	void SetStudyId(string sid);
	string GetStudyId();
	void SetStudyInstanceId(string siid);
	string GetStudyInstanceId();
	void SetStudyDate(string sd);
	string GetStudyDate();
	void SetStudyTime(string st);
	string GetStudyTime();
	void SetStudyBodyPart(string sbp);
	string GetStudyBodyPart();
	void SetStudyDescription(string sdp);
	string GetStudyDescription();
	StudyInfo& operator=(const StudyInfo&);
	void SetPatientId(unsigned int id);
	unsigned int GetPatientId();
	friend ostream & operator<<(ostream & os, const StudyInfo& s);
private:
	unsigned int id;
	string studyRisId;//检查号
	string studyId;//检查ID
	string studyInstanceId;//检查实例号
	string studyDate;
	string studyTime;
	string studyBodyPart;
	string studyDescription;
	unsigned int patientId;
};
#endif // !StudyInfo_H
