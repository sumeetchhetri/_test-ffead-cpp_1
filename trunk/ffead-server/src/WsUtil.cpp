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
 * WsUtil.cpp
 *
 *  Created on: Sep 21, 2009
 *      Author: sumeet
 */

#include "WsUtil.h"

WsUtil::WsUtil() {
	logger = Logger::getLogger("WsUtil");
}

WsUtil::~WsUtil() {
	// TODO Auto-generated destructor stub
}

string WsUtil::generateAllWSDL(vector<string> files,string resp,map<string,string> &wsmap,string ip_address)
{
	string ret,headers="#include \"Exception.h\"\n#include \"string\"\n#include <sstream>\n#include \"CastUtil.h\"\n#include \"Element.h\"\ntypedef vector<Element> ElementList;\ntypedef map<string,string> AttributeList;\n";
	for(unsigned int var = 0; var < files.size(); ++var)
	{
		string webdir = resp+"../web/"+files.at(var);
		ret += generateWSDL(webdir+"/config/ws.xml",webdir+"/include/",resp,headers,wsmap,files.at(var),ip_address);
	}
	ret = (headers + "using namespace std;\n\n" + ret);
	return ret;
}

string WsUtil::generateWSDL(string file,string usrinc,string resp,string &headers,map<string,string> &wsmap,string appname,string ip_address)
{
	XmlParser parser("Parser");
	Document doc = parser.getDocument(file);
	Element root = doc.getRootElement();
	typedef vector<Element> ElementList;
	typedef map<string,string> strMap;
	typedef map<string,strMap> meth_Map;
	typedef map<string,meth_Map> ws_inp_out_Map;
	ElementList wsvcs = root.getChildElements();
	if(wsvcs.size()==0)
		return "";
	Reflection ref;
	ws_inp_out_Map ws_info;
	string ws_funcs,obj_mapng,retObj_xml;
	for(unsigned int i=0;i<wsvcs.size();i++)
	{
		meth_Map meth_info;
		StringContext gcntxt;
		string ws_name,reqr_res_bind,wsdl_msgs,wsdl_ops,wsdl_bind,wsdl,wsdl_obj_bind;
		Element ws = wsvcs.at(i);
		ws_name = ws.getAttribute("class");
		if(StringUtil::trimCopy(ws_name)=="")
		{
			logger << ("No class name defined for web-service, skipping...") << endl;
			continue;
		}
		if(StringUtil::trimCopy(ws.getAttribute("location"))=="")
		{
			logger << ("No location defined for web-service, skipping...") << endl;
			continue;
		}
		gcntxt["WS_NAME"] = ws_name;
		gcntxt["WS_PATH"] = ws.getAttribute("location");
		if(gcntxt["WS_PATH"].at(0)=='/')
		{
			gcntxt["WS_PATH"] = gcntxt["WS_PATH"].substr(1);
			StringUtil::replaceAll(gcntxt["WS_PATH"], "//", "/");
		}
		gcntxt["WS_NMSPC"] = StringUtil::trimCopy(ws.getAttribute("namespace"));
		if(gcntxt["WS_NMSPC"]=="")
		{
			logger << ("No location defined for web-service, skipping...") << endl;
			continue;
		}
		logger << ("Web service " + ws_name + " found for appname " + appname) << endl;
		strVec info = ref.getAfcObjectData(usrinc+ws.getAttribute("class")+".h", false);
		headers.append("#include \""+ws.getAttribute("class")+".h\"\n");

		for(unsigned int i=0;i<info.size();i++)
		{
			string temp,temp11;
			strMap in_out_info;
			temp = info.at(i);
			temp11 = temp.substr(temp.find("(")+1);
			temp = temp.substr(0,temp.find("("));

			//StringUtil::replaceFirst(temp,"("," ");
			StringUtil::replaceAll(temp11,";","");
			StringUtil::replaceFirst(temp11,")","");
			strVec results,results1;
			StringUtil::split(results, temp, (" "));
			StringUtil::split(results1, temp11, (","));
			string retType,methName,inp_params;
			if(results.size()<2)
				continue;
			if(results.size()>0 && (results.at(0)==ws.getAttribute("class") || temp.find("~")!=string::npos))
				continue;

			methName = results.at(1);

			in_out_info["RETURN"] = ws.getElementByName(methName).getAttribute("outname");
			//logger << in_out_info["RETURN"] << flush;
			in_out_info["RETURNTYP"] = results.at(0);
			if(results.at(0).find("vector<")!=string::npos)
			{
				string vecn = results.at(0);
				StringUtil::replaceFirst(vecn,"vector<"," ");
				StringUtil::replaceFirst(vecn,">"," ");
				StringUtil::trim(vecn);
				if(vecn=="int" || vecn=="string" || vecn=="short" || vecn=="float" || vecn=="double" || vecn=="bool" || vecn=="long long" || vecn=="long"
						|| vecn=="unsigned int" || vecn=="unsigned short" || vecn=="unsigned long long" || vecn=="unsigned long")
				{
					if(vecn=="long long")
					{
						vecn = "long";
					}
					else if(vecn=="unsigned long long")
					{
						vecn = "unsignedlong";
					}
					else if(vecn.find("unsigned")==0)
					{
						StringUtil::replaceAll(vecn, " ", "");
					}
					else if(vecn=="bool")
					{
						vecn = "boolean";
					}
					retType = "\n<xsd:element minOccurs=\"0\" maxOccurs=\"unbounded\" name=\""+in_out_info["RETURN"]+"\" type=\"xsd:"+vecn+"\"/>";
				}
				else
				{
					results1.push_back(vecn);
					retType = "\n<xsd:element minOccurs=\"0\" maxOccurs=\"unbounded\" name=\""+in_out_info["RETURN"]+"\" type=\"ns0:"+vecn+"\"/>";
				}
			}
			else
			{
				if(results.at(0)!="void")
				{
					if(results.at(0)=="int" || results.at(0)=="string" || results.at(0)=="short" || results.at(0)=="float" || results.at(0)=="double"
							|| results.at(0)=="bool" || results.at(0)=="long long" || results.at(0)=="long"
							|| results.at(0)=="unsigned int" || results.at(0)=="unsigned short" || results.at(0)=="unsigned long long"
							|| results.at(0)=="unsigned long")
					{
						if(results.at(0)=="long long")
						{
							results.at(0) = "long";
						}
						else if(results.at(0)=="unsigned long long")
						{
							results.at(0) = "unsignedlong";
						}
						else if(results.at(0).find("unsigned")==0)
						{
							StringUtil::replaceAll(results.at(0), " ", "");
						}
						else if(results.at(0)=="bool")
						{
							results.at(0) = "boolean";
						}
						retType = "\n<xsd:element name=\""+in_out_info["RETURN"]+"\" type=\"xsd:"+results.at(0)+"\"/>";
					}
					else
					{
						retType = "\n<xsd:element name=\""+in_out_info["RETURN"]+"\" type=\"ns0:"+results.at(0)+"\"/>";
					}
				}
			}
			for(unsigned int j=0;j<results1.size();j++)
			{
				string type;
				strVec results2;
				StringUtil::split(results2, results1.at(j), (" "));
				if(results2.size()<2)continue;
				type = results2.at(0);
				int srn = j;
				char chr = CastUtil::lexical_cast<char>(j);
				stringstream ss;
				ss << srn;
				string te;
				ss >> te;
				if(type=="int" || type=="string" || type=="short" || type=="float" || type=="double" || type=="bool" || type=="long long" || type=="long"
						|| type=="unsigned int" || type=="unsigned short" || type=="unsigned long long" || type=="unsigned long")
				{
					inp_params.append("\n<xsd:element name=\""+results2.at(1)+"\" type=\"xsd:"+type+"\"/>");
					in_out_info[chr+results2.at(1)] = type;
				}
				else if(type!="")
				{
					if(results2.size()>=2)
					{
						in_out_info[chr+results2.at(1)] = type;
					}
					else
					{
						logger << ("Invalid thing happening " + results1.at(j)) << endl;
					}
					strMap allfs,tyfs;
					if(type.find("vector<")!=string::npos && results2.size()==2)
					{
						string vecn = type;
						StringUtil::replaceFirst(vecn,"vector<"," ");
						StringUtil::replaceFirst(vecn,">"," ");
						StringUtil::trim(vecn);
						if(vecn=="int" || vecn=="short" || vecn=="float" || vecn=="double" || vecn=="bool" || vecn=="long long" || vecn=="long"
								|| vecn=="string" || vecn=="unsigned int" || vecn=="unsigned short" || vecn=="unsigned long long" || vecn=="unsigned long")
						{

						}
						else
						{
							headers.append("#include \""+vecn+".h\"\n");
						}
						type = vecn;
						inp_params.append("\n<xsd:element minOccurs=\"0\" maxOccurs=\"unbounded\" name=\""+results2.at(1)+"\" type=\"ns0:"+type+"\"/>");
					}
					else if(results2.size()==2)
					{
						headers.append("#include \""+type+".h\"\n");
						inp_params.append("\n<xsd:element minOccurs=\"0\" name=\""+results2.at(1)+"\" type=\"ns0:"+type+"\"/>");
					}

					if(type=="int" || type=="string" || type=="short" || type=="float" || type=="double" || type=="bool" || type=="long long" || type=="long"
							|| type=="unsigned int" || type=="unsigned short" || type=="unsigned long long" || type=="unsigned long")
					{
						continue;
					}
					strVec onjinf = ref.getAfcObjectData(usrinc+type+".h", false);
					obj_mapng.append(type+" _" + appname + "getObj"+type+"(Element ele)\n{\n");
					obj_mapng.append(type+" _obj;\n");
					retObj_xml.append("string _" + appname + "getRetXmlFor"+type+"("+type+" _obj,string namespce,bool full)\n{\n");
					for(unsigned int k=0;k<onjinf.size();k++)
					{
						string temp1,field,type;
						temp1 = onjinf.at(k);
						if(temp1.find(",")!=string::npos)
							continue;
						size_t s = temp1.find("get");
						size_t s1 = temp1.find("set");
						if(s!=string::npos)
						{
							size_t e = temp1.find("(");
							field = temp1.substr(s+3,e-s+3);
							field = AfcUtil::reverseCamelCased(field);
							field = field.substr(0,field.find("("));
							//logger << "\nField--- " << field << flush;
							allfs[field] = "";
						}
						else if(s1!=string::npos)
						{
							size_t e = temp1.find("(");
							size_t ed = temp1.find(")");
							field = temp1.substr(s1+3,e-s1+3);
							field = AfcUtil::reverseCamelCased(field);
							field = field.substr(0,field.find("("));
							type = temp1.substr(e+1,ed-e-1);
							strVec results3;
							StringUtil::split(results3, type, (" "));
							type = results3.at(0);
							//logger << "\nField--- " << field << flush;
							//logger << "\nType--- " << type << flush;
							allfs[field] = field;
							tyfs[field] = type;
						}
					}
					strMap::iterator iter;
					bool flag = false;
					string obj_binding;
					retObj_xml.append("string _ret;\n");
					retObj_xml.append("string val;\n");
					for(iter=allfs.begin();iter!=allfs.end();iter++)
					{
						if(iter->second!="")
						{
							flag = true;
							string typ;
							typ = tyfs[iter->second];
							if(typ=="int" || typ=="string" || typ=="short" || typ=="float" || typ=="double" || typ=="bool" || typ=="long long" || typ=="long"
									|| typ=="unsigned int" || typ=="unsigned short" || typ=="unsigned long long" || typ=="unsigned long")
							{
								if(typ=="long long")
								{
									typ = "long";
								}
								else if(typ=="unsigned long long")
								{
									typ = "unsignedlong";
								}
								else if(typ.find("unsigned")==0)
								{
									StringUtil::replaceAll(typ, " ", "");
								}
								else if(typ=="bool")
								{
									typ = "boolean";
								}
								retObj_xml.append("val = CastUtil::lexical_cast<string>(_obj.get"+AfcUtil::camelCased(iter->second)+"());\n");
								obj_binding.append("\n<xsd:element name=\""+iter->second+"\" type=\"xsd:"+typ+"\"/>");
							}
							else
							{
								retObj_xml.append("val = _" + appname + "getRetXmlFor"+type+"(_obj.get"+AfcUtil::camelCased(iter->second)+"());\n");
								obj_binding.append("\n<xsd:element name=\""+iter->second+"\" type=\"ns0:"+typ+"\"/>");
							}
							obj_mapng.append("_obj.set"+AfcUtil::camelCased(iter->second)+"(CastUtil::lexical_cast<"+typ+">(ele.getElementByName(\""+iter->second+"\").getText()));\n");
							retObj_xml.append("_ret.append(\"<\"+namespce+\":"+iter->second+">\"+val+\"</\"+namespce+\":"+iter->second+">\");\n");
						}
					}
					obj_mapng.append("return _obj;\n}\n");
					retObj_xml.append("return _ret;\n}\n");
					if(flag)
					{
						StringContext cntxt1;
						cntxt1["OBJ"] = type;
						cntxt1["OBJ_MEMBERS"] = obj_binding;
						wsdl_obj_bind.append(TemplateEngine::evaluate(resp+"templateObjBin.wsdl",cntxt1));
					}
				}
			}
			meth_info[methName] = in_out_info;
			StringContext cntxt;
			cntxt["METH_NAME"] = methName;
			cntxt["RET_TYPE"] = retType;
			cntxt["INP_PARAMS"] = inp_params;
			reqr_res_bind.append("\n"+TemplateEngine::evaluate(resp+"templateReqRes.wsdl",cntxt));
			wsdl_msgs.append("\n"+TemplateEngine::evaluate(resp+"templateWsdlMsg.wsdl",cntxt));
			wsdl_ops.append("\n"+TemplateEngine::evaluate(resp+"templateWsdlOpe.wsdl",cntxt));
			wsdl_bind.append("\n"+TemplateEngine::evaluate(resp+"templateWsdlBind.wsdl",cntxt));
		}
		ws_info[ws_name] = meth_info;
		gcntxt["REQ_RES_BINDING"] = reqr_res_bind;
		gcntxt["WSDL_MESSAGES"] = wsdl_msgs;
		gcntxt["WSDL_OPERATIONS"] = wsdl_ops;
		gcntxt["WSDL_BINDING"] = wsdl_bind;
		gcntxt["OBJ_BINDING"] = wsdl_obj_bind;
		gcntxt["URL"] = "http://" + ip_address + "/" + appname;
		string wspath = gcntxt["URL"] + "/" + gcntxt["WS_PATH"];
		wsmap[wspath] = ws_name;
		wsdl = TemplateEngine::evaluate(resp+"template.wsdl",gcntxt);
		AfcUtil::writeTofile(resp+"../web/"+appname+"/public/"+ws_name+".wsdl",wsdl,true);
		ws_funcs.append(obj_mapng);
		ws_funcs.append(retObj_xml);
		ws_funcs.append("extern \"C\"\n{\n");
		ws_inp_out_Map::iterator iter;
		for(iter=ws_info.begin();iter!=ws_info.end();iter++)
		{
			string ws_n = iter->first;
			meth_Map meth = iter->second;
			meth_Map::iterator iter1;
			for(iter1=meth.begin();iter1!=meth.end();iter1++)
			{
				string me_n = iter1->first;
				strMap pars = iter1->second;
				ws_funcs.append("string "+appname+me_n+ws_n+"(Element _req)\n{\nElement ele;\n");
				ws_funcs.append("string _retStr;\n");
				ws_funcs.append("try{\n");
				strMap::iterator iter2;
				string args;
				unsigned int ter = 1;
				//logger << me_n << ws_n << pars.size() << endl;
				for(iter2=pars.begin();iter2!=pars.end();iter2++)
				{
					if(iter2->first!="RETURN" && iter2->first!="RETURNTYP")
					{
						string argname =  iter2->first.substr(1);
						if(iter2->second=="int" || iter2->second=="short" || iter2->second=="double" || iter2->second=="float" || iter2->second=="string"
								|| iter2->second=="bool" || iter2->second=="long" || iter2->second=="long long" || iter2->second=="unsigned int"
									|| iter2->second=="unsigned short" || iter2->second=="unsigned long" || iter2->second=="unsigned long long"
											|| iter2->second=="string")
						{
							if(iter2->second=="long long")
							{
								iter2->second = "long";
							}
							else if(iter2->second=="unsigned long long")
							{
								iter2->second = "unsignedlong";
							}
							else if(iter2->second.find("unsigned")==0)
							{
								StringUtil::replaceAll(iter2->second, " ", "");
							}
							else if(iter2->second=="bool")
							{
								iter2->second = "boolean";
							}
							ws_funcs.append("ele = _req.getElementByName(\""+argname+"\");\n");
							ws_funcs.append(iter2->second+" "+argname+";\n");
							ws_funcs.append("if(ele.getTagName()!=\"\")");
							ws_funcs.append(argname+" = CastUtil::lexical_cast<"+iter2->second+">(ele.getText());\n");
						}
						else if(iter2->second.find("vector<")!=string::npos)
						{
							string vecn = iter2->second;
							StringUtil::replaceFirst(vecn,"vector<"," ");
							StringUtil::replaceFirst(vecn,">"," ");
							StringUtil::trim(vecn);
							if(vecn=="int" || vecn=="short" || vecn=="float" || vecn=="double" || vecn=="bool" || vecn=="long long" || vecn=="long"
									|| vecn=="string" || vecn=="unsigned int" || vecn=="unsigned short" || vecn=="unsigned long long" || vecn=="unsigned long")
							{
								ws_funcs.append("vector<"+vecn+" > "+argname+";\n");
								ws_funcs.append("ElementList list = _req.getElementsByName(\""+argname+"\");\n");
								ws_funcs.append("for(int i=0;i<list.size();i++)");
								ws_funcs.append(argname+".push_back(CastUtil::lexical_cast<"+vecn+">(list.at(i).getText()));\n");
							}
							else
							{
								ws_funcs.append("vector<"+vecn+" > "+argname+";\n");
								ws_funcs.append("ElementList list = _req.getElementsByName(\""+argname+"\");\n");
								ws_funcs.append("for(int i=0;i<list.size();i++)");
								ws_funcs.append(argname+".push_back(_" + appname + "getObj"+vecn+"(list.at(i)));\n");
							}
						}
						else
						{
							ws_funcs.append("ele = _req.getElementByName(\""+argname+"\");\n");
							ws_funcs.append(iter2->second+" "+argname+";\n");
							ws_funcs.append("if(ele.getTagName()!=\"\")");
							ws_funcs.append(argname+" = _" + appname + "getObj"+iter2->second+"(ele);\n");
						}
						args.append(argname);
						if(ter++<pars.size()-2)
							args.append(",");
					}
				}
				ws_funcs.append(ws_n+" _obj;\n");

				ws_funcs.append("AttributeList attl = _req.getAttributes();\n");
				ws_funcs.append("AttributeList::iterator it;\n");
				ws_funcs.append("_retStr = \"<\" + _req.getTagNameSpc() + \"Response\";\n");
				ws_funcs.append("for(it=attl.begin();it!=attl.end();it++)\n");
				ws_funcs.append("_retStr.append(\" \" + it->first + \"=\\\"\" + it->second + \"\\\" \");\n");
				ws_funcs.append("_retStr.append(\">\");\n");
				if(pars["RETURNTYP"]=="void")
				{
					ws_funcs.append("_obj."+me_n+"("+args+");\n");
				}
				else if(pars["RETURNTYP"]=="int" || pars["RETURNTYP"]=="short" || pars["RETURNTYP"]=="float" || pars["RETURNTYP"]=="double"
						|| pars["RETURNTYP"]=="bool" || pars["RETURNTYP"]=="long long" || pars["RETURNTYP"]=="long"
						|| pars["RETURNTYP"]=="unsigned int" || pars["RETURNTYP"]=="unsigned short" || pars["RETURNTYP"]=="string"
						|| pars["RETURNTYP"]=="unsigned long long" || pars["RETURNTYP"]=="unsigned long")
				{
					if(pars["RETURNTYP"]=="long long")
					{
						pars["RETURNTYP"] = "long";
					}
					else if(pars["RETURNTYP"]=="unsigned long long")
					{
						pars["RETURNTYP"] = "unsignedlong";
					}
					else if(pars["RETURNTYP"].find("unsigned")==0)
					{
						StringUtil::replaceAll(pars["RETURNTYP"], " ", "");
					}
					else if(pars["RETURNTYP"]=="bool")
					{
						pars["RETURNTYP"] = "boolean";
					}
					ws_funcs.append(pars["RETURNTYP"]+" _retval;\n");
					ws_funcs.append("_retval = _obj."+me_n+"("+args+");\n");
					ws_funcs.append("_retStr += \"<\" + _req.getNameSpc() +\":"+pars["RETURN"]+">\"+_retval+\"</\" + _req.getNameSpc() + \":"+pars["RETURN"]+">\";\n");
				}
				else if(pars["RETURNTYP"]!="")
				{
					ws_funcs.append(pars["RETURNTYP"]+" _retval;\n");
					ws_funcs.append("_retval = _obj."+me_n+"("+args+");\n");
					if(pars["RETURNTYP"].find("vector<")!=string::npos)
					{
						string vecn = pars["RETURNTYP"];
						StringUtil::replaceFirst(vecn,"vector<"," ");
						StringUtil::replaceFirst(vecn,">"," ");
						StringUtil::trim(vecn);
						if(vecn=="string" || vecn=="int" || vecn=="short" || vecn=="float" || vecn=="double" || vecn=="bool" || vecn=="long long" || vecn=="long"
								|| vecn=="unsigned int" || vecn=="unsigned short" || vecn=="unsigned long long" || vecn=="unsigned long")
						{
							if(vecn=="long long")
							{
								vecn = "long";
							}
							else if(vecn=="unsigned long long")
							{
								vecn = "unsignedlong";
							}
							else if(vecn.find("unsigned")==0)
							{
								StringUtil::replaceAll(vecn, " ", "");
							}
							else if(vecn=="bool")
							{
								vecn = "boolean";
							}
							ws_funcs.append("for(int i=0;i<_retval.size();i++)");
							ws_funcs.append("_retStr += \"<\" + _req.getNameSpc() + \":"+pars["RETURN"]+">\"+CastUtil::lexical_cast<string>(_retval.at(i))+\"</\" + _req.getNameSpc() + \":"+pars["RETURN"]+">\";\n");
						}
						else
						{
							ws_funcs.append("for(int i=0;i<_retval.size();i++)");
							ws_funcs.append("_retStr += \"<\" + _req.getNameSpc() + \":"+pars["RETURN"]+">\"+_" + appname + "getRetXmlFor"+vecn+"(_retval.at(i),_req.getNameSpc())+\"</\" + _req.getNameSpc() + \":"+pars["RETURN"]+">\";\n");
						}
					}
					else
						ws_funcs.append("_retStr += \"<\" + _req.getNameSpc() + \":"+pars["RETURN"]+">\"+_" + appname + "getRetXmlFor"+pars["RETURNTYP"]+"(_retval,_req.getNameSpc())+\"</\" + _req.getNameSpc() + \":"+pars["RETURN"]+">\";\n");
				}
				ws_funcs.append("_retStr += \"</\" + _req.getTagNameSpc() + \"Response>\";\n");
				ws_funcs.append("}catch(const Exception& e){\n");
				ws_funcs.append("return e.getMessage();\n}\n");
				ws_funcs.append("catch(...){\n");
				ws_funcs.append("return \"<soap:Fault><faultcode>soap:Server</faultcode><faultstring>Exception occurred</faultstring></soap:Fault>\";\n}\n");
				ws_funcs.append("return _retStr;\n}\n");
			}
			ws_funcs.append("}\n");
		}
		//AfcUtil::writeTofile(rtdcfp+"WsInterface.cpp",ws_funcs,true);
	}
	return ws_funcs;
}
