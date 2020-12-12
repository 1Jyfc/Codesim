# 项目介绍

Codesim是南京大学2020年《软件工程研究导引》课程的课程PA作业，该工具可以分析两个cpp源码，并评判它们的相似度，用于确定这两份代码是否涉嫌代码抄袭。

代码核心算法基于[MOSS](https://theory.stanford.edu/~aiken/moss/)，并基于[clang](https://clang.llvm.org/)提供的代码分析功能对C++代码进行分析操作。



# 依赖获取

代码运行需要使用g++和clang，故在命令行运行：

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