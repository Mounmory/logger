#include "Logger.h"
#include "TimeCounter.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include <atomic>	

std::atomic_bool g_atoStart;

void logThread() 
{
	while (!g_atoStart.load());

	std::cout << "write log start!" << std::endl;

	for (int i = 0; i < 200000; ++i)
	{
		LOG_FORCE("multi thread log test. a looooooooooooooooooooooooooooooooooooooooooooooooooooooooog string, i value is %d!", i);
	}
}

int main()
{
	//测试同步写日志
	std::cout << "log write test,log size about 57 MB every time..." << std::endl;
	{
		std::cout << "syn log test..." << std::endl;
		mmrComm::Singleton<mmrUtil::Logger>::initInstance(10, 16, false);
		mmrUtil::TimeCounter timeCouner;
		for (int i = 0; i < 400000; ++i)
		{
			LOG_FORCE("syn log test. a looooooooooooooooooooooooooooooooooooooooooooooooooooooooog string, i value is %d!", i);
			//std::cout << "i value is" << i << std::endl;
			//std::this_thread::sleep_for(std::chrono::microseconds(10));
		}
		std::cout << "syn write log 57MB time cost " << timeCouner.elapsed_micro() << " us" << std::endl;
		mmrComm::Singleton<mmrUtil::Logger>::destroyInstance();//清理内存分配器
	}
	std::cout << "输入任意字符继续测试..." << std::endl;
	std::cin.get();
	//异步写日志
	{
		std::cout << "asyn log test..." << std::endl;
		mmrComm::Singleton<mmrUtil::Logger>::initInstance(10, 16, true);
		mmrUtil::TimeCounter timeCouner;
		for (int i = 0; i < 400000; ++i)
		{
			LOG_FORCE("asyn log test. a looooooooooooooooooooooooooooooooooooooooooooooooooooooooog string, i value is %d!", i);
		}
		std::cout << "sigle thread asyn write log 57MB time cost " << timeCouner.elapsed_micro() << " us" << std::endl;
		mmrComm::Singleton<mmrUtil::Logger>::destroyInstance();//清理内存分配器
	}
	std::cout << "输入任意字符继续测试..." << std::endl;
	std::cin.get();
	//异步多线程写日志
	{
		std::cout << "multi thread log test..." << std::endl;
		mmrComm::Singleton<mmrUtil::Logger>::initInstance(10, 16, true);

		g_atoStart.store(false);
		std::vector<std::thread> vecThread;
		for (int i = 0; i < 2; ++i)
		{
			vecThread.emplace_back(std::thread(logThread));
		}
		g_atoStart.store(true);
		mmrUtil::TimeCounter timeCouner;
		for (auto& iterThread : vecThread)
		{
			iterThread.join();
		}
		std::cout << "multi thread asyn write log 57MB time cost " << timeCouner.elapsed_micro() << " us" << std::endl;
		mmrComm::Singleton<mmrUtil::Logger>::destroyInstance();//清理内存分配器
	}


	std::cout << "输入任意字符退出..." << std::endl;
	std::cin.get();

	return 0;
}
