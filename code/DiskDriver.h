#pragma once

#ifndef DISKDRIVER_H
#define DISKDRIVER_H

#include "Buf.h"
#include <iostream>

class DiskDriver 
{
public:
    /* 磁盘镜像文件名 */
    static const char* DISK_FILE_NAME;

public:
    DiskDriver();
    ~DiskDriver();

    /* 检查镜像文件是否存在 */
    bool Exists();

    /* 打开镜像文件 */
    void Mount();

    /* 写磁盘 */
    void Write(const void* Buf, unsigned int size,
        int offset = -1, unsigned int origin = SEEK_SET);

    /* 读磁盘 */
    void Read(void* Buf, unsigned int size,
        int offset = -1, unsigned int origin = SEEK_SET);

private:

    FILE* fp;    /* 磁盘文件指针 */

};

#endif
