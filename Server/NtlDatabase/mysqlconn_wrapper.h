/* 
 * File:   mysqlconn_wrapper.h
 * Author: Eduardo Casas (www.eduardocasas.com)
 *
 * Created on February 24, 2013, 5:07 PM
 */

#ifndef MYSQLCONN_WRAPPER_H
#define	MYSQLCONN_WRAPPER_H

#include "stdafx.h"
#include "mysql_connection.h"
	
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>


#define	IP_SERVER_ALL "127.0.0.1"
using namespace std;

class MySQLConnWrapper
{
          
    public:
        /* Your MySQL server settings */
        MySQLConnWrapper()
        {
            host     = "tcp://127.0.0.1:3306";
            user     = "root";
            password = "your pass here";
			database = "dbo";
        };
        ~MySQLConnWrapper();
        void manageException(sql::SQLException& e);
        void connect();
        void switchDb(const string& db_name);
        void prepare(const string& query);
        void setInt(const int& num, const int& data);
		void setFloat(const int& num, const float& data);
        void setString(const int& num, const string& data);
        void execute(const string& query = "");
        bool fetch();
		long double getDouble(const char* index);
		bool getBoolean(const char* index);
		int getInt(const char* index);
		void setConfig(const char* _host, const char* _user, const char* _pass, const char* _db)
		{
			host = new char[strlen(_host)];
			user = new char[strlen(_user)];
			password = new char[strlen(_pass)];
			database = new char[strlen(_db)];
			strcpy(host, _host);
			strcpy(user, _user);
			if (strcmp(_pass, "NULL") != 0)
				strcpy(password, _pass);
			else
				password = "";
			strcpy(database, _db);
		};
		string getString(const char* index);
        string print(const string& field);
        string print(const int& index);
		size_t rowsCount();

    private:
        char* host;
        char* user;
        char* password;
		char* database;
        sql::Driver* driver;
        sql::Connection* con;
        sql::Statement* stmt;
        sql::PreparedStatement* prep_stmt;
        sql::ResultSet* res;
};


#endif	/* MYSQLCONN_WRAPPER_H */


