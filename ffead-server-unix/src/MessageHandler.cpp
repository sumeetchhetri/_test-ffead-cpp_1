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
 * MessageHandler.cpp
 *
 *  Created on: Sep 27, 2009
 *      Author: sumeet
 */

#include "MessageHandler.h"

using namespace std;
MessageHandler::MessageHandler(string path)
{
	logger = Logger::getLogger("MessageHandler");
	this->path = path;
}
MessageHandler* _mess_instance = NULL;

Message MessageHandler::readMessageFromQ(string fileName, bool erase)
{
	ifstream file;
	ifstream::pos_type fileSize;
	char *fileContents, *remcontents;
	file.open(fileName.c_str(), ios::in | ios::binary | ios::ate);
	if (file.is_open())
	{
		fileContents = new char[4];
		file.seekg(0, ios::beg);
		if(!file.read(fileContents, 4))
		{
			_mess_instance->logger << "fail to read" << endl;
		}
		int len = AMEFResources::charArrayToInt(fileContents);
		fileContents = new char[len];
		file.seekg(4, ios::beg);
		if(!file.read(fileContents, len))
		{
			_mess_instance->logger << "fail to read" << endl;
		}
		if(erase)
		{
			fileSize = (int)file.tellg() - len - 4;
			remcontents = new char[fileSize];
			file.seekg(4+len, ios::beg);
			if(!file.read(remcontents, fileSize))
			{
				_mess_instance->logger << "fail to read" << endl;
			}
		}
		file.close();
	}
	string f(fileContents);
	AMEFDecoder dec;
	AMEFObject* obj = dec.decodeB(f, false, true);
	Message m;
	m.setTimestamp(obj->getPackets().at(0)->getValue());
	m.setType(obj->getPackets().at(1)->getValue());
	m.setPriority(obj->getPackets().at(2)->getValue());
	m.setUserId(obj->getPackets().at(3)->getValue());
	m.setEncoding(obj->getPackets().at(4)->getValue());
	m.setBody(obj->getPackets().at(5)->getValue());
	m.getDestination().setName(obj->getPackets().at(5)->getValue());
	m.getDestination().setType(obj->getPackets().at(5)->getValue());
	delete[] fileContents;
	if(erase)
	{
		ofstream myfile;
		myfile.open(fileName.c_str(), ios::binary | ios::trunc);
		myfile << remcontents;
		myfile.close();
		delete[] remcontents;
	}
	return m;
}

void MessageHandler::writeMessageToQ(Message msg,string fileName)
{
	AMEFEncoder enc;
	AMEFObject ob;
	ob.addPacket(msg.getTimestamp());
	ob.addPacket(msg.getType());
	ob.addPacket(msg.getPriority());
	ob.addPacket(msg.getUserId());
	ob.addPacket(msg.getEncoding());
	ob.addPacket(msg.getBody());
	ob.addPacket(msg.getDestination().getName());
	ob.addPacket(msg.getDestination().getType());
	ofstream myfile;
	myfile.open(fileName.c_str(), ios::binary | ios::app);
	myfile << enc.encodeB(&ob, true);
	myfile.close();
}

bool MessageHandler::tempUnSubscribe(string subs,string fileName)
{
	string subscribers;
	ifstream myfile1;
	myfile1.open(fileName.c_str());
	if (myfile1.is_open())
	{
		while(getline(myfile1,subscribers))
		{
			_mess_instance->logger << subscribers << flush;
			break;
		}
	}
	myfile1.close();
	ofstream myfile;
	myfile.open(fileName.c_str());
	string rep = subs + ":";
	StringUtil::replaceFirst(subscribers,rep,"");
	myfile.write(subscribers.c_str(),subscribers.length());
	myfile.close();
	if(subscribers.find(":")!=string::npos)
		return false;
	else
		return true;
}

Message MessageHandler::readMessageFromT(string fileName,string subs)
{
	bool done = tempUnSubscribe(subs,fileName+":SubslistTemp");
	Message msg = readMessageFromQ(fileName, done);
	return msg;
}

void MessageHandler::writeMessageToT(Message msg,string fileName)
{
	writeMessageToQ(msg ,fileName);
}

void MessageHandler::subscribe(string subs,string fileName)
{
	ifstream myfile1;
	myfile1.open(fileName.c_str());
	string subscribers;
	if (myfile1.is_open())
	{
		while(getline(myfile1,subscribers))
		{
			_mess_instance->logger << subscribers << flush;
			break;
		}
	}
	myfile1.close();
	ofstream myfile;
	myfile.open(fileName.c_str());
	subscribers += (subs + ":");
	myfile.write(subscribers.c_str(),subscribers.length());
	myfile.close();
	fileName += "Temp";
	myfile.open(fileName.c_str());
	myfile.write(subscribers.c_str(),subscribers.length());
	myfile.close();
}

