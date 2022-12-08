#include "User.h"
#include "Utility.h"
#include "Kernel.h"
#include <iostream>
#include <fstream>

extern FileManager g_FileManager;

User::User() 
{
}

User::~User() 
{
}

void User::Initialize() 
{
    u_error = U_NOERROR;
    u_dirp = "/";
    u_curdir = "/";
    u_cdir = g_FileManager.rootDirInode;
    u_pdir = NULL;
    Utility::MemSet(u_arg, 0, sizeof(u_arg));
    u_IOParam.Initialize();
    u_ofiles.Initialize();

}

std::string User::Ls() 
{
    std::string res = "";
    Inode* pInode = this->u_cdir;
    Buf* pBuffer = NULL;

    /* 设置IO起始 */
    this->u_IOParam.m_Offset = 0;

    int fileNum = pInode->i_size / sizeof(DirectoryEntry);//获得文件数量

    while (fileNum) 
    {
        if (0 == this->u_IOParam.m_Offset % Inode::BLOCK_SIZE) 
        {
            if (pBuffer!= NULL) 
                Kernel::Instance().GetBufferManager().Brelse(pBuffer);
            
            int phyBlkno = pInode->Bmap(u_IOParam.m_Offset / Inode::BLOCK_SIZE);
            pBuffer = Kernel::Instance().GetBufferManager().Bread(phyBlkno);
        }
        Utility::MemCopy(&u_dent, pBuffer->b_addr + (u_IOParam.m_Offset % Inode::BLOCK_SIZE), sizeof(u_dent));
        this->u_IOParam.m_Offset += sizeof(DirectoryEntry);
        fileNum--;

        if (0 == this->u_dent.m_ino)
            continue;
        res += this->u_dent.m_name;
        res += "\n";
    }

    if (pBuffer) 
        Kernel::Instance().GetBufferManager().Brelse(pBuffer);

    return res;
}


void User::SetDirp(std::string path)
{
    
    if (path.substr(0, 2) != "..") {
        u_dirp = path;
    }
    else {
        std::string fdir = u_curdir;//父路径
        if (fdir == "/") {
            u_dirp = "/";
            return;
        }
        unsigned int p = 0;//path初始下标
        //多重相对路径 .. ../ ../.. ../../
        for (; p < path.length() && path.substr(p, 2) == ".."; ) {
            fdir.pop_back();
            fdir.erase(fdir.find_last_of('/') + 1);
            p += 2;
            p += p < path.length() && path[p] == '/';
        }
        u_dirp = fdir + path.substr(p);
    }

    if (u_dirp.length() > 1 && u_dirp.back() == '/') {
        u_dirp.pop_back();
    }

}

void User::SetCurDir(Inode* pInode) 
{
    /* 路径不是从根目录'/'开始，则在现有u.u_curdir后面加上当前路径分量 */
    if (u_dirp[0] != '/') {
        u_curdir += u_dirp;
    }
    else {
        /* 如果是从根目录'/'开始，则取代原有工作目录 */
        u_curdir = u_dirp;
    }
    if (u_curdir.back() != '/')
        u_curdir.push_back('/');
}

bool User::JudgeError() 
{
    if (u_error != U_NOERROR) 
    {
        return true;
    }
    return false;
}

std::string User::ErrorStr()
{
    switch (u_error)
    {
    case U_NOERROR:
        return " No error ";
    case U_EPERM:
        return " Operation not permitted ";
    case U_ENOENT:
        return " No such file or directory";
    case U_ESRCH:
        return " No such process ";
    case U_EINTR:
        return " Interrupted system call ";
    case U_EIO:
        return " I/O error ";
    case U_ENXIO:
        return " No such device or address ";
    case U_E2BIG:
        return " Arg list too long ";
    case U_ENOEXEC:
        return " Exec format error ";
    case U_EBADF:
        return " Bad file number ";
    case U_ECHILD:
        return " No child processes ";
    case U_EAGAIN:
        return " Try again ";
    case U_ENOMEM:
        return " Out of memory ";
    case U_EACCES:
        return " Permission denied ";
    case U_EFAULT:
        return " Bad address ";
    case U_ENOTBLK:
        return " Block device required ";
    case U_EBUSY:
        return " Device or resource busy ";
    case U_EEXIST:
        return " File exists ";
    case U_EXDEV:
        return " Cross-device link ";
    case U_ENODEV:
        return " No such device ";
    case U_ENOTDIR:
        return " Not a directory ";
    case U_EISDIR:
        return " Is a directory ";
    case U_EINVAL:
        return " Invalid argument ";
    case U_ENFILE:
        return " File table overflow ";
    case U_EMFILE:
        return " Too many open files ";
    case U_ENOTTY:
        return " Not a typewriter(terminal) ";
    case U_ETXTBSY:
        return " Text file busy ";
    case U_EFBIG:
        return " File too large ";
    case U_ENOSPC:
        return " No space left on device ";
    case U_ESPIPE:
        return " Illegal seek ";
    case U_EROFS:
        return " Read-only file system ";
    case U_EMLINK:
        return " Too many links ";
    case U_EPIPE:
        return " Broken pipe ";
    case U_ENOSYS:
        return " No System ";
    default:
        break;
    }
    return "";
}