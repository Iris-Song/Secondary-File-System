#pragma once

#ifndef DISKDRIVER_H
#define DISKDRIVER_H

#include "Buf.h"
#include <iostream>

class DiskDriver 
{
public:
    /* ���̾����ļ��� */
    static const char* DISK_FILE_NAME;

public:
    DiskDriver();
    ~DiskDriver();

    /* ��龵���ļ��Ƿ���� */
    bool Exists();

    /* �򿪾����ļ� */
    void Mount();

    /* д���� */
    void Write(const void* Buf, unsigned int size,
        int offset = -1, unsigned int origin = SEEK_SET);

    /* ������ */
    void Read(void* Buf, unsigned int size,
        int offset = -1, unsigned int origin = SEEK_SET);

private:

    FILE* fp;    /* �����ļ�ָ�� */

};

#endif
