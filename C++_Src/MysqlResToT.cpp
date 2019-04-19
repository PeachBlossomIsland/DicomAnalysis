#include "pch.h"
#include "MysqlResToT.h"
#include<iostream>
MysqlResToT::MysqlResToT()
{
	this->result = NULL;
}
 
MysqlResToT::~MysqlResToT()
{
	mysql_free_result(result);
}

MysqlResToT::MysqlResToT(MYSQL_RES * r)
{
	this->result = r;
}

void MysqlResToT::SetMysqlRes(MYSQL_RES * r)
{
	this->result = r;
}

MYSQL_RES * MysqlResToT::GetMysqlRes()
{
	return this->result;
}

void MysqlResToT::ToImageInfo(vector<ImageInfo>& re)
{
	if (result)
	{
		MYSQL_ROW sqlRow;
		while (sqlRow = mysql_fetch_row(result)) {
				ImageInfo* s = new ImageInfo();
				s->SetId(atoi(sqlRow[0]));
				s->SetSopInstanceId(sqlRow[1]);
				s->SetImageDate(sqlRow[2]);
				s->SetImageTime(sqlRow[3]);
				s->SetHightHit(sqlRow[4]);
				s->SetWindowCenter(sqlRow[5]);
				s->SetWindowWidth(sqlRow[6]);
				re.push_back((*s));
			}
	}
}
void MysqlResToT::ToSeriesInfo(vector<SeriesInfo>& re)
{
	if (result)
	{
		MYSQL_ROW sqlRow;
		while (sqlRow = mysql_fetch_row(result)) {
				SeriesInfo* s = new SeriesInfo();
				s->SetId(atoi(sqlRow[0]));
				s->SetSeriesNum(sqlRow[1]);
				s->SetSeriesInstanceId(sqlRow[2]);
				s->SetStudyModality(sqlRow[3]);
				s->SetSeriesDescription(sqlRow[4]);
				s->SetSeriesDate(sqlRow[5]);
				s->SetSeriesTime(sqlRow[6]);
				s->SetSliceThickness(sqlRow[7]);
				s->SetSpacingBetweenSlices(sqlRow[8]);
				s->SetSliceLocation(sqlRow[9]);
				re.push_back((*s));
		}
	}
}
void MysqlResToT::ToStudyInfo(vector<StudyInfo>& re)
{
	if (result)
	{
		MYSQL_ROW sqlRow;
		while (sqlRow = mysql_fetch_row(result)) {
				StudyInfo* s = new StudyInfo();
				s->SetId(atoi(sqlRow[0]));
				s->SetStudyId(sqlRow[1]);
				s->SetStudyRisId(sqlRow[2]);
				s->SetStudyInstanceId(sqlRow[3]);
				s->SetStudyDate(sqlRow[4]);
				s->SetStudyTime(sqlRow[5]);
				s->SetStudyBodyPart(sqlRow[6]);
				s->SetStudyDescription(sqlRow[7]);
				re.push_back((*s));
		}
	}
}
void MysqlResToT::ToPatientInfo(vector<PatientInfo>& re)
{
	if (result)
	{
		MYSQL_ROW sqlRow;
		while (sqlRow = mysql_fetch_row(result)) {
				PatientInfo* s = new PatientInfo();
				s->SetId(atoi(sqlRow[0]));
				s->SetPatientName(sqlRow[1]);
				s->SetPatientAge(sqlRow[2]);
				s->SetPatientAddress(sqlRow[3]);
				s->SetPatientBirthDate(sqlRow[4]);
				s->SetPatientBirthTime(sqlRow[5]);
				s->SetPatientWeight(sqlRow[6]);
				s->SetPatientId(sqlRow[7]);
				s->SetPatientSex(sqlRow[8]);
				std::cout << "sqlrow:"<<sqlRow[0] << endl;
				re.push_back((*s));
		}
	}
}
void MysqlResToT::ToDcmInfo(vector<DcmInfo>& re)
{
	if (result)
	{
		MYSQL_ROW sqlRow;
		while (sqlRow = mysql_fetch_row(result)) {
				DcmInfo* s = new DcmInfo();
				s->SetId(atoi(sqlRow[0]));
				s->SetPatientId(atoi(sqlRow[1]));
				s->SetStudyId(atoi(sqlRow[2]));
				s->SetSeriesId(atoi(sqlRow[3]));
				s->SetImageId(atoi(sqlRow[4]));
				s->SetImagePath(sqlRow[5]);
				re.push_back((*s));
		}
	}
}

