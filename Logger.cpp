#include "Logger.h"

#include <stdarg.h>
#include <iomanip> //std::setfill头文件
#include <queue>
#include <iostream>
#include <string.h>
#include <string>

#ifdef OS_MMR_WIN
#include <corecrt_io.h>	//_access头文件
#include <windows.h>
#elif defined OS_MMR_LINUX
#include <sys/stat.h>  //新建文件夹头文件
#include <sys/types.h> //新建文件夹头文件
#include <unistd.h>
#endif

#define MAX_STR_LEN 1024

using namespace mmrUtil;


bool getAppPathAndName(std::string& filePath, std::string& exeName)
{
#ifdef OS_MMR_WIN
	char path[MAX_STR_LEN];
	auto pathLen = GetModuleFileName(NULL, path, MAX_STR_LEN);
	if (pathLen > MAX_STR_LEN)
	{
		STD_CERROR << "funciton mmrUtil::getAppPathAndName path len[" << pathLen << "] is longer than max string leng " << std::endl;
	}
	filePath = path;

	auto pos = filePath.rfind('.');
	if (pos != std::string::npos)
	{
		filePath.erase(filePath.begin() + pos, filePath.end());
	}
	pos = filePath.rfind('\\');
	if (pos != std::string::npos)
	{
		exeName = &filePath.c_str()[pos + 1];
		filePath.erase(filePath.begin() + pos + 1, filePath.end());
	}
	else
	{
		return false;
	}
	return true;
#else
	pid_t pid = getpid();
	char tmpPath[MAX_STR_LEN];//路径
	char tmpName[MAX_STR_LEN];//exe名称
	ssize_t len = readlink(std::string("/proc/").append(std::to_string(pid)).append("/exe").c_str(), tmpPath, MAX_STR_LEN - 1);
	if (len <= 0)
		return false;
	tmpPath[len] = '\0';
	char* path_end = strrchr(tmpPath, '/');
	if (path_end == NULL)
		return false;
	++path_end;
	strcpy(tmpName, path_end);
	*path_end = '\0';
	filePath = tmpPath;
	exeName = tmpName;
	return true;
#endif
}

class CBigBuff
{
public:
	CBigBuff() = delete;
	CBigBuff(CBigBuff&) = delete;
	CBigBuff(CBigBuff&&) = delete;

	CBigBuff(uint32_t ulLen)
		: m_buf(new char[ulLen])
		, m_ulLen(ulLen - 1)//长度-1，避免添最后一位置空越界
		, m_ulPos(0)
		, m_usTryIncrease(0)
	{
	};

	~CBigBuff() { delete[] m_buf; }

	void tryWrite(char* buf, uint16_t len)
	{
		memcpy(m_buf + m_ulPos, buf, len);
		m_usTryIncrease += len;
	}

	void doneTry() {
		m_ulPos += m_usTryIncrease;
		m_buf[m_ulPos++] = '\n';
		m_usTryIncrease = 0;
	}

	void zeroEnd()
	{
		m_buf[m_ulPos] = 0x00;
	}

	void clearTry()
	{
		m_usTryIncrease = 0;
		m_buf[m_ulPos] = 0x00;
	}

	uint32_t getTryAvailid() { return (m_ulLen - m_ulPos - m_usTryIncrease); }

	char* getTryCurrent() { return (m_buf + m_ulPos + m_usTryIncrease); }

	void addTryIncrease(uint16_t weakLen) { m_usTryIncrease += weakLen; }

	void clear() {
		m_ulPos = 0;
		m_usTryIncrease = 0;
	}

	char* getBuf() { return m_buf; }

	uint32_t getMaxLen() { return m_ulLen; }

	uint32_t getSize() { return m_ulPos; }

private:
	char* m_buf;
	uint32_t m_ulLen;//buf长度
	uint32_t m_ulPos;//当前buf位置
	uint32_t m_usTryIncrease;
};

