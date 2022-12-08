#define _CRT_SECURE_NO_WARNINGS
#include "DiskDriver.h"
#include "Utility.h"
#include "User.h"

extern User g_User;

/*==============================class DiskDriver===================================*/
const char* DiskDriver::DISK_FILE_NAME = "myDisk.img";

DiskDriver::DiskDriver() 
{
    fp = fopen(DISK_FILE_NAME, "rb+");
}

DiskDriver::~DiskDriver() 
{
    if (fp) 
    {
        fclose(fp);
    }
}


bool DiskDriver::Exists() 
{
    return fp != NULL;
}

void DiskDriver::Mount() 
{
    fp = fopen(DISK_FILE_NAME, "wb+");
    if (fp == NULL) 
    {
        Utility::Panic("fail to mount disk driver",g_User.U_ENXIO);
    }
}

void DiskDriver::Write(const void* Buf, unsigned int size, int offset, unsigned int origin) 
{
    fseek(fp, offset, origin);
    fwrite(Buf, size, 1, fp);
}

void DiskDriver::Read(void* Buf, unsigned int size, int offset, unsigned int origin) 
{
    fseek(fp, offset, origin);
    fread(Buf, size, 1, fp);
}