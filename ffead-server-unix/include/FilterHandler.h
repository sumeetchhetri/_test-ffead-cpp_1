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
 * FilterHandler.h
 *
 *  Created on: Jun 17, 2012
 *      Author: Sumeet
 */

#ifndef FILTERHANDLER_H_
#define FILTERHANDLER_H_
#include "Filter.h"
#include "Reflector.h"
#include <dlfcn.h>
#include "Logger.h"

typedef ClassInfo (*FunPtr) ();

class FilterHandler {
public:
	FilterHandler();
	virtual ~FilterHandler();
	static void handleIn(HttpRequest* req, HttpResponse& res, map<string, vector<string> > filterMap, void* dlib,
			string ext);
	static void handleOut(HttpRequest* req, HttpResponse& res, map<string, vector<string> > filterMap, void* dlib,
			string ext);
};

#endif /* FILTERHANDLER_H_ */
