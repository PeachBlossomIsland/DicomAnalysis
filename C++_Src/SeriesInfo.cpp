#include "pch.h"
#include "SeriesInfo.h"

SeriesInfo::SeriesInfo(){}

SeriesInfo::SeriesInfo(unsigned int id,string sd, string sdp, string siid, string sn, string st , string sl, string stn, string sbs, string sm)
{
	this->id = id;
	this->seriesDate = sd;
	this->seriesDescription = sdp;
	this->seriesInstanceId = siid;
	this->seriesNum = sn;
	this->seriesTime = st;
	this->sliceLocation = sl;
	this->sliceThickness = stn;
	this->spacingBetweenSlices = sbs;
	this->studyModality = sm;
}

SeriesInfo::~SeriesInfo(){}

SeriesInfo& SeriesInfo::operator=(const SeriesInfo &s)
{
	if (this != &s)
	{
		this->id = s.id;
		this->seriesDate = s.seriesDate;
		this->seriesDescription = s.seriesDescription;
		this->seriesInstanceId = s.seriesInstanceId;
		this->seriesNum = s.seriesNum;
		this->seriesTime = s.seriesTime;
		this->sliceLocation = s.sliceLocation;
		this->sliceThickness = s.sliceThickness;
		this->spacingBetweenSlices = s.spacingBetweenSlices;
		this->studyModality = s.studyModality;
	}
	return *this;
}

void SeriesInfo::SetId(unsigned int id)
{
	this->id = id;
}

unsigned int SeriesInfo::GetId()
{
	return this->id;
}

void SeriesInfo::SetSeriesNum(string sn)
{
	this->seriesNum = sn;
}

string SeriesInfo::GetSeriesNum()
{
	return this->seriesNum;
}

void SeriesInfo::SetSeriesInstanceId(string siid)
{
	this->seriesInstanceId = siid;
}

string SeriesInfo::GetSeriesInstanceId()
{
	return this->seriesInstanceId;
}

void SeriesInfo::SetStudyModality(string sm)
{
	this->studyModality = sm;
}

string SeriesInfo::GetStudyModality()
{
	return this->studyModality;
}

void SeriesInfo::SetSeriesDescription(string sd)
{
	this->seriesDescription = sd;
}

string SeriesInfo::GetSeriesDescription()
{
	return this->seriesDescription;
}

void SeriesInfo::SetSeriesDate(string sd)
{
	this->seriesDate = sd;
}

string SeriesInfo::GetSeriesDate()
{
	return this->seriesDate;
}

void SeriesInfo::SetSeriesTime(string st)
{
	this->seriesTime = st;
}

string SeriesInfo::GetSeriesTime()
{
	return this->seriesTime;
}

void SeriesInfo::SetSliceThickness(string st)
{
	this->sliceThickness = st;
}

string SeriesInfo::GetSliceThickness()
{
	return this->sliceThickness;
}

void SeriesInfo::SetSpacingBetweenSlices(string sbs)
{
	this->spacingBetweenSlices = sbs;
}

string SeriesInfo::GetSpacingBetweenSlices()
{
	return this->spacingBetweenSlices;
}

void SeriesInfo::SetSliceLocation(string sl)
{
	this->sliceLocation = sl;
}

string SeriesInfo::GetSliceLocation()
{
	return this->sliceLocation;
}

void SeriesInfo::SetStudyId(unsigned int id)
{
	this->studyId = id;
}

unsigned int SeriesInfo::GetStudyId()
{
	return this->studyId;
}

ostream & operator<<(ostream & os, const SeriesInfo & s)
{
	os <<"ID: "<<s.id<<"\n"
		<< "seriesDate:" << s.seriesDate << "\n"
		<< "seriesDescription:" << s.seriesDescription << "\n"
		<< "seriesInstanceId:" << s.seriesInstanceId << "\n"
		<< "seriesNum:" << s.seriesNum << "\n"
		<< "seriesTime:" << s.seriesTime << "\n"
		<< "sliceLocation:" << s.sliceLocation << "\n"
		<< "sliceThickness:" << s.sliceThickness << "\n"
		<<"studyModality:"<<s.studyModality<<"\n"
		<< "spacingBetweenSlices:" << s.spacingBetweenSlices << "\n";
	return os;
}
