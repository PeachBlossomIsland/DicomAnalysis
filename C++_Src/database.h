#pragma once
#ifndef dataBase_H
#define dataBase_H
#include<string>
#include "mysql.h"
using namespace std;
class DataBase
{
public:
	~DataBase();
	DataBase(string host, string user, string pswd, string db, unsigned int port = 3306);
	int DoSelect(string sql);
	int DoDelete(string sql);//返回受影响的行数
	int DoInsert(string sql);//返回受影响的行数
	int DoUpdate(string sql);//返回受影响的行数
	bool InitConnection();
	MYSQL_RES* GetMysqlRes();
	int GetTableId();
private:
	string host;
	string user;
	string password;
	string database;
	unsigned int port;
	MYSQL myConn;
	MYSQL_RES* result;
};
#endif // !dataBase