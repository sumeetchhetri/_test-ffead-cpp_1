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
 * CHServer.cpp
 *
 *  Created on: Aug 25, 2009
 *      Author: sumeet
 */

#include "CHServer.h"


CHServer::CHServer()
{}

CHServer::~CHServer() {
	// TODO Auto-generated destructor stub
}
SharedData* SharedData::shared_instance = NULL;
string servd;
static bool isSSLEnabled,isThreadprq,processforcekilled,processgendone,sessatserv,isCompileEnabled;
static long sessionTimeout;
static int thrdpsiz/*,shmid*/;
static SSL_CTX *ctx;
static char *ciphers=0;
map<int,pid_t> pds;
static pid_t parid;
void *dlib = NULL;
typedef map<string,string> sessionMap;
static boost::mutex m_mutex,p_mutex;
ConfigurationData configurationData;
SSLHandler sSLHandler;

void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int send_connection(int fd,int unix_socket_fd)
{
	//struct sockaddr_un unix_socket_name = {0};
	struct msghdr msg;
	char ccmsg[CMSG_SPACE(sizeof(int))];
	struct cmsghdr *cmsg;
	struct iovec vec;  /* stupidity: must send/receive at least one byte */
	char *str = (char *)"x";
	int rv;

	//msg.msg_name = (struct sockaddr*)&unix_socket_name;
	//msg.msg_namelen = sizeof(unix_socket_name);
	msg.msg_name = 0;
	msg.msg_namelen = 0;
	vec.iov_base = str;
	vec.iov_len = 1;
	msg.msg_iov = &vec;
	msg.msg_iovlen = 1;

	/* old BSD implementations should use msg_accrights instead of
	* msg_control; the interface is different. */
	msg.msg_control = ccmsg;
	msg.msg_controllen = sizeof(ccmsg);
	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	cmsg->cmsg_len = CMSG_LEN(sizeof(int));
	*(int*)CMSG_DATA(cmsg) = fd;
	msg.msg_controllen = cmsg->cmsg_len;

	msg.msg_flags = 0;
	//boost::mutex::scoped_lock lock(p_mutex);
	if((rv= sendmsg(unix_socket_fd, &msg, 0)) < 0 )
	{
	  perror("sendmsg()");
	  exit(1);
	}
	close(fd);
	return rv;
}

int receive_fd(int fd)
{
  struct msghdr msg;
  struct iovec iov;
  char buf[1];
  int rv;
  int connfd = -1;
  char ccmsg[CMSG_SPACE(sizeof(connfd))];
  struct cmsghdr *cmsg;

  iov.iov_base = buf;
  iov.iov_len = 1;

  msg.msg_name = 0;
  msg.msg_namelen = 0;
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  /* old BSD implementations should use msg_accrights instead of
   * msg_control; the interface is different. */
  msg.msg_control = ccmsg;
  msg.msg_controllen = sizeof(ccmsg); /* ? seems to work... */

  rv = recvmsg(fd, &msg, 0);
  if (rv == -1) {
    perror("recvmsg");
    return -1;
  }

  cmsg = CMSG_FIRSTHDR(&msg);
  if (!cmsg->cmsg_type == SCM_RIGHTS) {
    fprintf(stderr, "got control message of unknown type %d\n",
	    cmsg->cmsg_type);
    return -1;
  }
  return *(int*)CMSG_DATA(cmsg);
}

