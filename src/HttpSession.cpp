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
 * HttpSession.cpp
 *
 *  Created on: Aug 11, 2009
 *      Author: sumeet
 */

#include "HttpSession.h"

#include <iostream>
HttpSession::HttpSession()
{
	this->dirty = false;
}

HttpSession::~HttpSession()
{
	//delete this;
}
string HttpSession::getSessionId() const
{
	return sessionId;
}

void HttpSession::setSessionId(string sessionId)
{
	this->sessionId = sessionId;
}

Map HttpSession::getSessionAttributes()
{
	return sessionAttributes;
}

void HttpSession::setSessionAttributes(Map sessionAttributes)
{
	map<string,string>::iterator it;
	for(it=sessionAttributes.begin();it!=sessionAttributes.end();it++)
	{
		this->sessionAttributes[it->first] = it->second;
	}
}
string HttpSession::getAttribute(string key)
{
	string tem;
	if(sessionAttributes.find(key)!=sessionAttributes.end())
		return sessionAttributes[key];
	else return tem;
}

void HttpSession::setAttribute(string key,string value)
{
	this->sessionAttributes[key] = value;
	this->dirty = true;
}
