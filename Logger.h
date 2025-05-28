/**
 * @file Logger.h
 * @brief 日志类
 * @author Mounmory (237628106@qq.com) https://github.com/Mounmory
 * @date 
 *
 * 
 */

#ifndef MMR_UTIL_LOGGER_H
#define MMR_UTIL_LOGGER_H
#include "Common_def.h"
#include "Singleton.hpp"

#include <mutex>
#include <condition_variable>
#include <atomic>
#include <memory>
#include <fstream>
#include <string.h>
#include <thread>

BEGINE_NAMESPACE(mmrUtil)

enum class emLogLevel
{
	Log_Off = -1,
	Log_Forece = 0,
	Log_Fatal,
	Log_Error,
	Log_Warn,
	Log_Info,
	Log_Debug
};

class Logger
{
	friend class mmrComm::Singleton<mmrUtil::Logger>;
	//文件数量，文件大小（M），异步日志
	Logger(uint32_t ulFileNum = 10, uint64_t ullFileSize = 16, bool bAsyn = true);

	~Logger();
public:
	

	const uint32_t getFileMaxNum() const { return m_fileNum; }
	const uint64_t getFileMaxSize()const { return m_fileSize; }
	const emLogLevel getLogLevel() const { return m_LogLevel; }

	void logForce(const char *format, ...);
	void logFatal(const char *format, ...);
	void logError(const char *format, ...);
	void logWarn(const char *format, ...);
	void logInfo(const char *format, ...);
	void logDebug(const char *format, ...);

	void logWrite(const char *format, ...);
private:
	bool init(const std::string& strPath, const std::string& strName);

	bool start();

	void stop();

	void dealThread();

	void updateBufWrite();

	void fileSizeCheck();//检查文件大小
private:
	emLogLevel m_LogLevel;
	uint32_t m_fileNum;
	uint64_t m_fileSize;
	bool m_bAsynLog;//是否为异步日志
	std::fstream m_logStream;   //写文件流,后续考虑对比C标准库中FILE文件接口

	std::mutex	m_mutWrite;  //进行客户端句柄存储修改时，线程锁
	std::condition_variable m_cv;
	std::unique_ptr<std::thread> m_threadDeal;
	std::atomic_bool m_bRunning;

	std::tm m_lastTime;//上一次日志时间
	char m_szLastTime[32];//上一次时间字符串

	struct DataImp;
	std::unique_ptr<DataImp> m_data;
};


END_NAMESPACE(mmrUtil)

#define logInstancePtr mmrComm::Singleton<mmrUtil::Logger>::getInstance()

#define LOG_FORCE(format, ...) \
   logInstancePtr->logForce("[%ld][A][%s][%d]" format,Thread_ID, __FUNCTION__,__LINE__, ##__VA_ARGS__)

#define LOG_FORCE_PRINT(format, ...) \
	logInstancePtr->logForce("[%ld][A][%s][%d]" format,Thread_ID, __FUNCTION__,__LINE__, ##__VA_ARGS__);\
   printf("[%ld][A][%s][%d]" format "\n",Thread_ID, __FUNCTION__,__LINE__, ##__VA_ARGS__)

#define LOG_FATAL(format, ...) \
   logInstancePtr->logFatal("[%ld][F][%s][%d]" format,Thread_ID, __FUNCTION__,__LINE__, ##__VA_ARGS__)

#define LOG_FATAL_PRINT(format, ...) \
	logInstancePtr->logFatal("[%ld][F][%s][%d]" format,Thread_ID, __FUNCTION__,__LINE__, ##__VA_ARGS__);\
   printf("[%ld][F][%s][%d]" format "\n",Thread_ID, __FUNCTION__,__LINE__, ##__VA_ARGS__)

#define LOG_ERROR(format, ...) \
   logInstancePtr->logError("[%ld][E][%s][%d]" format,Thread_ID, __FUNCTION__,__LINE__, ##__VA_ARGS__)

#define LOG_ERROR_PRINT(format, ...) \
	logInstancePtr->logError("[%ld][E][%s][%d]" format,Thread_ID, __FUNCTION__,__LINE__, ##__VA_ARGS__);\
   printf("[%ld][E][%s][%d]" format "\n",Thread_ID, __FUNCTION__,__LINE__, ##__VA_ARGS__)

#define LOG_WARN(format, ...) \
   logInstancePtr->logWarn("[%ld][W][%s][%d]" format,Thread_ID, __FUNCTION__,__LINE__, ##__VA_ARGS__)

#define LOG_WARN_PRINT(format, ...) \
	logInstancePtr->logWarn("[%ld][W][%s][%d]" format,Thread_ID, __FUNCTION__,__LINE__, ##__VA_ARGS__);\
   printf("[%ld][W][%s][%d]" format "\n",Thread_ID, __FUNCTION__,__LINE__, ##__VA_ARGS__)

#define LOG_INFO(format, ...) \
   logInstancePtr->logInfo("[%ld][I][%s][%d]" format,Thread_ID, __FUNCTION__,__LINE__, ##__VA_ARGS__)

#define LOG_DEBUG(format, ...) \
   logInstancePtr->logDebug("[%ld][D][%s][%d]" format,Thread_ID, __FUNCTION__,__LINE__, ##__VA_ARGS__)

#endif
