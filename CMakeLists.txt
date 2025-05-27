cmake_minimum_required(VERSION 3.10)

# 设置项目名称
project(mmrLogger)

if(WIN32)
	message("platform windows")
	if(MSVC)
		#STRING(REPLACE "-O2" "-Od" CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})#取消优化
		if(MSVC_VERSION LESS 1900) # 判断编译器版本是否低于VS2015
			message(FATAL_ERROR "MSVC version must be at least 14.0 (Visual Studio 2015)!")
		endif()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++14")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus /source-charset:utf-8")#源文件编码方式
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /source-charset:utf-8")#设置C编译选项
		add_definitions(-DOS_MMR_WIN)#添加window系统宏
	else()
		message(FATAL_ERROR "only msvc project can be generated!")
	endif()
elseif(UNIX)
	message("platform linux")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -fPIC")#设置C++编译选项
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
	add_definitions(-DOS_MMR_LINUX)#添加linux系统宏
else()	
	message(FATAL_ERROR "Unsupported OS type!")
endif()

# 自动查找当前目录下所有的.cpp、.h和.hpp文件
file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h" "*.hpp")

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

if(UNIX )
	target_link_libraries(${PROJECT_NAME} pthread)
endif()