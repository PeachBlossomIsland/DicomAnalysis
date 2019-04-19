#include "pch.h"
#include "imageInfo.h"

ImageInfo::ImageInfo(){}

ImageInfo::~ImageInfo(){}

ImageInfo::ImageInfo(unsigned int id, string sopid, string date, string time, string hit, string wc, string ww)
{
	this->id = id;
	this->sopInstanceId = sopid;
	this->imageDate = date;
	this->imageTime = time;
	this->hightHit = hit;
	this->windowCenter = wc;
	this->windowWidth = ww;
}

void ImageInfo::SetId(unsigned int id)
{
	this->id = id;
}

unsigned int ImageInfo::GetId()
{
	return this->id;
}


ImageInfo & ImageInfo::operator=(const ImageInfo &s)
{
	if (this != &s)
	{
		this->id = s.id;
		this->sopInstanceId = s.sopInstanceId;
		this->imageTime = s.imageTime;
		this->imageDate = s.imageDate;
		this->hightHit = s.hightHit;
		this->windowCenter = s.windowCenter;
		this->windowWidth = s.windowWidth;
	}
	return *this;
}

void ImageInfo::SetSopInstanceId(string id)
{
	this->sopInstanceId = id;
}

string ImageInfo::GetSopInstanceId()
{
	return this->sopInstanceId;
}

void ImageInfo::SetImageDate(string id)
{
	this->imageDate = id;
}

string ImageInfo::GetimageDate()
{
	return this->imageDate;
}

void ImageInfo::SetImageTime(string it)
{
	this->imageTime = it;
}

string ImageInfo::GetImageTime()
{
	return this->imageTime;
}

void ImageInfo::SetHightHit(string hh)
{
	this->hightHit = hh;
}

string ImageInfo::GetHightHit()
{
	return this->hightHit;
}

void ImageInfo::SetWindowCenter(string wc)
{
	this->windowCenter = wc;
}

string ImageInfo::GetWindowCenter()
{
	return this->windowCenter;
}

void ImageInfo::SetWindowWidth(string ww)
{
	this->windowWidth = ww;
}

string ImageInfo::GetWindowWidth()
{
	return this->windowWidth;
}

void ImageInfo::SetImagePath(string path)
{
	this->imagepath = path;
}

string ImageInfo::GetImagePath()
{
	return this->imagepath;
}

void ImageInfo::SetSeriesId(unsigned int id)
{
	this->seriesId = id;
}

unsigned int ImageInfo::GetSeriesId()
{
	return this->seriesId;
}

ostream & operator<<(ostream & os, const ImageInfo & i)
{
	os << "ID: " << i.id << "\n"
		<<"sopInstanceId:" << i.sopInstanceId << "\n"
		<< "imageDate:" << i.imageDate << "\n"
		<< "imageTime:" << i.imageTime << "\n"
		<< "hightHit:" << i.hightHit << "\n"
		<< "windowCenter:" << i.windowCenter << "\n"
		<< "windowWidth:" << i.windowWidth << "\n";
	return os;
}
