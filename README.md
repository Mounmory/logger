### mmrLogger - 异步多缓冲日志库  
### mmrLogger - Asynchronous Multi-Buffer Logger  

#### 0 概述/Overview
- **中文**:
  `mmrLogger` 是一个基于**异步多缓冲队列**的高性能日志库，采用 CMake 构建，可跨平台应用于 Windows 和 Linux 项目。  

- **English**:
  `mmrLogger` is a high-performance asynchronous logger using a multi-buffer queue. It is built with CMake and supports both Windows and Linux projects.  

---

#### 1 开发环境 / Development Environments

##### Windows 支持/Windows Support
- **中文**:
  已测试通过环境：  
  - VS2015、VS2017、VS2019  
  - *更高版本的 Visual Studio 理论上支持。*  

- **English**:
  Tested and verified on:  
  - VS2015, VS2017, VS2019  
  - *Higher versions of Visual Studio should be compatible.*  

##### Linux 支持 / Linux Support 
- **中文**:
  已测试通过环境：  
  - Ubuntu 16、18、20（GCC 编译器）  
  - *更高版本的 GCC 编译器应兼容。*  

- **English**:
  Tested on:  
  - Ubuntu 16.04, 18.04, 20.04 (GCC compiler)  
  - *Higher GCC versions should work.*  

---

#### 2 特性 / Features
- 异步多缓冲队列 / **Asynchronous Multi-Buffer Queue**  
- 高性能 / **High Performance**
- CMake 构建跨平台（Windows/Linux） / **Cross-Platform whith CMake (Windows/Linux)**

---

#### 3 使用方法 / Usage
- **中文**:
  - Window：使用CMake直接构建Visual Studio工程，打开后编辑运行即可 
  - Linux：使用CMake-gui构建或在控制台使用如下命令构建
		```bash
		# Build with CMake In Linux 
		mkdir build && cd build  
		cmake ..  
		make  
		```

- **English**:
  - Window: Use CMake to generate the Visual Studio project directly. Open the solution file (.sln) to edit and run.
  - Linux: Build with CMake-gui or via the command line. 		
		```bash
		# Build with CMake In Linux 
		mkdir build && cd build  
		cmake ..  
		make  
		```
		
#### 4 更多信息 / For more infomation
- **中文**:
  - 代码源自我的开源项目MmmrService（https://github.com/Mounmory/MmrService），同时在MmmrService中，实现了按模块动态控制日志等级功能。
- **English**:
  - The code originates from my open-source project MmmrService (https://github.com/Mounmory/MmrService). In MmmrService, I've implemented module-based dynamic log level control, allowing granular logging configuration per module.