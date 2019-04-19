#pragma once
#ifndef ImageInfo_H
#define ImageInfo_H
#include<string>
using namespace std;
class ImageInfo
{
public:
	ImageInfo();
	~ImageInfo();
	ImageInfo(unsigned int id,string sopid, string date, string time, string hit, string wc, string ww);
	void SetId(unsigned int id);
	unsigned int GetId();
	ImageInfo& operator=(const ImageInfo&);
	void SetSopInstanceId(string id);
	string GetSopInstanceId();
	void SetImageDate(string id);
	string GetimageDate();
	void SetImageTime(string it);
	string GetImageTime();
	void SetHightHit(string hh);
	string GetHightHit();
	void SetWindowCenter(string wc);
	string GetWindowCenter();
	void SetWindowWidth(string ww);
	string GetWindowWidth();
	void SetImagePath(string path);
	string GetImagePath();
	void SetSeriesId(unsigned int id);
	unsigned int GetSeriesId();
	friend ostream & operator<<(ostream & os, const ImageInfo& i);
private:
	unsigned int id;
	string sopInstanceId;
	string imageDate;
	string imageTime;
	string hightHit;//¸ßÎ»
	string windowCenter;//´°Î»
	string windowWidth;//´°¿í
	string imagepath;
	unsigned int seriesId;
};
#endif // !ImageInfo_H