struct Logger::DataImp 
{
	DataImp() 
		: m_lMaxStrLen(2048)
		, m_usBufEmptySize(3)
		, m_ulBigBufSize(1024 * 1024)
	{
		m_pBufWrite = std::make_unique<CBigBuff>(m_ulBigBufSize);//不要在构造函数里面分配
		for (uint16_t i = 0; i < m_usBufEmptySize; ++i)
		{
			m_queBufsEmpty.push(std::make_unique<CBigBuff>(m_ulBigBufSize));
		}
	}
	~DataImp() = default;

	uint32_t m_lMaxStrLen;//每一条日志的最大长度
	std::string m_strLogDir; //当前路径
	std::string m_strLogName; //文件路径
	std::string m_strFilePath; //输出文件全路径

	std::unique_ptr<CBigBuff> m_pBufWrite;//写
	std::unique_ptr<CBigBuff> m_pBufDeal;//写
	std::queue<std::unique_ptr<CBigBuff>> m_queBufsWrite;
	std::queue<std::unique_ptr<CBigBuff>> m_queBufsDeal;
	std::queue<std::unique_ptr<CBigBuff>> m_queBufsEmpty;
	uint16_t m_usBufEmptySize;
	uint32_t m_ulBigBufSize;
};


#define MIN_AVAILI_SIZE 1024

#define LOG_BY_LEVEL(logLevel)\
if (m_LogLevel < logLevel)\
return;\
auto now = std::chrono::system_clock::now();\
auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;\
std::time_t current_time = std::chrono::system_clock::to_time_t(now);\
std::tm* time_info = std::localtime(&current_time);\
std::unique_lock<std::mutex> guard(m_mutWrite); \
if (time_info->tm_sec != m_lastTime.tm_sec\
	|| time_info->tm_min != m_lastTime.tm_min\
	|| time_info->tm_hour != m_lastTime.tm_hour\
	|| time_info->tm_mday != m_lastTime.tm_mday\
	|| time_info->tm_mon != m_lastTime.tm_mon\
	|| time_info->tm_year != m_lastTime.tm_year)\
{\
	m_lastTime.tm_sec = time_info->tm_sec;\
	m_lastTime.tm_min = time_info->tm_min;\
	m_lastTime.tm_hour = time_info->tm_hour;\
	m_lastTime.tm_mday = time_info->tm_mday;\
	m_lastTime.tm_mon = time_info->tm_mon;\
	m_lastTime.tm_year = time_info->tm_year;\
	snprintf(m_szLastTime, sizeof(m_szLastTime), "%04d-%02d-%02d %02d:%02d:%02d",\
		m_lastTime.tm_year + 1900, m_lastTime.tm_mon + 1, m_lastTime.tm_mday,\
		m_lastTime.tm_hour, m_lastTime.tm_min, m_lastTime.tm_sec);\
}\
if (m_bAsynLog && m_data->m_pBufWrite->getTryAvailid() <= MIN_AVAILI_SIZE)/*保留1024个字节，小于这个字节就保存了*/\
{\
	updateBufWrite();\
	m_cv.notify_all();\
}\
snprintf(m_data->m_pBufWrite->getTryCurrent(), 25, "%s.%03d ", m_szLastTime, now_ms);\
m_data->m_pBufWrite->addTryIncrease(24);\
va_list arglist;\
va_start(arglist, format);\
int strLen = vsnprintf(m_data->m_pBufWrite->getTryCurrent(), m_data->m_pBufWrite->getTryAvailid(), format, arglist);\
if(strLen < 0)\
	strLen = m_data->m_pBufWrite->getTryAvailid();\
if (strLen > m_data->m_lMaxStrLen)\
	strLen =  m_data->m_lMaxStrLen;\
