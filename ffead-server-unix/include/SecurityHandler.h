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
/*
 * SecurityHandler.h
 *
 *  Created on: Jun 17, 2012
 *      Author: Sumeet
 */

#ifndef SECURITYHANDLER_H_
#define SECURITYHANDLER_H_
#include "FileAuthController.h"
#include "Reflector.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "Logger.h"

typedef ClassInfo (*FunPtr) ();

class SecureAspect
{
public:
	string path;
	string role;
};

class Security
{
	Logger logger;
public:
	Security();
	~Security();
	vector<SecureAspect> secures;
	string loginProvider;
	string loginUrl;
	string welocmeFile;
	long sessTimeout;
	bool isLoginConfigured()
	{
		return (loginProvider!="" && loginUrl!="");
	}
	bool isSecureConfigured()
	{
		return secures.size()!=0;
	}
	bool isLoginUrl(string url, string actUrl)
	{
		return (actUrl==(url+"/_ffead_security_cntxt_login_url"));
	}
	bool isLoginPage(string url, string actUrl)
	{
		return (actUrl==(url+"/"+loginUrl));
	}
	SecureAspect matchesPath(string url);
};


class SecurityHandler {
	static Logger logger;
public:
	SecurityHandler();
	virtual ~SecurityHandler();
	static bool handle(string ip_addr, HttpRequest* req, HttpResponse& res, map<string, Security> securityObjectMap,
			long sessionTimeout, void* dlib, map<string, string> cntMap);
};

#endif /* SECURITYHANDLER_H_ */