void signalSIGSEGV(int dummy)
{
	signal(SIGSEGV,signalSIGSEGV);
	string filename;
	stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());
	void * array[25];
	int nSize = backtrace(array, 25);
	char ** symbols = backtrace_symbols(array, nSize);
	string tempo;
	for (int i = 0; i < nSize; i++)
	{
		tempo = symbols[i];
		tempo += "\n";
	}
	free(symbols);
	cout << "Segmentation fault occurred for process" << getpid() << "\n" << tempo << flush;
	abort();
}
void signalSIGCHLD(int dummy)
{
	signal(SIGCHLD,signalSIGCHLD);
	string filename;
	stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());
	void * array[25];
	int nSize = backtrace(array, 25);
	char ** symbols = backtrace_symbols(array, nSize);
	string tempo;
	for (int i = 0; i < nSize; i++)
	{
		tempo = symbols[i];
		tempo += "\n";
	}
	free(symbols);
	cout << "Child process got killed " << getpid() << "\n" << tempo << flush;
	abort();
}
void signalSIGABRT(int dummy)
{
	signal(SIGABRT,signalSIGABRT);
	string filename;
	stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());
	void * array[25];
	int nSize = backtrace(array, 25);
	char ** symbols = backtrace_symbols(array, nSize);
	string tempo;
	for (int i = 0; i < nSize; i++)
	{
		tempo = symbols[i];
		tempo += "\n";
	}
	free(symbols);
	cout << "Abort signal occurred for process" << getpid() << "\n" << tempo << flush;
	abort();
}
void signalSIGTERM(int dummy)
{
	signal(SIGKILL,signalSIGTERM);
	string filename;
	stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());
	void * array[25];
	int nSize = backtrace(array, 25);
	char ** symbols = backtrace_symbols(array, nSize);
	string tempo;
	for (int i = 0; i < nSize; i++)
	{
		tempo = symbols[i];
		tempo += "\n";
	}
	free(symbols);
	cout << "Termination signal occurred for process" << getpid() << "\n" << tempo << flush;
	abort();
}

void signalSIGKILL(int dummy)
{
	signal(SIGKILL,signalSIGKILL);
	string filename;
	stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());
	void * array[25];
	int nSize = backtrace(array, 25);
	char ** symbols = backtrace_symbols(array, nSize);
	string tempo;
	for (int i = 0; i < nSize; i++)
	{
		tempo = symbols[i];
		tempo += "\n";
	}
	free(symbols);
	cout << "Kill signal occurred for process" << getpid() << "\n" << tempo << flush;
	abort();
}

void signalSIGINT(int dummy)
{
	signal(SIGINT,signalSIGINT);
	string filename;
	stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());
	void * array[25];
	int nSize = backtrace(array, 25);
	char ** symbols = backtrace_symbols(array, nSize);
	string tempo;
	for (int i = 0; i < nSize; i++)
	{
		tempo = symbols[i];
		tempo += "\n";
	}
	free(symbols);
	cout << "Interrupt signal occurred for process" << getpid() << "\n" << tempo << flush;
	abort();
}

void signalSIGFPE(int dummy)
{
	signal(SIGFPE,signalSIGFPE);
	string filename;
	stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());
	void * array[25];
	int nSize = backtrace(array, 25);
	char ** symbols = backtrace_symbols(array, nSize);
	string tempo;
	for (int i = 0; i < nSize; i++)
	{
		tempo = symbols[i];
		tempo += "\n";
	}
	free(symbols);
	cout << "Floating point Exception occurred for process" << getpid() << "\n" << tempo << flush;
	abort();
}
void signalSIGPIPE(int dummy)
{
	signal(SIGPIPE,signalSIGPIPE);
	/*string filename;
	stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());*/
	void * array[25];
	int nSize = backtrace(array, 25);
	char ** symbols = backtrace_symbols(array, nSize);
	string tempo;
	for (int i = 0; i < nSize; i++)
	{
		tempo = symbols[i];
		tempo += "\n";
	}
	free(symbols);
	cout << "Broken pipe ignore it" << getpid() << "\n" << tempo << flush;
	//abort();
}

void signalSIGILL(int dummy)
{
	signal(SIGILL,signalSIGILL);
	string filename;
	stringstream ss;
	ss << servd;
	ss << getpid();
	ss >> filename;
	filename.append(".cntrl");
	remove(filename.c_str());
	void * array[25];
	int nSize = backtrace(array, 25);
	char ** symbols = backtrace_symbols(array, nSize);
	string tempo;
	for (int i = 0; i < nSize; i++)
	{
		tempo = symbols[i];
		tempo += "\n";
	}
	free(symbols);
	cout << "Floating point Exception occurred for process" << getpid() << "\n" << tempo << flush;
	abort();
}

/*void cleanUpRoutine(string tempo)
{
	cout << "Floating point Exception occurred for process" << getpid() << "\n" << tempo << flush;
	abort();
}*/
void service(int fd,string serverRootDirectory,map<string,string> *params,
		bool isSSLEnabled, SSL_CTX *ctx, SSLHandler sslHandler, ConfigurationData configData, void* dlib)
{
	cout << "service method " << endl;
	ServiceTask *task = new ServiceTask(fd,serverRootDirectory,params,
			isSSLEnabled, ctx, sslHandler, configurationData, dlib);
	task->run();
	delete task;
	//cout << "\nDestroyed task" << flush;
}