m_data->m_pBufWrite->addTryIncrease(strLen);\
m_data->m_pBufWrite->doneTry();\
va_end(arglist);\
if (!m_bAsynLog)\
{\
	m_logStream << m_data->m_pBufWrite->getBuf();\
	m_logStream.flush();\
	m_data->m_pBufWrite->clear();\
	fileSizeCheck();\
}

mmrUtil::Logger::Logger(uint32_t ulFileNum, uint64_t ullFileSize, bool bAsyn)
	: m_LogLevel(emLogLevel::Log_Debug)
	, m_fileNum(ulFileNum)
	, m_fileSize(ullFileSize * 1024 *1024)
	, m_bAsynLog(bAsyn)
	, m_data(std::make_unique<DataImp>())
{
	std::cout << "Logger instance construct: file number[" << m_fileNum << "] file size [" << ullFileSize << " Mb]" << std::endl;
	start();
}

mmrUtil::Logger::~Logger()
{
	stop();
	std::cout << "Logger instance destruct." << std::endl;
}

void mmrUtil::Logger::logForce(const char *format, ...)
{
	LOG_BY_LEVEL(emLogLevel::Log_Forece);
}

void mmrUtil::Logger::logFatal(const char *format, ...)
{
	LOG_BY_LEVEL(emLogLevel::Log_Fatal);
}

void mmrUtil::Logger::logError(const char *format, ...)
{
	LOG_BY_LEVEL(emLogLevel::Log_Error);
}

void mmrUtil::Logger::logWarn(const char *format, ...)
{
	LOG_BY_LEVEL(emLogLevel::Log_Warn);
}

void mmrUtil::Logger::logInfo(const char *format, ...)
{
	LOG_BY_LEVEL(emLogLevel::Log_Info);
}

void mmrUtil::Logger::logDebug(const char *format, ...)
{
	LOG_BY_LEVEL(emLogLevel::Log_Debug);
}

void mmrUtil::Logger::logWrite(const char *format, ...)
{
	auto now = std::chrono::system_clock::now();
	auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;
	std::time_t current_time = std::chrono::system_clock::to_time_t(now);
	std::tm* time_info = std::localtime(&current_time);

	std::unique_lock<std::mutex> lock(m_mutWrite);
	if (time_info->tm_sec != m_lastTime.tm_sec //由低到高比较
		|| time_info->tm_min != m_lastTime.tm_min
		|| time_info->tm_hour != m_lastTime.tm_hour
		|| time_info->tm_mday != m_lastTime.tm_mday
		|| time_info->tm_mon != m_lastTime.tm_mon
		|| time_info->tm_year != m_lastTime.tm_year)
	{
		m_lastTime.tm_sec = time_info->tm_sec;
		m_lastTime.tm_min = time_info->tm_min;
		m_lastTime.tm_hour = time_info->tm_hour;
		m_lastTime.tm_mday = time_info->tm_mday;
		m_lastTime.tm_mon = time_info->tm_mon;
		m_lastTime.tm_year = time_info->tm_year;

		snprintf(m_szLastTime, sizeof(m_szLastTime), "%04d-%02d-%02d %02d:%02d:%02d",
			m_lastTime.tm_year + 1900, m_lastTime.tm_mon + 1, m_lastTime.tm_mday,
			m_lastTime.tm_hour, m_lastTime.tm_min, m_lastTime.tm_sec);
	}


	if (m_bAsynLog && m_data->m_pBufWrite->getTryAvailid() <= MIN_AVAILI_SIZE)//保留1024个字节，小于这个字节就保存了
	{
		updateBufWrite();
		m_cv.notify_all();
	}

	//写入时间
	snprintf(m_data->m_pBufWrite->getTryCurrent(), 25, "%s.%03d ", m_szLastTime, now_ms);
	m_data->m_pBufWrite->addTryIncrease(24);

	va_list arglist;
	va_start(arglist, format);
	int strLen = vsnprintf(m_data->m_pBufWrite->getTryCurrent(), m_data->m_pBufWrite->getTryAvailid(), format, arglist);

	if (strLen < 0)//字符换太长了，格式化失败
		strLen = m_data->m_pBufWrite->getTryAvailid();

	if (strLen > m_data->m_lMaxStrLen)
		strLen = m_data->m_lMaxStrLen;
	
	m_data->m_pBufWrite->addTryIncrease(strLen);
	m_data->m_pBufWrite->doneTry();
	va_end(arglist);

	if (!m_bAsynLog)//同步日志
	{
		m_logStream.write(m_data->m_pBufWrite->getBuf(), m_data->m_pBufWrite->getSize());
		m_logStream.flush();
		m_data->m_pBufWrite->clear();
		fileSizeCheck();
	}

}

