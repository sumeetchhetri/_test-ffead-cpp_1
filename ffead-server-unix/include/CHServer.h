/*
	Copyright 2010, Sumeet Chhetri 
  
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
/*
 * CHServer.h
 *
 *  Created on: Aug 11, 2009
 *      Author: sumeet
 */

#ifndef CHSERVER_H_
#define CHSERVER_H_
#include "Cibernate.h"
#include <algorithm>
#include "Client.h"
#include "PropFileReader.h"
#include "AfcUtil.h"
#include "string"
#include "Controller.h"

#include "PropFileReader.h"
#include "TemplateEngine.h"
#include "DCPGenerator.h"
#include "Reflection.h"
#include <cstdlib>
#include <dlfcn.h>

#include "WsUtil.h"
#include "sstream"

#include "ClassInfo.h"

#include "View.h"
#include "XmlParser.h"
#include "TemplateHandler.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include "DynamicView.h"
#include "HttpRequest.h"
#include "ApplicationUtil.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/epoll.h>
#include <execinfo.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <queue>
#include <sys/uio.h>
#include <sys/un.h>
#include "Cibernate.h"
#include "ComponentGen.h"
#include "ComponentHandler.h"
#include "MessageHandler.h"
#include "MethodInvoc.h"
#include "Reflector.h"
#include "AppContext.h"

#include "Logger.h"
#include "ThreadPool.h"
#include "FileAuthController.h"
#include <sys/stat.h>
#include "Timer.h"
#include "DateFormat.h"
#include "Filter.h"
#include "FormController.h"
#include "RestController.h"
#include "SecurityHandler.h"
#include "FilterHandler.h"
#include "AuthHandler.h"
#include "ControllerHandler.h"
#include "FormHandler.h"
#include "SoapHandler.h"
#include "ScriptHandler.h"
#include "FviewHandler.h"
#include "ExtHandler.h"
#include "ServiceTask.h"
#include "ConfigurationHandler.h"
#include "SSLHandler.h"
#include "ServiceTask.h"
#include "Thread.h"
#ifdef WINDOWS
    #include <direct.h>
    #define pwd _getcwd
#else
    #include <unistd.h>
    #define pwd getcwd
 #endif




//#define CA_LIST "root.pem"
#define HOST1	"localhost"
//#define RANDOM1  "random.pem"
#define PORT1	4433
#define BUFSIZZ 1024
//#define KEYFILE "server.pem"
//#define PASSWORD "password"
//#define DHFILE "dh1024.pem"

#define MAXEPOLLSIZE 100
#define BACKLOGM 500

typedef bool (*FunPtr1) (void *);
typedef ClassInfo (*FunPtr) ();
typedef void* (*toVoidP) (string);
typedef string (*DCPPtr) ();
typedef void (*ReceiveTask1)(int);

using namespace std;
/*class SharedData
{
private:
	static SharedData* shared_instance;
	void *dlib;
public:
	static void init()
	{
		if(shared_instance==NULL)
		{
			shared_instance = new SharedData();
			shared_instance->dlib = NULL;
			logger << "\nInitialised Shared data" << endl;
		}
	}
	static void setDLIB(void *dlib)
	{
		shared_instance->dlib = dlib;
	}
	static void* getDLIB()
	{
		return shared_instance->dlib;
	}
	static void deinit()
	{
		if(shared_instance->dlib!=NULL)dlclose(shared_instance->dlib);
		shared_instance->~SharedData();
		shared_instance = NULL;
		logger << "\nclosed handle" << endl;
	}
};*/


class CHServer {
public:
	CHServer();
	virtual ~CHServer();
};

#endif /* CHSERVER_H_ */