void MessageHandler::unSubscribe(string subs,string fileName)
{
	string subscribers;
	ifstream myfile1;
	myfile1.open(fileName.c_str());
	if (myfile1.is_open())
	{
		while(getline(myfile1,subscribers))
		{
			_mess_instance->logger << subscribers << flush;
			break;
		}
	}
	myfile1.close();
	ofstream myfile;
	myfile.open(fileName.c_str());
	string rep = subs + ":";
	StringUtil::replaceFirst(subscribers,rep,"");
	myfile.write(subscribers.c_str(),subscribers.length());
	myfile.close();
	fileName += "Temp";
	myfile.open(fileName.c_str());
	myfile.write(subscribers.c_str(),subscribers.length());
	myfile.close();
}

void* MessageHandler::service(void* arg)
{
	int fd = *(int*)arg;
	char buf[MAXBUFLEN];
	string results;
	//int bytes = recv(fd, buf, sizeof buf, 0);
	_mess_instance->getServer()->Receive(fd,results,1024);
	//string temp,results;
	/*stringstream ss;
	ss << buf;
	while(getline(ss,temp))
	{
		_mess_instance->logger << temp << flush;
		results.append(temp);
	}*/
	results = results.substr(0,results.find_last_of(">")+1);
	_mess_instance->logger << results << flush;

	if(results.find("<")!=string::npos && results.find(">")!=string::npos)
	{
		string h = "Received Message----";
		Cont test;
		try
		{
			Message msg(results);
			string fileName = _mess_instance->path+msg.getDestination().getName()+":"+msg.getDestination().getType();
			if(msg.getDestination().getType()=="Queue")
				_mess_instance->writeMessageToQ(msg,fileName);
			else if(msg.getDestination().getType()=="Topic")
				_mess_instance->writeMessageToT(msg,fileName);
		}
		catch(Exception *e)
		{
			_mess_instance->logger << e->what() << flush;
		}
		_mess_instance->getServer()->Send(fd,h);
		//if (send(fd,&h[0] , h.length(), 0) == -1)
		//	_mess_instance->logger << "send failed" << flush;
		_mess_instance->logger << h << flush;
	}
	else if(results.find("GET FROM ")!=string::npos)
	{
		Message msg;
		if(results.find("Queue")!=string::npos)
		{
			StringUtil::replaceFirst(results,"GET FROM ",_mess_instance->path);
			msg = _mess_instance->readMessageFromQ(results, true);
		}
		else if(results.find("Topic")!=string::npos)
		{
			string subs = results.substr(results.find("-")+1);
			string te = "-" + subs;
			StringUtil::replaceFirst(results,te,"");
			StringUtil::replaceFirst(results,"GET FROM ",_mess_instance->path);
			msg = _mess_instance->readMessageFromT(results,subs);
		}
		string h;
		if(results.find("Queue")!=string::npos || results.find("Topic")!=string::npos)
		{
			h = msg.toXml();
			_mess_instance->logger << h << flush;
		}
		else
			h = "Improper Destination";
		_mess_instance->getServer()->Send(fd,h);
		//if (send(fd,&h[0] , h.length(), 0) == -1)
		//	_mess_instance->logger << "send failed" << flush;
	}
	else if(results.find("SUBSCRIBE ")!=string::npos)
	{
		int len = results.find("TO") - results.find("SUBSCRIBE ") - 11;
		string subs  = results.substr(results.find("SUBSCRIBE ")+10,len);
		results = results.substr(results.find("TO ")+3);
		results = (_mess_instance->path+results+":Subslist");
		_mess_instance->subscribe(subs,results);
		string h = "Subscribed";
		_mess_instance->getServer()->Send(fd,h);
		//if (send(fd,&h[0] , h.length(), 0) == -1)
		//	_mess_instance->logger << "send failed" << flush;
	}
	else if(results.find("UNSUBSCRIBE ")!=string::npos)
	{
		int len = results.find("TO") - results.find("UNSUBSCRIBE ") - 12;
		string subs  = results.substr(results.find("UNSUBSCRIBE ")+11,len);
		results = results.substr(results.find("TO ")+3);
		results = (_mess_instance->path+results+":Subslist");
		_mess_instance->subscribe(subs,results);
		string h = "Unsubscribed";
		_mess_instance->getServer()->Send(fd,h);
		//if (send(fd,&h[0] , h.length(), 0) == -1)
		//	_mess_instance->logger << "send failed" << flush;
	}
	memset(&buf[0], 0, sizeof(buf));
	close(fd);
	return NULL;
}


void MessageHandler::init(string path)
{
	if(_mess_instance==NULL)
	{
		_mess_instance = new MessageHandler(path);
		_mess_instance->running = false;
	}
}

void MessageHandler::trigger(string port,string path)
{
	init(path);
	if(_mess_instance->running)
		return;
	_mess_instance->setServer(new Server(port,false,500,&service,Constants::SYS_FORK_AVAIL));
	_mess_instance->running = true;
	return;
}