pid_t createChildProcess(string serverRootDirectory,int sp[],int sockfd)
{
	pid_t pid;
	struct epoll_event ev;
	if (socketpair(AF_UNIX, SOCK_DGRAM, 0, sp) == -1)
	{
		perror("socketpair");
		exit(1);
	}
	if((pid=fork())==0)
	{
		dlib = dlopen(Constants::INTER_LIB_FILE.c_str(), RTLD_NOW);
		cout << endl <<dlib << endl;
		if(dlib==NULL)
		{
			cout << dlerror() << endl;
			Logger::info("Could not load Library");
		}
		else
			Logger::info("Library loaded successfully");
		if(isSSLEnabled)
		{
			/*HTTPS related*/
			//client_auth=CLIENT_AUTH_REQUIRE;
			/* Build our SSL context*/
			ctx = sSLHandler.initialize_ctx((char*)configurationData.key_file.c_str(),(char*)configurationData.sec_password.c_str(),
					configurationData.ca_list);
			sSLHandler.load_dh_params(ctx,(char*)configurationData.dh_file.c_str());

			SSL_CTX_set_session_id_context(ctx,
			  (const unsigned char*)&SSLHandler::s_server_session_id_context,
			  sizeof SSLHandler::s_server_session_id_context);

			/* Set our cipher list */
			if(ciphers){
			  SSL_CTX_set_cipher_list(ctx,ciphers);
			}
			if(configurationData.client_auth==2)
				SSL_CTX_set_verify(ctx,SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT,0);
			else
				SSL_CTX_set_verify(ctx,SSL_VERIFY_PEER,0);
		}
		servd = serverRootDirectory;
		string filename;
		stringstream ss;
		ss << serverRootDirectory;
		ss << getpid();
		ss >> filename;
		filename.append(".cntrl");
		cout << "generated file " << filename << flush;
		ofstream cntrlfile;
		cntrlfile.open(filename.c_str());
		cntrlfile << "Process Running" << flush;
		cntrlfile.close();
		struct msghdr msg;
		struct iovec iov;
		char buf[1];
		int rv;
		int connfd = -1;
		char ccmsg[CMSG_SPACE(sizeof(connfd))];
		struct cmsghdr *cmsg;

		iov.iov_base = buf;
		iov.iov_len = 1;

		msg.msg_name = 0;
		msg.msg_namelen = 0;
		msg.msg_iov = &iov;
		msg.msg_iovlen = 1;
		/* old BSD implementations should use msg_accrights instead of
		* msg_control; the interface is different. */
		msg.msg_control = ccmsg;
		msg.msg_controllen = sizeof(ccmsg); /* ? seems to work... */
		close(sockfd);
		struct epoll_event events[1];
		//printf("parent side--%d child side---%d child pid--%ld\n",sp[j][0],sp[j][1],(long)getpid());
		int epoll_handle = epoll_create(1);
		ev.events = EPOLLIN | EPOLLPRI;
		ev.data.fd = sp[1];
		if (epoll_ctl(epoll_handle, EPOLL_CTL_ADD, sp[1], &ev) < 0)
		{
			fprintf(stderr, "epoll set insertion error: fd=%d\n", sp[1]);
			return -1;
		}
		ThreadPool pool;
		if(!isThreadprq)
		{
			pool.init(thrdpsiz,30,true);
		}
		PropFileReader pread;
		propMap params = pread.getProperties(serverRootDirectory+"resources/security.prop");

		cout << params.size() <<endl;
		while(1)
		{
			int nfds = epoll_wait(epoll_handle, events, 1,-1);
			if (nfds == -1)
			{
				perror("epoll_wait child process");
				cout << "\n----------epoll_wait child process----" << flush;
				//break;
			}
			else
			{
				//int fd = receive_fd(sp[j][1]);
				rv = recvmsg(sp[1], &msg, 0);
				if (rv == -1)
				{
					perror("recvmsg");
					cout << "\n----------error occurred----" << flush;
					exit(1);
				}

				cmsg = CMSG_FIRSTHDR(&msg);
				if (!cmsg->cmsg_type == SCM_RIGHTS)
				{
					fprintf(stderr, "got control message of unknown type %d\n",cmsg->cmsg_type);
					exit(1);
				}
				int fd = *(int*)CMSG_DATA(cmsg);
				fcntl(fd, F_SETFL,O_SYNC);

				char buf[10];
				int err;
				if((err=recv(fd,buf,10,MSG_PEEK))==0)
				{
					close(fd);
					cout << "\nsocket conn closed before being serviced" << flush;
					continue;
				}

				if(isThreadprq)
					boost::thread m_thread(boost::bind(&service,fd,serverRootDirectory,&params,
							isSSLEnabled, ctx, sSLHandler, configurationData, dlib));
				else
				{
					ServiceTask *task = new ServiceTask(fd,serverRootDirectory,&params,
							isSSLEnabled, ctx, sSLHandler, configurationData, dlib);
					pool.execute(*task);
				}
			}
		}
	}
	return pid;
}



