#pragma once
#ifndef SeriesInfo_H
#define SeriesInfo_H
#include<string>
using namespace std;
class SeriesInfo
{
private:
	unsigned int id;
	string seriesNum;
	string seriesInstanceId;
	string studyModality;
	string seriesDescription;
	string seriesDate;
	string seriesTime;
	string sliceThickness;//���
	string spacingBetweenSlices;//�����֮�����Լ��
	string sliceLocation;//ʵ�ʵ���Ծ���
	unsigned int studyId;
	//string mrAcquisition;//�չ���
public:
	SeriesInfo();
	SeriesInfo(unsigned int,string, string , string ,string , string , string , string , string , string );
	~SeriesInfo();
	SeriesInfo& operator=(const SeriesInfo&);
	void SetId(unsigned int id);
	unsigned int GetId();
	void SetSeriesNum(string sn);
	string GetSeriesNum();
	void SetSeriesInstanceId(string siid);
	string GetSeriesInstanceId();
	void SetStudyModality(string sm);
	string GetStudyModality();
	void SetSeriesDescription(string sd);
	string GetSeriesDescription();
	void SetSeriesDate(string sd);
	string GetSeriesDate();
	void SetSeriesTime(string st);
	string GetSeriesTime();
	void SetSliceThickness(string st);
	string GetSliceThickness();
	void SetSpacingBetweenSlices(string sbs);
	string GetSpacingBetweenSlices();
	void SetSliceLocation(string sl);
	string GetSliceLocation();
	void SetStudyId(unsigned int id);
	unsigned int GetStudyId();
	friend ostream & operator<<(ostream & os, const SeriesInfo& s);
};
#endif // !SeriesInfo_H
