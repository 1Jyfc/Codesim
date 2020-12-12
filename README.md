# 依赖获取

需要使用g++和clang，故在命令行运行：

`sudo apt-get install g++ clang libclang-10-dev`

即完成配置。

# 编译

命令行运行：

`make`

即可编译生成可执行文件codesim。

运行：

`./codesim --help`或`./codesim -h`

即可打印使用说明。

运行：

`./codesim [path of code1] [path of code2]`

即可运行codesim，输出两个代码文件的相似度百分比。如果出错，会打印一行错误报告。

运行：

`make clean`

即可清除可执行文件。