unsigned int MysqlResToT::ToTableId()
{
	unsigned int re;
	if (result)
	{
		MYSQL_ROW sqlRow;
		while (sqlRow = mysql_fetch_row(result)) {
			re = atoi(sqlRow[0]);
		}
	}
	return re;
}


string MysqlResToT::ImageInfoToSql(ImageInfo & s)
{
	//主要用于插入语句
	string id = s.GetSopInstanceId();
	string hh= s.GetHightHit();
	string ide= s.GetimageDate();
	string it= s.GetImageTime();
	string wc= s.GetWindowCenter();
	string ww = s.GetWindowWidth();
	string path = s.GetImagePath();
	unsigned int sid = s.GetSeriesId();
	char re[CHAR_LEN] = {};
	sprintf_s(re,CHAR_LEN,"insert into imageinfo values('%d','%s','%s','%s','%s','%s','%s','%s','%d')",0,id.c_str(),ide.c_str(),it.c_str(),hh.c_str(),wc.c_str(),ww.c_str(),path.c_str(),sid);
	return re;
}

string MysqlResToT::SeriesInfoToSql(SeriesInfo & s)
{
	//主要用于插入语句
	string siid= s.GetSeriesInstanceId();
	string sde= s.GetSeriesDate();
	string sdc= s.GetSeriesDescription();
	string sn= s.GetSeriesNum();
	string st= s.GetSeriesTime();
	string sl= s.GetSliceLocation();
	string slt = s.GetSliceThickness();
	string sbs= s.GetSpacingBetweenSlices();
	string sm= s.GetStudyModality();
	unsigned int sid = s.GetStudyId();
	char re[CHAR_LEN] = {};
	sprintf_s(re, CHAR_LEN, "insert into seriesinfo values('%d','%s','%s','%s','%s','%s','%s','%s','%s','%s','%d')",0,sn.c_str(), siid.c_str(), sm.c_str(), sdc.c_str(), sde.c_str(), st.c_str(), slt.c_str(),sbs.c_str(),sl.c_str(),sid);
	return re;
}

string MysqlResToT::StudyInfoToSql(StudyInfo & s)
{
	string sid= s.GetStudyId();
	string siid= s.GetStudyInstanceId();
	string sbp= s.GetStudyBodyPart();
	string sd= s.GetStudyDate();
	string sdc= s.GetStudyDescription();
	string srisd= s.GetStudyRisId();
	string st= s.GetStudyTime();
	unsigned int pid = s.GetPatientId();
	char re[CHAR_LEN] = {};
	sprintf_s(re, CHAR_LEN, "insert into studyinfo values('%d','%s','%s','%s','%s','%s','%s','%s','%d')",0,sid.c_str(), srisd.c_str(), siid.c_str(), sd.c_str(), st.c_str(), sbp.c_str(), sdc.c_str(),pid);
	return re;
}

string MysqlResToT::PatientInfoToSql(PatientInfo & s)
{
	string pid= s.GetPatientId();
	string pad= s.GetPatientAddress();
	string pag= s.GetPatientAge();
	string pbd= s.GetPatientBirthDate();
	string pbt= s.GetPatientBirthTime();
	string pn= s.GetPatientName();
	string ps= s.GetPatientSex();
	string pw = s.GetPatientWeight();
	char re[CHAR_LEN] = {};
	sprintf_s(re, CHAR_LEN, "insert into patientinfo values('%d','%s','%s','%s','%s','%s','%s','%s','%s')",0,pn.c_str(), pag.c_str(), pad.c_str(), pbd.c_str(), pbt.c_str(), pw.c_str(), pid.c_str(), ps.c_str());
	return re;
}

string MysqlResToT::DcmInfoToSql(DcmInfo & s)
{
	unsigned int id = s.GetId();
	unsigned int pid = s.GetPatientId();
	unsigned int sid = s.GetStudyId();
	unsigned int siid = s.GetSeriesId();
	unsigned int iid = s.GetImageId();
	string path = s.GetImagePath();
	char re[CHAR_LEN] = {};
	sprintf_s(re, CHAR_LEN, "insert into dcminfo values('%d','%d','%d','%d','%d','%s')",0,pid, sid, siid, iid,path.c_str());
	return re;
}
