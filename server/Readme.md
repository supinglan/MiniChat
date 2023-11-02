### 编译运行说明

#### Clion

使用Clion打开项目文件夹Server，载入cmake项目，然后打开server.cpp,点击编译运行即可再cmake-build-debug下获得可执行文件server.exe。

### Mingw64-g++

若系统已经安装g++编译器并添加到系统环境路径中，在文件夹Server下输入以下命令：

g++ server.cpp client.cpp  -o server -l ws2_32

即可在同文件夹下获得可执行文件server.exe.