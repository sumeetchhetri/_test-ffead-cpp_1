/*
	Copyright 2009-2012, Sumeet Chhetri 
  
    Licensed under the Apache License, Version 2.0 (the "License"); 
    you may not use this file except in compliance with the License. 
    You may obtain a copy of the License at 
  
        http://www.apache.org/licenses/LICENSE-2.0 
  
    Unless required by applicable law or agreed to in writing, software 
    distributed under the License is distributed on an "AS IS" BASIS, 
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
    See the License for the specific language governing permissions and 
    limitations under the License.  
*/
/*+
 * CibernateConnectionPool.cpp
 *
 *  Created on: Jan 17, 2010
 *      Author: sumeet
 */

#include "CibernateConnectionPool.h"
/*static CibernateConnectionPool *instance = NULL;

CibernateConnectionPool* CibernateConnectionPool::getinstance()
{
	if(this==NULL)
		instance = new CibernateConnectionPool;
	return instance;
}*/
CibernateConnectionPool::CibernateConnectionPool(int size,string dbName,string uname,string pass)
{
	logger = Logger::getLogger("CibernateConnectionPool");
	createPool(size,dbName,uname,pass);
}

CibernateConnectionPool::~CibernateConnectionPool()
{
	for (int var = 0; var < readConnections.size(); ++var) {
		delete readConnections.at(var);
	}
	for (int var = 0; var < writeConnections.size(); ++var) {
		delete writeConnections.at(var);
	}
	SQLFreeHandle(SQL_HANDLE_ENV, V_OD_Env);
	logger << "\nDestructed CibernateConnectionPool" << flush;
}

void CibernateConnectionPool::closeConnection(Connection *conn)
{
	conn->busy = false;
}

void  CibernateConnectionPool::newConnection(bool read)
{
	int V_OD_erg;// result of functions
	SQLCHAR V_OD_msg[200],V_OD_stat[10];		// Status SQL;
	SQLSMALLINT	V_OD_mlen;
	SQLINTEGER V_OD_err_s;
	Connection *connection = new Connection;
	// 2. allocate connection handle, set timeout
	V_OD_erg = SQLAllocHandle(SQL_HANDLE_DBC, V_OD_Env, &(connection->conn));
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
		logger << "Error AllocHDB " << V_OD_erg << endl;
		SQLFreeHandle(SQL_HANDLE_ENV, V_OD_Env);
		this->initialized = false;
		return;
		//exit(0);
	}
	SQLSetConnectAttr(connection->conn, SQL_LOGIN_TIMEOUT, (SQLPOINTER *)5, 0);
	// 3. Connect to the datasource "MySQL-test"
	V_OD_erg = SQLConnect(connection->conn, (SQLCHAR*) this->dbName.c_str(), SQL_NTS,
									 (SQLCHAR*) this->uname.c_str(), SQL_NTS,
									 (SQLCHAR*) this->pass.c_str(), SQL_NTS);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
		logger << "Error SQLConnect " << V_OD_erg << endl;
		SQLGetDiagRec(SQL_HANDLE_DBC, connection->conn,1,
					  V_OD_stat, &V_OD_err_s,V_OD_msg,100,&V_OD_mlen);
		logger << V_OD_msg << " (" << (int)V_OD_err_s <<  ")" << endl;
		SQLFreeHandle(SQL_HANDLE_ENV, V_OD_Env);
		this->initialized = false;
		return;
		//exit(0);
	}
	connection->busy = false;
	connection->type = read;
	if(read)
		this->readConnections.push_back(connection);
	else
		this->writeConnections.push_back(connection);

}

void CibernateConnectionPool::createPool(int size,string dbName,string uname,string pass)
{
	this->dbName = dbName;
	this->uname = uname;
	this->pass = pass;
	this->readNumber = 0;
	int reads = round(size/5);
	if(reads<1)
		reads = 1;
	int V_OD_erg;// result of functions
	// 1. allocate Environment handle and register version
	this->initialized = true;
	V_OD_erg=SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&V_OD_Env);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
		logger << "Error AllocHandle" << endl;
		this->initialized = false;
	}
	V_OD_erg=SQLSetEnvAttr(V_OD_Env, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
	if((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
		logger << "Error SetEnv" << endl;
		SQLFreeHandle(SQL_HANDLE_ENV, V_OD_Env);
		this->initialized = false;
	}
	for(int i=0;i<reads;i++)
	{
		this->newConnection(true);
		if(!this->initialized)
			break;
	}
	for(int i=0;i<size-reads;i++)
	{
		this->newConnection(false);
		if(!this->initialized)
			break;
	}
	if(!this->initialized)
	{
		logger << "Failed to create pool !" << endl;
	}
	else
	{
		logger << "Created pool successfully !" << endl;
	}
}

Connection* CibernateConnectionPool::getReadConnection()
{
	if(this->readNumber==(int)this->readConnections.size())
		this->readNumber = 0;
	return this->readConnections.at(this->readNumber++);
}
Connection* CibernateConnectionPool::getWriteConnection()
{
	Timer t;
	t.start();
	while(true)
	{
		for(unsigned int i=0;i<this->writeConnections.size();i++)
		{
			if(!this->writeConnections.at(i)->busy)
				return this->writeConnections.at(i);
		}
		if(t.elapsedMilliSeconds()>500)
		{
			newConnection(false);
			return this->writeConnections.at(this->writeConnections.size()-1);
		}

	}
}

Connection::Connection()
{
	logger = Logger::getLogger("Connection");
	logger << "\nCreated Connection" << flush;
}

Connection::~Connection()
{
	SQLDisconnect(conn);
	SQLFreeHandle(SQL_HANDLE_DBC,conn);
	logger << "\nDestructed Connection" << flush;
}
