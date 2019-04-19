#pragma once
#ifndef MysqlResToT_H
#define MysqlResToT_H
#include "mysql.h"
#include "imageInfo.h"
#include "PatientInfo.h"
#include "SeriesInfo.h"
#include "StudyInfo.h"
#include "dcminfo.h"
#include <vector>
constexpr auto CHAR_LEN = 1024;
class MysqlResToT
{
public:
	MysqlResToT();
	~MysqlResToT();
	MysqlResToT(MYSQL_RES* r);
	void SetMysqlRes(MYSQL_RES* r);
	MYSQL_RES* GetMysqlRes();
	void ToImageInfo(vector<ImageInfo>&);
	void ToSeriesInfo(vector<SeriesInfo>&);
	void ToStudyInfo(vector<StudyInfo>&);
	void ToPatientInfo(vector<PatientInfo>&);
	void ToDcmInfo(vector<DcmInfo>&);
	unsigned int ToTableId();
	//主要用于插入时的sql语句
	string ImageInfoToSql(ImageInfo& s);
	string SeriesInfoToSql(SeriesInfo& s);
	string StudyInfoToSql(StudyInfo& s);
	string PatientInfoToSql(PatientInfo& s);
	string DcmInfoToSql(DcmInfo& s);
private:
	MYSQL_RES* result;
};

#endif // !MysqlResToT_H

