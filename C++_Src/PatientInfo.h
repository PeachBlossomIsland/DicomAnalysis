#pragma once
#ifndef PatientInfo_H
#define PatientInfo_H
#include <string>
using namespace std;
class PatientInfo
{
public:
	PatientInfo();
	PatientInfo(unsigned int id,string pid,string pn, string ps, string pa, string pbd, string pbt, string pw,string pad);
	~PatientInfo();
	PatientInfo& operator=(const PatientInfo &);
	void SetId(unsigned int id);
	unsigned int GetId();
	void SetPatientName(string pn);
	string GetPatientName();
	void SetPatientSex(string ps);
	string GetPatientSex();
	void SetPatientAge(string pa);
	string GetPatientAge();
	void SetPatientBirthTime(string pbt);
	string GetPatientBirthTime();
	void SetPatientBirthDate(string pbd);
	string GetPatientBirthDate();
	void SetPatientWeight(string pw);
	string GetPatientWeight();
	void SetPatientId(string id);
	string GetPatientId();
	void SetPatientAddress(string pa);
	string GetPatientAddress();
	friend ostream & operator<<(ostream & os, const PatientInfo& p);
private:
	unsigned int id;
	string patientName;
	string patientSex;
	string patientAge;//患者做检查时的年龄
	string  patientBirthDate;
	string patientBirthTime;
	string patientWeight;
	string patientId;
	string patientAddress;
};
#endif // !PatientInfo_H