#include "pch.h"
#include "PatientInfo.h"

PatientInfo::PatientInfo(){}

PatientInfo::PatientInfo(unsigned int id, string pid, string pn, string ps, string pa, string pbd, string pbt, string pw, string pad)
{
	this->id = id;
	this->patientName = pn;
	this->patientSex = ps;
	this->patientBirthDate = pbd;
	this->patientBirthTime = pbt;
	this->patientWeight = pw;
	this->patientAddress = pad;
	this->patientAge = pa;
	this->patientId = pid;
}

PatientInfo::~PatientInfo(){}

PatientInfo & PatientInfo::operator=(const PatientInfo &p)
{
	if (this != &p)
	{
		this->id = p.id;
		this->patientId = p.patientId;
		this->patientBirthDate = p.patientBirthDate;
		this->patientBirthTime = p.patientBirthTime;
		this->patientAddress = p.patientAddress;
		this->patientAge = p.patientAge;
		this->patientName = p.patientName;
		this->patientSex = p.patientSex;
		this->patientWeight = p.patientWeight;
	}
	return *this;
}

void PatientInfo::SetId(unsigned int id)
{
	this->id = id;
}

unsigned int PatientInfo::GetId()
{
	return this->id;
}

void PatientInfo::SetPatientName(string pn)
{
	this->patientName = pn;
}

string PatientInfo::GetPatientName()
{
	return this->patientName;
}

void PatientInfo::SetPatientSex(string ps)
{
	this->patientSex = ps;
}

string PatientInfo::GetPatientSex()
{
	return this->patientSex;
}

void PatientInfo::SetPatientAge(string pa)
{
	this->patientAge = pa;
}

string PatientInfo::GetPatientAge()
{
	return this->patientAge;
}


void PatientInfo::SetPatientBirthTime(string pbt)
{
	this->patientBirthTime = pbt;
}

string PatientInfo::GetPatientBirthTime()
{
	return this->patientBirthTime;
}

void PatientInfo::SetPatientBirthDate(string pbd)
{
	this->patientBirthDate = pbd;
}

string PatientInfo::GetPatientBirthDate()
{
	return this->patientBirthDate;
}

void PatientInfo::SetPatientWeight(string pw)
{
	this->patientWeight = pw;
}

string PatientInfo::GetPatientWeight()
{
	return this->patientWeight;
}

void PatientInfo::SetPatientId(string id)
{
	this->patientId = id;
}

string PatientInfo::GetPatientId()
{
	return this->patientId;
}

void PatientInfo::SetPatientAddress(string pa)
{
	this->patientAddress = pa;
}

string PatientInfo::GetPatientAddress()
{
	return this->patientAddress;
}

ostream & operator<<(ostream & os, const PatientInfo & p)
{
	os  <<"ID: "<<p.id<<"\n"
		<< "patientName:" << p.patientName << "\n"
		<< "patientAge:" << p.patientAge << "\n"
		<< "patientAddress:" << p.patientAddress << "\n"
		<< "patientBirthDate:" << p.patientBirthDate << "\n"
		<< "patientBirthTime:" << p.patientBirthTime << "\n"
		<< "patientId:" << p.patientId << "\n"
		<< "patientSex:" << p.patientSex << "\n"
		<< "patientWeight:" << p.patientWeight << "\n";
	return os;
}
