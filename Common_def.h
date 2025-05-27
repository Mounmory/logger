/**
 * @file Common_def.h
 * @brief
 * @author Mounmory (237628106@qq.com) https://github.com/Mounmory
 * @date
 *
 *
 */

#ifndef MMR_COMMON_DEF_H
#define MMR_COMMON_DEF_H

//#if defined(_WIN64) || defined(WIN32)
//#define OS_MMR_WIN
//#elif defined(linux) || defined(__linux) || defined(__linux__) || defined(__gnu_linux__)
//#define OS_MMR_LINUX
////#elif defined(unix) || defined(__unix) || defined(__unix__)
////#define MMR_UNIX
//#endif

#define BEGINE_NAMESPACE(ns) namespace ns {
#define END_NAMESPACE(ns) }

#define IN
#define OUT

//#include <chrono>
//#include <ctime>
//#include <thread>
//#include <iomanip>

#ifdef OS_MMR_WIN
	//#ifndef WIN32_LEAN_AND_MEAN//WINDOWS API用于屏蔽一些不常用的API（优化应用程序）才用的。
	//	#define WIN32_LEAN_AND_MEAN
	//#endif
	//#ifndef _CRT_NONSTDC_NO_DEPRECATE
	//	#define _CRT_NONSTDC_NO_DEPRECATE
	//#endif
	//#ifndef _CRT_SECURE_NO_WARNINGS
	//	#define _CRT_SECURE_NO_WARNINGS
	//#endif
	//#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
	//	#define _WINSOCK_DEPRECATED_NO_WARNINGS
	//#endif
#include <winsock2.h>
//#include <ws2tcpip.h>   // for inet_pton,inet_ntop
#include <windows.h>

#define DIR_SEPARATOR       '\\'
#define DIR_SEPARATOR_STR   "\\"

#define Thread_ID GetCurrentThreadId()		//获取线程ID
#define Process_ID GetCurrentProcessId()	//获取进程ID
#define libHandle HINSTANCE					//动态库句柄类型
#define STR_OS_TYPE "Windows"	

static const char* strLibExtension = ".dll";	//动态库扩展名
#else
#include <pthread.h>  
#include <unistd.h>  
#include <dirent.h>
#include <dlfcn.h>
#include <sys/syscall.h>  
#include <string>

#define DIR_SEPARATOR       '/'
#define DIR_SEPARATOR_STR   "/"

#define Thread_ID syscall(SYS_gettid)	//获取线程ID
#define Process_ID getpid()				//获取进程ID
#define libHandle void*					//动态库句柄类型
#define STR_OS_TYPE "Linux"
static const char* strLibExtension = ".so";		//动态库扩展名

static std::string _CutParenthesesNTail(std::string&& prettyFuncon)
{
	auto pos = prettyFuncon.find('(');
	if (pos != std::string::npos)
		prettyFuncon.erase(prettyFuncon.begin() + pos, prettyFuncon.end());
	pos = prettyFuncon.find(' ');
	if (pos != std::string::npos)//删除返回类型
		prettyFuncon.erase(prettyFuncon.begin(), prettyFuncon.begin() + pos + 1);
	return std::move(prettyFuncon);
}
#define __FUNCTION__ _CutParenthesesNTail(__PRETTY_FUNCTION__).c_str()//系统自带的宏不显示作用域信息
#endif

#ifndef __FILENAME__
// #define __FILENAME__  (strrchr(__FILE__, DIR_SEPARATOR) ? strrchr(__FILE__, DIR_SEPARATOR) + 1 : __FILE__)
#define __FILENAME__  (strrchr(DIR_SEPARATOR_STR __FILE__, DIR_SEPARATOR) + 1)
#endif

#ifdef NDEBUG
#define STR_BUILD_TYPE "Release"
#else
#define STR_BUILD_TYPE "Debug"
#endif

#define STD_CERROR \
std::cerr << "[" << __FUNCTION__ << "][" << __LINE__ << "]"

#endif