bool mmrUtil::Logger::init(const std::string& strPath, const std::string& strName)
{
	m_data->m_strLogDir = strPath;
	m_data->m_strLogName = strName;
	m_data->m_strFilePath = m_data->m_strLogDir + m_data->m_strLogName + ".log";
#ifdef OS_MMR_WIN
	if (_access(m_data->m_strLogDir.c_str(), 0) == -1)//如果文件夹不存在，则创建
	{
		if (CreateDirectory(m_data->m_strLogDir.c_str(), 0) == false)
		{
			m_LogLevel = emLogLevel::Log_Off;//创建文件夹失败，不输出日志
			STD_CERROR << "Create directory " << m_data->m_strLogDir.c_str() << "failed!" << std::endl;
			return false;
		}
	}
#elif defined OS_MMR_LINUX
	if (access(m_data->m_strLogDir.c_str(), 0) != F_OK) //检查文件夹是否存在，不存在则创建
	{
		mkdir(m_data->m_strLogDir.c_str(), S_IRWXO); //所有人都有权限读写

		if (access(m_data->m_strLogDir.c_str(), 0) != F_OK)
		{
			m_LogLevel = emLogLevel::Log_Off;//创建文件夹失败，不输出日志
			std::cerr << __LINE__ << "Create directory " << m_data->m_strLogDir.c_str() << "failed!" << std::endl;
			return false;
		}
	}
#endif
	return true;
}

bool mmrUtil::Logger::start()
{
	if (m_data->m_strLogDir.empty() || m_data->m_strLogName.empty())
	{
		std::string logDir, logName;
		getAppPathAndName(logDir, logName);
		logDir += "log/";
		if (!init(logDir, logName))
		{
			return false;
		}
	}

	if (!m_logStream.is_open())
	{
		//m_logStream.open(m_data->m_strFilePath.c_str(), std::ios::app);
		m_logStream.open(m_data->m_strFilePath.c_str(), std::ios::app | std::ios::binary);
		if (m_logStream.fail())
		{
			STD_CERROR << "open file " << m_data->m_strFilePath.c_str() << "failed!" << std::endl;
			return false;
		}
		m_logStream.seekp(0, std::ios::end);
	}

	//启动处理线程
	if (m_bAsynLog)
	{
		m_bRunning.store(true, std::memory_order_relaxed);
		m_threadDeal = std::make_unique<std::thread>(&Logger::dealThread, this);
	}

	logWrite("[%d][A][%s][%d]----------------- start -----------------", Thread_ID, __FUNCTION__, __LINE__);
	return true;
}

void mmrUtil::Logger::stop()
{
	//停掉线程
	if (true == m_bRunning)
	{
		logWrite("[%d][A][%s][%d]----------------- stop -----------------", Thread_ID, __FUNCTION__, __LINE__);
		m_bRunning.store(false, std::memory_order_relaxed);//退出线程
		m_cv.notify_all();
		if (m_threadDeal && m_threadDeal->joinable())
		{
			m_threadDeal->join();//等待线程结束
		}
	}

	if (m_logStream.is_open())
	{
		m_logStream.close();
	}
}

