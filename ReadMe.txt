一.用户使用说明
编译生成：
定位到code文件夹，使用make进行构建，生成UNIX-FILESYS.exe可执行文件。生成的可执行文件UNIX-FILESYS在code文件夹下。可省略编译步骤，直接使用压缩包内的可执行文件，在Windows系统下双击该可执行文件即可运行。
使用操作：
点击可执行文件后键入build。build指令进行初始路径的构建，并在相应路径创建读入上层目录的reports.pdf，ReadMe.txt，star.png文件。
键入help 查看该系统支持的指令。键入help -指令名查看对应指令详情。输入相应指令即可进行操作。
请使用exit指令退出该系统，不要直接关闭。
二.压缩包内文件说明
code:存放源代码、可执行文件和镜像文件myDisk.img，若采用编译方法运行，其下会产生obj文件夹用来存放.o文件。运行可执行文件后才可生成镜像文件。
ReadMe.txt:项目说明
reports.pdf:实验报告
star.png:测试图片文件
注意：ReadMe.txt，reports.pdf，star.png可执行文件相对位置不要随意更改，否则build可能失败。

1. User Instructions
Compilation generates:
Navigate to the code folder, use make to build, and generate the UNIX-FILESYS.exe executable file. The generated executable file UNIX-FILESYS is under the code folder. The compilation step can be omitted, and the executable file in the compressed package can be directly used, and the executable file can be run by double-clicking the executable file under the Windows system.
Operation:
Type build after clicking on the executable. The build command constructs the initial path, and creates reports.pdf, ReadMe.txt, and star.png files read into the upper directory in the corresponding path.
Type help to view the commands supported by the system. Type help -command name to view the details of the corresponding command. Enter the corresponding command to operate.
Please use the exit command to exit the system, do not close it directly.
2. Description of the files in the compressed package
code: store the source code, executable file and image file myDisk.img, if run by compiling method, obj folder will be generated under it to store the .o file. The image file can only be generated after running the executable file.
ReadMe.txt: Project Description
reports.pdf: experiment report
star.png: test image file
Note: Do not change the relative positions of ReadMe.txt, reports.pdf, and star.png executable files at will, otherwise the build may fail.
