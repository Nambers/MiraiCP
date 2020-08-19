#pragma once
#ifndef http996
	#define http996
	#include <string>
	#include <vector>
	#include <map>
	#include <memory>
	#include <fstream>
	#include <Windows.h>
	#include "wininet.h"
	#include "utils.h"
	#include "CJsonObject.hpp"
	#pragma comment(lib,"wininet.lib")
	#pragma comment(lib, "ws2_32.lib")
#if (_MSC_VER < 1900) //vs < 2019
	#ifdef _DEBUG
		#ifdef _WIN64
			#pragma comment(lib, "..\\packages\\CppRequests.0.0.5\\build\\native\\lib\x64\\v120\\static\\Debug\\HttpLib.lib")
		#else
			#pragma comment(lib, "..\\packages\\CppRequests.0.0.5\\build\\native\\lib\\Win32\\v120\\static\\Debug\\HttpLib.lib")
		#endif
	#else
		#ifdef _WIN64
			#pragma comment(lib, "..\\packages\\CppRequests.0.0.5\\build\\native\\lib\\x64\\v120\\static\\Release\\HttpLib.lib")
		#else
			#pragma comment(lib, "..\\packages\\CppRequests.0.0.5\\build\\native\\lib\\Win32\\v120\\static\\Release\\HttpLib.lib")
		#endif
	#endif
#else
	#ifdef _DEBUG
		#ifdef _WIN64
			#pragma comment(lib, "..\\packages\\CppRequests.0.0.5\\build\\native\\lib\\x64\\v160\\static\\Debug\\HttpLib.lib")
		#else
			#pragma comment(lib, "..\\packages\\CppRequests.0.0.5\\build\\native\\lib\\Win32\\v160\\static\\Debug\\HttpLib.lib")	
		#endif
	#else
		#ifdef _WIN64
			#pragma comment(lib, "..\\packages\\CppRequests.0.0.5\\build\\native\\lib\\x64\\v160\\static\\Release\\HttpLib.lib")
		#else
			#pragma comment(lib, "..\\packages\\CppRequests.0.0.5\\build\\native\\lib\\Win32\\v160\\static\\Release\\HttpLib.lib")
		#endif
	#endif
#endif
	using namespace std;
	typedef unsigned char byte;//
	namespace requests{
	
		class BinaryData
		{
		public:
			BinaryData();
			explicit BinaryData(int size);
			//BinaryData(const BinaryData &);
			~BinaryData();
			void append(byte n);
			void append(byte *buffer, int size);
			void append(std::string n);
			int  find(const char *s);
			std::string substr(int start, int end);
			void erase(int start, int end);
			const byte* raw_buffer();
			int	 size();
			std::string	to_string();
			std::string user_data;
		private:
			std::vector<byte>	data;
		};
		class Response
		{
		public:
			Response();
			Response(std::shared_ptr<BinaryData> rep);
			Response(std::string h,std::string origin_domain, std::shared_ptr<BinaryData> data);
			~Response();
			std::string	GetText();
			map<std::string,std::string>	Header();
			const  byte*GetBinary();
			unsigned int size();
			map < std::string, std::string> cookie;
			std::vector<std::string>		vec_cookie;
			std::string		req_domain;
			std::string	operator[](std::string key);
			unsigned int status;
		private:
			map<std::string,std::string>	header;
			std:: string	text;
			std::shared_ptr<BinaryData>	pContent;
			void SplitString(const string& s, vector<string>& v, const string& c);
			unsigned int	status2int(string &s);
		};

		class Request
		{
		public:
			Request(std::string url, std::string method,const map<string, string> &header =map<string, string>(),const  map<string, string> &options = map<string, string>());
			~Request();
			string	HeaderToString();
			void  SetPostHeader(BinaryData &data);
			std::string	url;
			std::string	domain;
			std::string param;
			std::string prefix;
			unsigned int timeout;
			std::string proxy;
			map<string, string>	header;
			map<std::string, std::string> default_options;
			int	port;
			std::string method;
		};
		class Session{
		public:
			Session();
			Session(std::string proxy);
			~Session();
			Response	Get(std::string url, const map<string, string> &head = map<string, string>(), std::string cookie = "", const map<string, string> &options = map<string, string>());
			Response	Post(std::string url, map<string, string> &data,const map<string, string> files = map<string, string>(), const map<string, string> &head = map<string, string>(), std::string cookie = "", const map<string, string> &options = map<string, string>());
			Response	Post(std::string url, BinaryData &data,const map<string, string> &head = map<string, string>(), const map<string, string> &options = map<string, string>());
			map<std::string, std::string> cookies;
			std::vector<std::string>	vec_cookie;
		private:
			HINTERNET	hInternet;
			HINTERNET	hConnect;
			HINTERNET	hRequest;
			std::string	inner_proxy;
		private:
			bool		InstallCookie(Response &r);
		};
	
		std::string GetIpByDomainName(const char *szHost);
		Response	Get(std::string url,const map<string, string> &head = map<string, string>(),std::string cookie="",const map<string, string> &options = map<string, string>());
		Response	Delete(std::string url, const map<string, string> &head = map<string, string>(), std::string cookie = "", const map<string, string> &options = map<string, string>());
		Response	Head(std::string url, const map<string, string> &head = map<string, string>(), std::string cookie = "", const map<string, string> &options = map<string, string>());
		Response	Options(std::string url, const map<string, string> &head = map<string, string>(), std::string cookie = "", const map<string, string> &options = map<string, string>());
		Response	Put(std::string url, const std::string filepath,const map<string, string> &head = map<string, string>(), std::string cookie = "", const map<string, string> &options = map<string, string>());
		Response	Post(std::string url, map<string, string> &data, map<string, string> files = map<string, string>(),const map<string, string> &head = map<string, string>(),std::string cookie="",const map<string, string> &options = map<string, string>());
		Response	Post(std::string url, BinaryData &bin_data,const map<string, string> &head = map<string, string>(),std::string cookie="",const map<string, string> &options = map<string, string>());
		Response	Post(std::string url, std::string str_data, const map<string, string> &head = map<string, string>(), std::string cookie = "", const map<string, string> &options = map<string, string>());
		Response	https_post(string url, BinaryData &data,const  map<string, string> &head = map<string, string>(),std::string cookie="",const map<string,string> &options = map<string, string>());
		Response	https_post(string url, map<string, string> &data, map<string, string> files,const map<string, string> &head = map<string, string>(),std::string cookie="",const map<string, string> &options = map<string, string>());
		Response	request(string method, string url, BinaryData &data,const map<string, string> &head = map<string, string>(),std::string cookie="",const map<string, string> &options = map<string, string>());
		Response	https_send(string method, string url, int port, DWORD flags, BinaryData &data,const map<string, string> &head = map<string, string>(),std::string cookie="",const map<string, string> &options = map<string, string>());
		std::string get_content_type(string &filename);
		std::string http_err2str(DWORD code);
	}
#endif