/*pid_t createChildMonitProcess(int sp[])
{
	pid_t pid;
	if (socketpair(AF_UNIX, SOCK_DGRAM, 0, sp) == -1)
	{
		perror("socketpair");
		exit(1);
	}
	if((pid=fork())==0)
	{
		map<string,bool> stat;
		while(1)
		{
			char buf[10];
			if(read(sp[1], buf, sizeof buf) < 0)
			{
				string temp = buf;
				strVec tempv;
				boost::iter_split(tempv, temp, boost::first_finder(":"));
				if(tempv.size()==2)
				{
					if(tempv.at(0)=="R")
					{
						string h = "0";
						if(stat[tempv.at(1)])
							h = "1";
						write(sp[0], h.c_str() , sizeof(h));
					}
					else if(tempv.at(0)=="W")
					{
						stat[tempv.at(1)] = false;
					}
				}
			}
		}
	}
	return pid;
}*/


void dynamic_page_monitor(string serverRootDirectory)
{
	struct stat statbuf;
	strVec dcpsss = configurationData.dcpsss;
	map<string,long> statsinf;
	for(int i=0;i<(int)dcpsss.size();i++)
	{
		stat(dcpsss.at(i).c_str(), &statbuf);
		time_t tm = statbuf.st_mtime;
		long tim = (uintmax_t)tm;
		statsinf[dcpsss.at(i)] = tim;
	}
	while(true)
	{
		boost::this_thread::sleep(boost::posix_time::seconds(5));
		bool flag = false;
		if(processgendone)
			continue;
		for(int i=0;i<(int)dcpsss.size();i++)
		{
			stat(dcpsss.at(i).c_str(), &statbuf);
			time_t tm = statbuf.st_mtime;
			long tim = (uintmax_t)tm;
			if(tim!=statsinf[dcpsss.at(i)])
			{
				string rtdcfpath = serverRootDirectory + "rtdcf/";
				string respath = serverRootDirectory + "resources/";
				string ret = DCPGenerator::generateDCPAll(dcpsss);
				AfcUtil::writeTofile(rtdcfpath+"DCPInterface.cpp",ret,true);
				string compres = respath+"rundyn.sh";
				int i=system(compres.c_str());
				if(!i)
				{
					cout << "regenarting intermediate code-----Done" << endl;
					Logger::info("Done generating intermediate code");
				}
				m_mutex.lock();
				map<int,pid_t>::iterator it;
				for(it=pds.begin();it!=pds.end();it++)
				{
					kill(it->second,9);
				}
				m_mutex.unlock();
				processforcekilled = true;
				flag = true;
				break;
			}
		}
		if(flag)
		{
			for(int ii=0;ii<(int)dcpsss.size();ii++)
			{
				stat(dcpsss.at(ii).c_str(), &statbuf);
				time_t tm = statbuf.st_mtime;
				long tim = (uintmax_t)tm;
				statsinf[dcpsss.at(ii)] = tim;
			}
		}
	}
}


