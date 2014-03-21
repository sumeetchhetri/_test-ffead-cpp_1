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
 * BinarySerialize.cpp
 *
 *  Created on: 12-Jun-2013
 *      Author: sumeetc
 */

#include "BinarySerialize.h"

BinarySerialize::BinarySerialize() {
	dlib = dlopen(INTER_LIB_FILE, RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		throw "Cannot load serialization shared library";
	}
	dlibinstantiated = true;
}

BinarySerialize::BinarySerialize(void* dlib) {
	if(dlib == NULL)
	{
		throw "Cannot load serialization shared library";
	}
	this->dlib = dlib;
	dlibinstantiated = false;
}

BinarySerialize::~BinarySerialize() {
	if(dlibinstantiated)
	{
		dlclose(dlib);
	}
}

string BinarySerialize::serializePrimitive(string className, void* t)
{
	string objXml;
	AMEFEncoder enc;
	AMEFObject object;

	if(className=="std::string" || className=="string")
	{
		string tem = *(string*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
	}
	else if(className=="char")
	{
		char tem = *(char*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
	}
	else if(className=="unsigned char")
	{
		unsigned char tem = *(unsigned char*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
	}
	else if(className=="int")
	{
		int tem = *(int*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
	}
	else if(className=="unsigned int")
	{
		unsigned int tem = *(unsigned int*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
	}
	else if(className=="short")
	{
		short tem = *(short*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
	}
	else if(className=="unsigned short")
	{
		unsigned short tem = *(unsigned short*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
	}
	else if(className=="long")
	{
		long tem = *(long*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
	}
	else if(className=="unsigned long")
	{
		unsigned long tem = *(unsigned long*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
	}
	else if(className=="long long")
	{
		long long tem = *(long long*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
	}
	else if(className=="unsigned long long")
	{
		unsigned long long tem = *(unsigned long long*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
	}
	else if(className=="float")
	{
		float tem = *(float*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
	}
	else if(className=="double")
	{
		double tem = *(double*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
	}
	else if(className=="long double")
	{
		long double tem = *(long double*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
	}
	else if(className=="bool")
	{
		bool tem = *(bool*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
	}
	else if(className=="Date")
	{
		string tem = *(string*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
	}
	else if(className=="BinaryData")
	{
		string tem = *(string*)t;
		object.addPacket(tem, className);
		objXml = enc.encodeB(&object, false);
	}
	return enc.encodeB(&object, false);
}

void* BinarySerialize::getSerializableObject()
{
	return new AMEFObject;
}

void BinarySerialize::cleanSerializableObject(void* _1)
{
	AMEFObject* object = (AMEFObject*)_1;
	delete object;
}

void BinarySerialize::startContainerSerialization(void* _1, string className, string container)
{
	StringUtil::replaceAll(container, "std::", "");
	StringUtil::replaceAll(container, "::", "_");
	StringUtil::replaceAll(className, "std::", "");
	StringUtil::replaceAll(className, "::", "_");
	StringUtil::replaceAll(className, "<", "-");
	StringUtil::replaceAll(className, ">", "-");
	StringUtil::replaceAll(className, ",", "-");
	if(className.at(className.length()-1)=='-')
		className = className.substr(0, className.length()-1);
	AMEFEncoder enc;
	AMEFObject* object = (AMEFObject*)_1;
	object->setName(container+"-"+className);
}

void BinarySerialize::endContainerSerialization(void* _1, string className, string container){}

void BinarySerialize::afterAddContainerSerializableElement(void* _1, int counter, int size){}

void BinarySerialize::addContainerSerializableElement(void* _1, string tem)
{
	AMEFEncoder enc;
	AMEFObject* object = (AMEFObject*)_1;
	object->addPacket(tem);
}

void BinarySerialize::addContainerSerializableElementMulti(void* _1, string tem)
{
	//tem = tem.substr(4);
	AMEFEncoder enc;
	AMEFObject* object = (AMEFObject*)_1;
	object->addPacket(tem);
}

string BinarySerialize::fromSerializableObjectToString(void* _1)
{
	AMEFEncoder enc;
	AMEFObject* object = (AMEFObject*)_1;
	return enc.encodeB(object, false);
}

string BinarySerialize::elementToSerializedString(void* _1, int counter)
{
	AMEFObject* object = (AMEFObject*)_1;
	return object->getPackets().at(counter)->getValueStr();
}

string BinarySerialize::getConatinerElementClassName(void* _1, string className)
{
	AMEFObject* root = (AMEFObject*)_1;
	string stlclassName = className;
	if(stlclassName.find(">")!=string::npos)
	{
		className = stlclassName.substr(stlclassName.find("<")+1);
		className = className.substr(0, className.find(">"));
	}
	else
	{
		className = stlclassName.substr(stlclassName.find("<")+1);
		if(className.find(",")!=string::npos)
		{
			className = className.substr(0, className.find_last_of(','));
		}
	}
	/*if(stlclassName.find("-")!=string::npos)
	{
		className = stlclassName.substr(stlclassName.find("-")+1);
	}*/
	StringUtil::trim(className);
	return className;
}

void* BinarySerialize::getContainerElement(void* _1, int counter, int counter1)
{
	AMEFDecoder dec;
	AMEFObject* root = (AMEFObject*)_1;
	AMEFObject* root2 = dec.decodeB(root->getPackets().at(counter)->getValue(), true, false);
	return root2;
}

void BinarySerialize::addPrimitiveElementToContainer(void* _1, int counter, string className, void* cont, string container)
{
	AMEFDecoder dec;
	AMEFObject* root = (AMEFObject*)_1;
	AMEFObject* root2 = dec.decodeB(root->getPackets().at(counter)->getValue(), true, false);
	if(className=="std::string" || className=="string")
	{
		string retVal = root2->getPackets().at(0)->getValueStr();
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="int")
	{
		int retVal = root2->getPackets().at(0)->getIntValue();
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="short")
	{
		short retVal = root2->getPackets().at(0)->getShortValue();
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="long")
	{
		long retVal = root2->getPackets().at(0)->getLongValue();
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="long long")
	{
		long long retVal = root2->getPackets().at(0)->getLongLongValue();
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="long double")
	{
		long double retVal = root2->getPackets().at(0)->getLongDoubleValue();
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="unsigned int")
	{
		unsigned int retVal = root2->getPackets().at(0)->getUIntValue();
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="unsigned short")
	{
		unsigned short retVal = root2->getPackets().at(0)->getUShortValue();
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="unsigned long")
	{
		unsigned long retVal = root2->getPackets().at(0)->getULongValue();
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="unsigned long long")
	{
		unsigned long long retVal = root2->getPackets().at(0)->getULongLongValue();
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="float")
	{
		float retVal = root2->getPackets().at(0)->getFloatValue();
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="double")
	{
		double retVal = root2->getPackets().at(0)->getDoubleValue();
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="bool")
	{
		bool retVal = root2->getPackets().at(0)->getBoolValue();
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="char")
	{
		char retVal = root2->getPackets().at(0)->getCharValue();
		addValueToNestedContainer(container, retVal, cont);
	}
	else if(className=="unsigned char")
	{
		unsigned char retVal = root2->getPackets().at(0)->getUCharValue();
		addValueToNestedContainer(container, retVal, cont);
	}
}

void* BinarySerialize::getUnserializableObject(string _1)
{
	AMEFDecoder dec;
	AMEFObject* root = dec.decodeB(_1, true, false);
	return root;
}

void BinarySerialize::cleanUnserializableObject(void* _1)
{
	AMEFObject* object = (AMEFObject*)_1;
	delete object;
}

void BinarySerialize::cleanValidUnserializableObject(void* _1)
{
	AMEFObject* object = (AMEFObject*)_1;
	delete object;
}

void* BinarySerialize::getValidUnserializableObject(string _1){return NULL;}

int BinarySerialize::getContainerSize(void* _1)
{
	AMEFObject* root = (AMEFObject*)_1;
	return root->getPackets().size();
}

string BinarySerialize::getUnserializableClassName(void* _1, string className)
{
	AMEFObject* root = (AMEFObject*)_1;
	return root->getNameStr();
}

void* BinarySerialize::getPrimitiveValue(void* _1, string className)
{
	AMEFObject* root = (AMEFObject*)_1;
	root = root->getPackets().at(0);
	if((className=="signed" || className=="int" || className=="signed int") && className==root->getNameStr())
	{
		int *vt = new int;
		*vt = root->getIntValue();
		return vt;
	}
	else if((className=="unsigned" || className=="unsigned int") && className==root->getNameStr())
	{
		unsigned int *vt = new unsigned int;
		*vt = root->getUIntValue();
		return vt;
	}
	else if((className=="short" || className=="short int" || className=="signed short" || className=="signed short int") && className==root->getNameStr())
	{
		short *vt = new short;
		*vt = root->getShortValue();
		return vt;
	}
	else if((className=="unsigned short" || className=="unsigned short int") && className==root->getNameStr())
	{
		unsigned short *vt = new unsigned short;
		*vt = root->getUShortValue();
		return vt;
	}
	else if((className=="long" || className=="long int" || className=="signed long" || className=="signed long int") && className==root->getNameStr())
	{
		long *vt = new long;
		*vt = root->getLongLongValue();
		return vt;
	}
	else if((className=="unsigned long" || className=="unsigned long int") && root->getNameStr()==className)
	{
		long *vt = new long;
		*vt = root->getLongLongValue();
		return vt;
	}
	else if((className=="long long" || className=="long long int") && className==root->getNameStr())
	{
		long long *vt = new long long;
		*vt = root->getULongLongValue();
		return vt;
	}
	else if((className=="unsigned long long" || className=="unsigned long long int") && className==root->getNameStr())
	{
		unsigned long long *vt = new unsigned long long;
		*vt = root->getULongLongValue();
		return vt;
	}
	else if((className=="char" || className=="signed char") && className==root->getNameStr())
	{
		char *vt = new char;
		*vt = root->getCharValue();
		return vt;
	}
	else if(className=="unsigned char" && className==root->getNameStr())
	{
		unsigned char *vt = new unsigned char;
		*vt = root->getUCharValue();
		return vt;
	}
	else if(className=="Date" && className==root->getNameStr())
	{
		DateFormat formt("yyyy-mm-dd hh:mi:ss");
		return formt.parse(root->getValueStr());
	}
	else if(className=="BinaryData" && className==root->getNameStr())
	{
		return BinaryData::unSerilaize(root->getValueStr());
	}
	else if(className=="float" && className==root->getNameStr())
	{
		float *vt = new float;
		*vt = root->getFloatValue();
		return vt;
	}
	else if(className=="double" && className==root->getNameStr())
	{
		double *vt = new double;
		*vt = root->getDoubleValue();
		return vt;
	}
	else if(className=="long double" && className==root->getNameStr())
	{
		long double *vt = new long double;
		*vt = root->getLongDoubleValue();
		return vt;
	}
	else if(className=="bool" && className==root->getNameStr())
	{
		bool *vt = new bool;
		*vt = root->getBoolValue();
		return vt;
	}
	else if((className=="std::string" || className=="string") && className==root->getNameStr())
	{
		string *vt = new string;
		*vt = root->getValueStr();
		return vt;
	}
	return NULL;
}

string BinarySerialize::getSerializationMethodName(string className, string appName, bool which, string type)
{
	string methodname;
	if(which)
		methodname = appName + "serialize" + className + type;
	else
		methodname = appName + "unSerialize" + className + type;
	return methodname;
}

string BinarySerialize::serializeUnknown(void* t,string className, string appName)
{
	BinarySerialize serialize;
	return _handleAllSerialization(className,t,appName, &serialize);
}

void* BinarySerialize::unSerializeUnknown(string objXml,string className, string appName)
{
	BinarySerialize serialize;
	return _handleAllUnSerialization(objXml,className,appName,&serialize,false,NULL);
}

bool BinarySerialize::isValidClassNamespace(void* _1, string className, string namespc, bool iscontainer)
{
	StringUtil::replaceAll(namespc, "::", "_");
	StringUtil::replaceAll(className, "std::", "");
	StringUtil::replaceAll(className, "::", "_");
	StringUtil::replaceAll(className, "<", "-");
	StringUtil::replaceAll(className, ">", "-");
	if(className.at(className.length()-1)=='-')
		className = className.substr(0, className.length()-1);
	if(className.find('-')!=string::npos)
	{
		string pre = className.substr(0, className.find_last_of("-")+1);
		className = className.substr(className.find_last_of("-")+1);
		className = pre + namespc + className;
	}
	else
		className = namespc + className;
	AMEFObject* element = (AMEFObject*)_1;
	if(element->getNameStr()!=className || (iscontainer &&  element->getPackets().size()==0))
		return false;
	return true;
}

bool BinarySerialize::isValidObjectProperty(void* _1, string propname, int counter)
{
	AMEFObject* element = (AMEFObject*)_1;
	if((int)element->getPackets().size()>counter && element->getPackets().at(counter)->getNameStr()==propname)
		return true;
	return false;
}

void* BinarySerialize::getObjectProperty(void* _1, int counter)
{
	AMEFObject* element = (AMEFObject*)_1;
	return element->getPackets().at(counter);
}

void BinarySerialize::startObjectSerialization(void* _1, string className)
{
	AMEFEncoder enc;
	AMEFObject* object = (AMEFObject*)_1;
	object->setName(className);
}

void BinarySerialize::endObjectSerialization(void* _1, string className){}

void BinarySerialize::afterAddObjectProperty(void* _1){}

void BinarySerialize::addObjectPrimitiveProperty(void* _1, string propName, string className, void* t)
{
	AMEFObject* object = (AMEFObject*)_1;
	if(className=="std::string" || className=="string")
	{
		string tem = *(string*)t;
		object->addPacket(tem, propName);
	}
	else if(className=="char")
	{
		char tem = *(char*)t;
		object->addPacket(tem, propName);
	}
	else if(className=="unsigned char")
	{
		unsigned char tem = *(unsigned char*)t;
		object->addPacket(tem, propName);
	}
	else if(className=="int")
	{
		int tem = *(int*)t;
		object->addPacket(tem, propName);
	}
	else if(className=="unsigned int")
	{
		unsigned int tem = *(unsigned int*)t;
		object->addPacket(tem, propName);
	}
	else if(className=="short")
	{
		short tem = *(short*)t;
		object->addPacket(tem, propName);
	}
	else if(className=="unsigned short")
	{
		unsigned short tem = *(unsigned short*)t;
		object->addPacket(tem, propName);
	}
	else if(className=="long")
	{
		long tem = *(long*)t;
		object->addPacket(tem, propName);
	}
	else if(className=="unsigned long")
	{
		unsigned long tem = *(unsigned long*)t;
		object->addPacket(tem, propName);
	}
	else if(className=="long long")
	{
		long long tem = *(long long*)t;
		object->addPacket(tem, propName);
	}
	else if(className=="unsigned long long")
	{
		unsigned long long tem = *(unsigned long long*)t;
		object->addPacket(tem, propName);
	}
	else if(className=="float")
	{
		float tem = *(float*)t;
		object->addPacket(tem, propName);
	}
	else if(className=="double")
	{
		double tem = *(double*)t;
		object->addPacket(tem, propName);
	}
	else if(className=="long double")
	{
		long double tem = *(long double*)t;
		object->addPacket(tem, propName);
	}
	else if(className=="bool")
	{
		bool tem = *(bool*)t;
		object->addPacket(tem, propName);
	}
	else if(className=="Date")
	{
		string tem = *(string*)t;
		object->addPacket(tem, propName);
	}
	else if(className=="BinaryData")
	{
		string tem = *(string*)t;
		object->addPacket(tem, propName);
	}
}

void BinarySerialize::addObjectProperty(void* _1, string propName, string className, string t)
{
	AMEFObject* object = (AMEFObject*)_1;
	object->addPacket(t, propName);
}

void* BinarySerialize::getObjectPrimitiveValue(void* _1, string className, string propName)
{
	AMEFObject* root = (AMEFObject*)_1;
	if((className=="signed" || className=="int" || className=="signed int") && root->getNameStr()==propName)
	{
		int *vt = new int;
		*vt = root->getIntValue();
		return vt;
	}
	else if((className=="unsigned" || className=="unsigned int") && root->getNameStr()==propName)
	{
		unsigned int *vt = new unsigned int;
		*vt = root->getUIntValue();
		return vt;
	}
	else if((className=="short" || className=="short int" || className=="signed short" || className=="signed short int") && root->getNameStr()==propName)
	{
		short *vt = new short;
		*vt = root->getShortValue();
		return vt;
	}
	else if((className=="unsigned short" || className=="unsigned short int") && root->getNameStr()==propName)
	{
		unsigned short *vt = new unsigned short;
		*vt = root->getUShortValue();
		return vt;
	}
	else if((className=="long" || className=="long int" || className=="signed long" || className=="signed long int") && root->getNameStr()==propName)
	{
		long *vt = new long;
		*vt = root->getLongLongValue();
		return vt;
	}
	else if((className=="unsigned long" || className=="unsigned long int") && root->getNameStr()==propName)
	{
		long *vt = new long;
		*vt = root->getLongLongValue();
		return vt;
	}
	else if((className=="long long" || className=="long long int" || className=="signed long long int") && root->getNameStr()==propName)
	{
		long long *vt = new long long;
		*vt = root->getULongLongValue();
		return vt;
	}
	else if((className=="unsigned long long" || className=="unsigned long long int") && root->getNameStr()==propName)
	{
		unsigned long long *vt = new unsigned long long;
		*vt = root->getULongLongValue();
		return vt;
	}
	else if((className=="char" || className=="signed char") && root->getNameStr()==propName)
	{
		char *vt = new char;
		*vt = root->getCharValue();
		return vt;
	}
	else if(className=="unsigned char" && root->getNameStr()==propName)
	{
		unsigned char *vt = new unsigned char;
		*vt = root->getUCharValue();
		return vt;
	}
	else if(className=="Date" && root->getNameStr()==propName)
	{
		DateFormat formt("yyyy-mm-dd hh:mi:ss");
		return formt.parse(root->getValueStr());
	}
	else if(className=="BinaryData" && root->getNameStr()==propName)
	{
		return BinaryData::unSerilaize(root->getValueStr());
	}
	else if(className=="float" && root->getNameStr()==propName)
	{
		float *vt = new float;
		*vt = root->getFloatValue();
		return vt;
	}
	else if(className=="double" && root->getNameStr()==propName)
	{
		double *vt = new double;
		*vt = root->getDoubleValue();
		return vt;
	}
	else if(className=="long double" && root->getNameStr()==propName)
	{
		long double *vt = new long double;
		*vt = root->getLongDoubleValue();
		return vt;
	}
	else if(className=="bool" && root->getNameStr()==propName)
	{
		bool *vt = new bool;
		*vt = root->getBoolValue();
		return vt;
	}
	else if((className=="std::string" || className=="string") && root->getNameStr()==propName)
	{
		string *vt = new string;
		*vt = root->getValueStr();
		return vt;
	}
	return NULL;
}

string BinarySerialize::serializeUnknownBase(void* t,string className, string appName)
{
	return _handleAllSerialization(className,t,appName, this);
}
void* BinarySerialize::unSerializeUnknownBase(void* unserObj,string className, string appName)
{
	return _handleAllUnSerialization("",className,appName,this,false,unserObj);
}
