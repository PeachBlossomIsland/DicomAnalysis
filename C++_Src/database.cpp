#include "pch.h"
#include "database.h"
#include "winsock.h"
#include "windows.h"
#include "mysql.h"
#include "global.h"
#include<iostream>
//注意销毁指针内存
DataBase::~DataBase()
{
	//cout << "DataBase析构函数\n";
	//关闭MySQL连接 
	mysql_close(&myConn); 
	mysql_library_end();
}

DataBase::DataBase(string host, string user, string pswd, string db, unsigned int port)
{
	this->host = host;
	this->user = user;
	this->password = pswd;
	this->database = db;
	this->port = port;
}

int DataBase::DoSelect(string sql)
{
	if (mysql_query(&myConn, sql.c_str()))
		return false;
	result = mysql_store_result(&myConn);
	return true;
}

int DataBase::DoDelete(string sql)
{
	if (mysql_query(&myConn, sql.c_str()))
		return false;
	return mysql_affected_rows(&myConn);
}

int DataBase::DoInsert(string sql)
{
	if (mysql_query(&myConn, sql.c_str()))
		return false;
	return mysql_affected_rows(&myConn);
}

int DataBase::DoUpdate(string sql)
{
	if (mysql_query(&myConn, sql.c_str()))
		return false;
	return mysql_affected_rows(&myConn);
}

bool DataBase::InitConnection()
{
	try
	{
		if (mysql_init(&myConn) == NULL)
		{
			cout << "mysql_init"<<endl;
			return false;
		}
		if (!mysql_real_connect(&myConn, host.c_str(), user.c_str(), password.c_str(), database.c_str(), port, NULL, 0))
		{
			g_log->info("DataBase::InitConnection:初始化数据库连接....");
			//cout << "mysql_real_connect" << endl;
			return false;
		}
		return true;
	}
	catch (const std::exception& e)
	{
		g_log->info("DataBase::InitConnection:初始化数据库连接失败：{}",(std::string)e.what());
		//std::cout << "初始化数据库出错: " << e.what() << endl;
		return false;
	}
}

MYSQL_RES * DataBase::GetMysqlRes()
{
	return result;
}
//得到刚插入的自增长的ID
int DataBase::GetTableId()
{
	string sql = "SELECT LAST_INSERT_ID()";
	if (mysql_query(&myConn, sql.c_str()))
		return -1;
	return atoi(mysql_fetch_row(mysql_store_result(&myConn))[0]);
}