int main(int argc, char* argv[])
{
	parid = getpid();
	signal(SIGSEGV,signalSIGSEGV);
	signal(SIGFPE,signalSIGFPE);

	//signal(SIGILL,signalSIGILL);
	signal(SIGPIPE,signalSIGPIPE);
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;


    struct epoll_event ev;
	//struct rlimit rt;
    int yes=1;
    //char s[INET6_ADDRSTRLEN];
    int rv,nfds,preForked=5;

    /*char path[1024];
	pwd(path,sizeof path);
	string serverRootDirectory;
	stringstream sf;
	sf << path;
	sf >> serverRootDirectory;*/
    string serverRootDirectory = argv[1];

	serverRootDirectory += "/";//serverRootDirectory = "/home/sumeet/server/";
	string incpath = serverRootDirectory + "include/";
	string rtdcfpath = serverRootDirectory + "rtdcf/";
	string pubpath = serverRootDirectory + "public/";
	string respath = serverRootDirectory + "resources/";
	string webpath = serverRootDirectory + "web/";
	string resourcePath = respath;

	servd = serverRootDirectory;
	//string logf = serverRootDirectory+"/server.log";
	//logfile.open(logf.c_str());
	string logp = respath+"/log.prop";
	Logger::init(logp);

    PropFileReader pread;
    propMap srprps = pread.getProperties(respath+"server.prop");
    if(srprps["NUM_PROC"]!="")
    	preForked = boost::lexical_cast<int>(srprps["NUM_PROC"]);
    string sslEnabled = srprps["SSL_ENAB"];
   	if(sslEnabled=="true" || sslEnabled=="TRUE")
   		isSSLEnabled = true;
   	string thrdpreq = srprps["THRD_PREQ"];
   	if(thrdpreq=="true" || thrdpreq=="TRUE")
   		isThreadprq = true;
   	string compileEnabled = srprps["DEV_MODE"];
	if(compileEnabled=="true" || compileEnabled=="TRUE")
		isCompileEnabled = true;
   	else
   	{
   		thrdpreq = srprps["THRD_PSIZ"];
   		if(thrdpreq=="")
   			thrdpsiz = 30;
   		else
   		{
   			try
   			{
   				thrdpsiz = boost::lexical_cast<int>(thrdpreq);
   			}
   			catch(...)
   			{
   				cout << "\nInvalid thread pool size defined" <<flush;
   				thrdpsiz = 30;
   			}
   		}
   	}
   	if(srprps["SESS_STATE"]=="server")
   		sessatserv = true;
   	if(srprps["SESS_TIME_OUT"]!="")
   	{
   		try {
   			sessionTimeout = boost::lexical_cast<long>(srprps["SESS_TIME_OUT"]);
		} catch (...) {
			sessionTimeout = 3600;
			cout << "\nInvalid session timeout value defined, defaulting to 1hour/3600sec";
		}
   	}
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
    string PORT = srprps["PORT_NO"];
    string IP_ADDRES = srprps["IP_ADDR"];
    string IP_ADDRESS;
    const char *ip_addr = NULL;
	if(IP_ADDRES!="")
		ip_addr = IP_ADDRES.c_str();
    if ((rv = getaddrinfo(ip_addr, &PORT[0], &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    if(IP_ADDRES!="")
    	IP_ADDRESS = IP_ADDRES + ":" + PORT;
    else
    	IP_ADDRESS = "localhost:" + PORT;
    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
            perror("server: bind");
            continue;
        }
        break;
    }

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }
    freeaddrinfo(servinfo); // all done with this structure
    if (listen(sockfd, BACKLOGM) == -1) {
        perror("listen");
        exit(1);
    }
    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    strVec webdirs,webdirs1,pubfiles;
    ConfigurationHandler::listi(webpath,"/",true,webdirs);
    ConfigurationHandler::listi(webpath,"/",false,webdirs1);
    ConfigurationHandler::listi(pubpath,".js",false,pubfiles);


    strVec cmpnames;
    try
    {
    	//cmpnames = temporaray(webdirs,webdirs1,incpath,rtdcfpath,pubpath,respath);
    	configurationData = ConfigurationHandler::handle(webdirs, webdirs1, incpath, rtdcfpath, pubpath, respath, isSSLEnabled);
    }
    catch(XmlParseException *p)
    {
    	cout << p->getMessage() << endl;
    }
    configurationData.sessionTimeout = sessionTimeout;
    configurationData.ip_address = IP_ADDRESS;
    configurationData.sessatserv = sessatserv;
    for(unsigned int var=0;var<pubfiles.size();var++)
	{
    	configurationData.pubMap[pubfiles.at(var)] = "true";
	}
    bool libpresent = true;
    void *dlibtemp = dlopen(Constants::INTER_LIB_FILE.c_str(), RTLD_NOW);
	cout << endl <<dlibtemp << endl;
	if(dlibtemp==NULL)
	{
		libpresent = false;
		cout << dlerror() << endl;
		Logger::info("Could not load Library");
	}
	else
		dlclose(dlibtemp);
    if(isCompileEnabled)
    	libpresent = false;

    configurationData.props = pread.getProperties(respath+"mime-types.prop");
    configurationData.lprops = pread.getProperties(respath+"locale.prop");
	string compres = respath+"run.sh";
	if(!libpresent)
	{
		int i=system(compres.c_str());
		if(!i)
		{
			cout << "Done" << flush;
			Logger::info("Done generating intermediate code");
			//logfile << "Done generating intermediate code\n" << flush;
		}
	}

	for (unsigned int var1 = 0;var1<configurationData.cmpnames.size();var1++)
	{
		string name = configurationData.cmpnames.at(var1);
		boost::replace_first(name,"Component_","");
		ComponentHandler::registerComponent(name);
		AppContext::registerComponent(name);
	}
	if(srprps["CMP_PORT"]=="")
	{
		srprps["CMP_PORT"] = "7001";
	}
	ComponentHandler::trigger(srprps["CMP_PORT"]);
	if(srprps["MESS_PORT"]=="")
	{
		srprps["MESS_PORT"] = "7002";
	}
	MessageHandler::trigger(srprps["MESS_PORT"],resourcePath);
	if(srprps["MI_PORT"]=="")
	{
		srprps["MI_PORT"] = "7003";
	}
	MethodInvoc::trigger(srprps["MI_PORT"]);
	int sp[preForked][2]; /* the pair of socket descriptors */
	printf("server: waiting for connections...\n");
	//logfile << "Server: waiting for connections on port " << PORT << "\n" << flush;
	Logger::info("Server: waiting for connections on port "+PORT);

	vector<string> files;
	for(int j=0;j<preForked;j++)
	{
		pid_t pid = createChildProcess(serverRootDirectory,sp[j],sockfd);
		pds[j] = pid;
		stringstream ss;
		string filename;
		ss << serverRootDirectory;
		ss << pds[j];
		ss >> filename;
		filename.append(".cntrl");
		files.push_back(filename);
	}
	if(isCompileEnabled)boost::thread m_thread(boost::bind(&dynamic_page_monitor ,serverRootDirectory));
	struct epoll_event events[MAXEPOLLSIZE];
	int epoll_handle = epoll_create(MAXEPOLLSIZE);
	ev.events = EPOLLIN | EPOLLPRI;
	ev.data.fd = sockfd;
	if (epoll_ctl(epoll_handle, EPOLL_CTL_ADD, sockfd, &ev) < 0)
	{
		fprintf(stderr, "epoll set insertion error: fd=%d\n", sockfd);
		return -1;
	}
	else
		printf("listener socket to join epoll success!\n");
	int childNo = 0;
	/*if(fork()==0)
	{
		//start  of hotdeployment process

	}*/
	//cout << "Done" << flush;
	struct msghdr msg;
	char ccmsg[CMSG_SPACE(sizeof(int))];
	struct cmsghdr *cmsg;
	struct iovec vec;  /* stupidity: must send/receive at least one byte */
	char *str = (char *)"x";
	//msg.msg_name = (struct sockaddr*)&unix_socket_name;
	//msg.msg_namelen = sizeof(unix_socket_name);
	msg.msg_name = 0;
	msg.msg_namelen = 0;
	vec.iov_base = str;
	vec.iov_len = 1;
	msg.msg_iov = &vec;
	msg.msg_iovlen = 1;

	/* old BSD implementations should use msg_accrights instead of
	* msg_control; the interface is different. */
	msg.msg_control = ccmsg;
	msg.msg_controllen = sizeof(ccmsg);
	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	cmsg->cmsg_len = CMSG_LEN(sizeof(int));
	msg.msg_flags = 0;
	int curfds = 1;
	ifstream cntrlfile;
	ThreadPool pool;
	if(!isThreadprq)
	{
		pool.init(thrdpsiz,30,true);
	}
	while(1)
	{

		if(childNo>=preForked)
			childNo = 0;
		nfds = epoll_wait(epoll_handle, events, curfds,-1);
		if (nfds == -1)
		{
			perror("epoll_wait main process");
			//logfile << "Interruption Signal Received\n" << flush;
			Logger::info("Interruption Signal Received\n");
			curfds = 1;
			if(errno==EBADF)
				cout << "\nInavlid fd" <<flush;
			else if(errno==EFAULT)
				cout << "\nThe memory area pointed to by events is not accessible" <<flush;
			else if(errno==EINTR)
				cout << "\ncall was interrupted by a signal handler before any of the requested events occurred" <<flush;
			else
				cout << "\nnot an epoll file descriptor" <<flush;
			//break;
		}
		processgendone = false;
		if(processforcekilled)
		{
			files.clear();
			for(int j=0;j<preForked;j++)
			{
				pid_t pid = createChildProcess(serverRootDirectory,sp[j],sockfd);
				pds[j] = pid;
				stringstream ss;
				string filename;
				ss << serverRootDirectory;
				ss << pds[j];
				ss >> filename;
				filename.append(".cntrl");
				files.push_back(filename);
			}
			processforcekilled = false;
			processgendone = true;
		}
		for(int n=0;n<nfds;n++)
		{
			if(childNo>=preForked)
				childNo = 0;
			if (events[n].data.fd == sockfd)
			{
				new_fd = -1;
				sin_size = sizeof their_addr;
				new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
				//cout << "\nnew http request" <<flush;
				//logfile << "Interruption Signal Received\n" << flush;
				if (new_fd == -1)
				{
					perror("accept");
					continue;
				}
				else
				{
					curfds++;
					fcntl(new_fd, F_SETFL, fcntl(new_fd, F_GETFD, 0) | O_NONBLOCK);
					ev.events = EPOLLIN | EPOLLPRI;
					ev.data.fd = new_fd;
					if (epoll_ctl(epoll_handle, EPOLL_CTL_ADD, new_fd, &ev) < 0)
					{
						perror("epoll");
						cout << "\nerror adding to epoll cntl list" << flush;
						return -1;
					}
				}
			}
			else
			{
				epoll_ctl(epoll_handle, EPOLL_CTL_DEL, events[n].data.fd,&ev);
				curfds--;
				cntrlfile.open(files.at(childNo).c_str());
				if(cntrlfile.is_open())
				{
					*(int*)CMSG_DATA(cmsg) = events[n].data.fd;
					msg.msg_controllen = cmsg->cmsg_len;
					if((rv= sendmsg(sp[childNo][0], &msg, 0)) < 0)
					{
					  perror("sendmsg()");
					  exit(1);
					}
					string cno = boost::lexical_cast<string>(childNo);
					close(events[n].data.fd);
					childNo++;
				}
				else
				{
					int tcn = childNo;
					for(int o=0;o<preForked;o++)
					{
						cntrlfile.open(files.at(o).c_str());
						if(cntrlfile.is_open())
						{
							*(int*)CMSG_DATA(cmsg) = events[n].data.fd;
							msg.msg_controllen = cmsg->cmsg_len;
							if((rv= sendmsg(sp[o][0], &msg, 0)) < 0)
							{
							  perror("sendmsg()");
							  exit(1);
							}
							string cno = boost::lexical_cast<string>(o);
							//logfile << ("sent socket to process "+cno+"\n") << flush;
							close(events[n].data.fd);
							childNo = o+1;
							break;
						}
					}
					close(sp[tcn][0]);
					close(sp[tcn][1]);
					cout << "Process got killed" << flush;
					pid_t pid = createChildProcess(serverRootDirectory,sp[tcn],sockfd);
					pds[tcn] = pid;
					stringstream ss;
					string filename;
					ss << serverRootDirectory;
					ss << pid;
					ss >> filename;
					filename.append(".cntrl");
					files[tcn] = filename;
					cout << "created a new Process" << flush;
					//logfile << "Process got killed hence created a new Process " << pid << flush;
					Logger::info("Process got killed hence created a new Process\n");
				}
				cntrlfile.close();
			}
		}
	}
	return 0;
}

