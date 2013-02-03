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
 * Reflection.h
 *
 *  Created on: Aug 21, 2009
 *      Author: sumeet
 */

#ifndef REFLECTION_H_
#define REFLECTION_H_
#include "PropFileReader.h"
#include "Method.h"
#include "Field.h"
#include "CastUtil.h"
#include "AfcUtil.h"
#include "Logger.h"
#include "RegexUtil.h"
class Reflection {
	strVec pub,pri,pro;
	bool prosetser;
	string classN,baseClassN,bcvisib;
	bool generateClassInfoFromDD(string);
	bool generateClassInfo(string);
	void collectInfo(string,string);
	strVec list(string);
	map<string,bool> methsall;
	map<string,bool> invalidcls;
	Logger logger;
public:
	Reflection();
	virtual ~Reflection();
	string updateClassDefinition(string, bool);
	string updateTemplateContextDefinition(string, bool);
	strVec getAfcObjectData(string,bool,strVec &);
	strVec getAfcObjectData(string,bool);
	propMap getDbTableInfo(string);
	string generateClassDefinitionsAll(strVec,string &);
	string generateSerDefinitionAll(strVec all,string &includeRef, bool isBinary);
	string generateClassDefinition(string,string &,string &,string &,string &,string &);
	string generateSerDefinition(string,string &,string &,string &,string &);
	string generateClassDefinitions(string,string &,string &,string &,string &,string &);
	string generateSerDefinitions(string,string &,string &,string &,string &,bool);

	string generateSerDefinitionBinary(string className,string &includesDefs,string &typedefs,string &classes,string &methods);
};

#endif /* REFLECTION_H_ */