void mmrUtil::Logger::dealThread()
{
	while (m_bRunning.load(std::memory_order_relaxed) || m_data->m_pBufWrite->getSize() || m_data->m_queBufsWrite.size())
	{
		{
			std::unique_lock<std::mutex> lock(m_mutWrite);
			m_cv.wait_for(lock, std::chrono::milliseconds(5000));//异步日志5秒写一次
		}
		if (m_data->m_pBufWrite->getSize() > 0 || m_data->m_queBufsWrite.size() > 0)
		{
			{
				std::unique_lock<std::mutex> lock(m_mutWrite);
				updateBufWrite();
				m_data->m_queBufsDeal = std::move(m_data->m_queBufsWrite);
			}
			while (m_data->m_queBufsDeal.size())
			{
				m_data->m_pBufDeal = std::move(m_data->m_queBufsDeal.front());
				m_data->m_queBufsDeal.pop();
				//std::cout << "log write!" << std::endl;
				m_logStream.write(m_data->m_pBufDeal->getBuf(), m_data->m_pBufDeal->getSize());
				m_logStream.flush();//异步日志写入到文件
				m_data->m_pBufDeal->clear();

				fileSizeCheck();

				{
					std::unique_lock<std::mutex> lock(m_mutWrite);
					m_data->m_queBufsEmpty.push(std::move(m_data->m_pBufDeal));
				}
			}
		}
	}
}

void mmrUtil::Logger::updateBufWrite()
{
	m_data->m_pBufWrite->zeroEnd();
	m_data->m_queBufsWrite.push(std::move(m_data->m_pBufWrite));
	if (m_data->m_queBufsEmpty.size())
	{
		m_data->m_pBufWrite = std::move(m_data->m_queBufsEmpty.front());
		m_data->m_queBufsEmpty.pop();
	}
	else
	{
		//是否新增缓冲区标记
		std::cout << " warning! log buf empty queue is empty!" << std::endl;
		m_data->m_pBufWrite = std::make_unique<CBigBuff>(m_data->m_ulBigBufSize);
	}
}

void mmrUtil::Logger::fileSizeCheck()
{
	if (m_logStream.tellp() > m_fileSize)//文件大于最大文件大小
	{
		//修改文件名称
		m_logStream.close();

		std::string strOldName; //旧名称
		std::string strNewName; //新名称
		int32_t nLogNumIndex = m_fileNum;
		strNewName = m_data->m_strFilePath + "." + std::to_string(nLogNumIndex); //最大序号的日志
#ifdef OS_MMR_WIN
		if (_access(strNewName.c_str(), 0) != -1)
		{
			remove(strNewName.c_str()); //删除
		}
		while (--nLogNumIndex > 0) //减小序号，以此更改日志名称
		{
			strOldName = m_data->m_strFilePath + "." + std::to_string(nLogNumIndex);
			if (_access(strOldName.c_str(), 0) != -1) //检查文件夹是否存在，不存在则创建
			{
				rename(strOldName.c_str(), strNewName.c_str()); //删除
			}
			strNewName = m_data->m_strFilePath + "." + std::to_string(nLogNumIndex);
		}

#elif defined OS_MMR_LINUX
		if (access(strNewName.c_str(), 0) == F_OK)                                                 //检查文件夹是否存在，不存在则创建
		{
			remove(strNewName.c_str()); //删除
		}
		while (--nLogNumIndex > 0) //减小序号，以此更改日志名称
		{
			strOldName = m_data->m_strFilePath + "." + std::to_string(nLogNumIndex);
			if (access(strOldName.c_str(), 0) == F_OK) //检查文件夹是否存在，不存在则创建
			{
				rename(strOldName.c_str(), strNewName.c_str()); //删除
			}
			strNewName = m_data->m_strFilePath + "." + std::to_string(nLogNumIndex);
		}
#endif
		rename(m_data->m_strFilePath.c_str(), strNewName.c_str()); //重命名
		m_logStream.open(m_data->m_strFilePath, std::fstream::app);
	}
}

