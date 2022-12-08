#include "FileManager.h"
#include "Kernel.h"
#include "Utility.h"

extern FileSystem g_FileSystem;
extern InodeTable g_InodeTable;
extern OpenFileTable g_OpenFileTable;

/*==========================class FileManager===============================*/

FileManager::FileManager()
{
	//nothing to do here
}

FileManager::~FileManager()
{
	//nothing to do here
}

void FileManager::Initialize()
{
	m_FileSystem = &g_FileSystem;
	m_OpenFileTable = &g_OpenFileTable;
	m_InodeTable = &g_InodeTable;
	rootDirInode = m_InodeTable->IGet(FileSystem::ROOTINO);
}

/*
* ���ܣ����ļ�
* Ч�����������ļ��ṹ���ڴ�i�ڵ㿪�� ��i_count Ϊ������i_count ++��
* */
void FileManager::Open()
{
	Inode* pInode;
	User& u = Kernel::Instance().GetUser();

	pInode = this->NameI(FileManager::OPEN);
	if (NULL == pInode)
		return;

	this->Open1(pInode, u.u_arg[1], 0);
}

/*
 * ���ܣ�����һ���µ��ļ�
 * Ч�����������ļ��ṹ���ڴ�i�ڵ㿪�� ��i_count Ϊ������Ӧ���� 1��
 * */
void FileManager::Creat()
{
	Inode* pInode;
	User& u = Kernel::Instance().GetUser();
	unsigned int newACCMode = u.u_arg[1];

	/* ����Ŀ¼��ģʽΪ1����ʾ����������Ŀ¼����д�������� */
	pInode = this->NameI(FileManager::CREATE);

	/* û���ҵ���Ӧ��INode����NameI���� */
	if (NULL == pInode) {
		if (u.u_error)
			return;
		/* ����Inode */
		pInode = this->MakNode(newACCMode);
		/* ����ʧ�� */
		if (NULL == pInode)
			return;

		/*
		 * �����ϣ�������ֲ����ڣ�ʹ�ò���trf = 2������open1()��
		 * ����Ҫ����Ȩ�޼�飬��Ϊ�ոս������ļ���Ȩ�޺ʹ������mode
		 * ����ʾ��Ȩ��������һ���ġ�
		 */
		this->Open1(pInode, File::FWRITE, 2);
	}
	else /*���NameI()�������Ѿ�����Ҫ�������ļ�*/
	{
		/* ������ļ���Ŀ¼�ļ������󣬲������µĴ���*/
		if (pInode->i_mode & Inode::IFDIR) {
			u.u_error = u.U_EISDIR;
			return;
		}
		/* ��ո��ļ������㷨ITrunc()��*/
		this->Open1(pInode, File::FWRITE, 1);
		pInode->i_mode |= newACCMode;
	}
}

/*
* trf == 0��open����
* trf == 1��creat���ã�creat�ļ���ʱ��������ͬ�ļ������ļ�
* trf == 2��creat���ã�creat�ļ���ʱ��δ������ͬ�ļ������ļ��������ļ�����ʱ��һ������
* mode���������ļ�ģʽ����ʾ�ļ������� ����д���Ƕ�д
*/
void FileManager::Open1(Inode* pInode, int mode, int trf)
{
	User& u = Kernel::Instance().GetUser();

	/*
	 * ����ϣ�����ļ��Ѵ��ڵ�����£���trf == 0��trf == 1����Ȩ�޼��
	 * �����ϣ�������ֲ����ڣ���trf == 2������Ҫ����Ȩ�޼�飬��Ϊ�ս���
	 * ���ļ���Ȩ�޺ʹ���Ĳ���mode������ʾ��Ȩ��������һ���ġ�
	 */
	if (trf != 2 )
	{
		if (mode & File::FREAD)
		{
			/* ����Ȩ�� */
			this->Access(pInode, Inode::IREAD);
		}
		if (mode & File::FWRITE)
		{
			/* ���дȨ�� */
			this->Access(pInode, Inode::IWRITE);
		}
	}

	if (u.u_error)
	{
		/*�������ͬ���ļ�Ȩ�޷�����ͻ�����ͷ�inode*/
		this->m_InodeTable->IPut(pInode);
		return;
	}

	/* ��creat�ļ���ʱ��������ͬ�ļ������ļ����ͷŸ��ļ���ռ�ݵ������̿� */
	if (1 == trf)
		pInode->ITrunc();


	/* ������ļ����ƿ�File�ṹ */
	File* pFile = this->m_OpenFileTable->FAlloc();
	if (NULL == pFile)
	{
		this->m_InodeTable->IPut(pInode);
		return;
	}

	/* ���ô��ļ���ʽ������File�ṹ���ڴ�INode�Ĺ�����ϵ */
	pFile->f_flag = mode & (File::FREAD | File::FWRITE);
	pFile->f_inode = pInode;

	/* Ϊ�򿪻��ߴ����ļ��ĸ�����Դ���ѳɹ����䣬�������� */
	if (u.u_error == 0)
		return;
	else	/* ����������ͷ���Դ */
	{
		/* �ͷŴ��ļ������� */
		int fd = u.u_ar0[User::EAX];
		if (fd != -1)
		{
			u.u_ofiles.SetF(fd, NULL);
			/* �ݼ�File�ṹ��INode�����ü��� ,File�ṹû���� f_countΪ0�����ͷ�File�ṹ��*/
			pFile->f_count--;
		}
		this->m_InodeTable->IPut(pInode);
	}
}

void FileManager::Close()
{
	User& u = Kernel::Instance().GetUser();
	int fd = u.u_arg[0];

	/* ��ȡ���ļ����ƿ�File�ṹ */
	File* pFile = u.u_ofiles.GetF(fd);
	if (NULL == pFile)
		return;

	/* �ͷŴ��ļ�������fd���ݼ�File�ṹ���ü��� */
	u.u_ofiles.SetF(fd, NULL);
	this->m_OpenFileTable->CloseF(pFile);
}

void FileManager::Seek()
{
	File* pFile;
	User& u = Kernel::Instance().GetUser();
	int fd = u.u_arg[0];

	pFile = u.u_ofiles.GetF(fd);
	if (NULL == pFile)
	{
		return;  /* ��FILE�����ڣ�GetF��������� */
	}

	int offset = u.u_arg[1];

	switch (u.u_arg[2])
	{
		/* ��дλ������Ϊoffset */
	case 0:
		pFile->f_offset = offset;
		break;
		/* ��дλ�ü�offset(�����ɸ�) */
	case 1:
		pFile->f_offset += offset;
		break;
		/* ��дλ�õ���Ϊ�ļ����ȼ�offset */
	case 2:
		pFile->f_offset = pFile->f_inode->i_size + offset;
		break;
	}
}

void FileManager::Read()
{
	/* ֱ�ӵ���Rdwr()�������� */
	this->Rdwr(File::FREAD);
}

void FileManager::Write()
{
	/* ֱ�ӵ���Rdwr()�������� */
	this->Rdwr(File::FWRITE);
}

void FileManager::Rdwr(enum File::FileFlags mode)
{
	File* pFile;
	User& u = Kernel::Instance().GetUser();

	/* ����Read()/Write()��ϵͳ���ò���fd��ȡ���ļ����ƿ�ṹ */
	pFile = u.u_ofiles.GetF(u.u_arg[0]);	/* fd */
	if (NULL == pFile)
	{
		/* �����ڸô��ļ���GetF�Ѿ����ù������룬�������ﲻ��Ҫ�������� */
		/*	u.u_error = User::EBADF;	*/
		return;
	}

	/* ��д��ģʽ����ȷ */
	if ((pFile->f_flag & mode) == 0)
	{
		u.u_error = User::U_EACCES;
		return;
	}

	u.u_IOParam.m_Base = (unsigned char*)u.u_arg[1];     /* Ŀ�껺������ַ */
	u.u_IOParam.m_Count = u.u_arg[2];		/* Ҫ���/д���ֽ��� */

	/* �����ļ���ʼ��λ�� */
	u.u_IOParam.m_Offset = pFile->f_offset;   /* �����ļ���ʼ��λ�� */
	if (File::FREAD == mode) 
	{
		pFile->f_inode->ReadI();
	}
	else {
		pFile->f_inode->WriteI();
	}

	/* ���ݶ�д�������ƶ��ļ���дƫ��ָ�� */
	pFile->f_offset += (u.u_arg[2] - u.u_IOParam.m_Count);

	/* ����ʵ�ʶ�д���ֽ������޸Ĵ��ϵͳ���÷���ֵ�ĺ���ջ��Ԫ */
	u.u_ar0[User::EAX] = u.u_arg[2] - u.u_IOParam.m_Count;
}


/* ����NULL��ʾĿ¼����ʧ�ܣ������Ǹ�ָ�룬ָ���ļ����ڴ��i�ڵ� ���������ڴ�i�ڵ�  */
Inode* FileManager::NameI(enum DirectorySearchMode mode)
{
	Inode* pInode;
	Buf* pBuf;
	int freeEntryOffset;
	User& u = Kernel::Instance().GetUser();
	BufferManager& bufMgr = Kernel::Instance().GetBufferManager();
	
	unsigned int index = 0, nindex = 0;

	/*
	 * �����·����'/'��ͷ�ģ��Ӹ�Ŀ¼��ʼ������
	 * ����ӽ��̵�ǰ����Ŀ¼��ʼ������
	 */
	pInode = u.u_cdir;
	if ('/' == u.u_dirp[0])
	{
		nindex = ++index + 1;
		pInode = this->rootDirInode;
	}


	/* ���ѭ��ÿ�δ���pathname��һ��·������ */
	while (true)
	{
		/* ����������ͷŵ�ǰ��������Ŀ¼�ļ�Inode�����˳� */
		if (u.u_error != User::U_NOERROR) {
			break;
		}
		/* ����·��������ϣ�������ӦInodeָ�롣Ŀ¼�����ɹ����ء� */
		if (nindex >= u.u_dirp.length())
		{
			return pInode;
		}

		/* ���Ҫ���������Ĳ���Ŀ¼�ļ����ͷ����INode��Դ���˳� */
		if ((pInode->i_mode & Inode::IFMT) != Inode::IFDIR) {
			u.u_error = User::U_ENOTDIR;
			break;
		}

		nindex = u.u_dirp.find_first_of('/', index);
		Utility::MemSet(u.u_dbuf, 0, sizeof(u.u_dbuf));
		Utility::MemCopy(u.u_dbuf, u.u_dirp.data() + index, (nindex == (unsigned int)std::string::npos ? u.u_dirp.length() : nindex) - index);
		index = nindex + 1;

		/* �ڲ�ѭ�����ֶ���u.dbuf[]�е�·���������������Ѱƥ���Ŀ¼�� */
		u.u_IOParam.m_Offset = 0;
		/* ����ΪĿ¼����� �����հ׵�Ŀ¼��*/
		u.u_IOParam.m_Count = pInode->i_size / sizeof(DirectoryEntry);
		freeEntryOffset = 0;
		pBuf = NULL;

		/* ��һ��Ŀ¼��Ѱ�� */
		while (true)
		{
			/* ��Ŀ¼���Ѿ�������� */
			if (0 == u.u_IOParam.m_Count) {
				if (NULL != pBuf)
				{
					bufMgr.Brelse(pBuf);
				}

				/* ����Ǵ������ļ� */
				if (FileManager::CREATE == mode && nindex >= u.u_dirp.length())
				{
					/* ����Ŀ¼Inodeָ�뱣���������Ժ�дĿ¼��WriteDir()�������õ� */
					u.u_pdir = pInode;
					if (freeEntryOffset)   /* �˱�������˿���Ŀ¼��λ��Ŀ¼�ļ��е�ƫ���� */
					{
						/* ������Ŀ¼��ƫ��������u���У�дĿ¼��WriteDir()���õ� */
						u.u_IOParam.m_Offset = freeEntryOffset - sizeof(DirectoryEntry);
					}
					else
					{
						pInode->i_flag |= Inode::IUPD;
					}
					/* �ҵ�����д��Ŀ���Ŀ¼��λ�ã�NameI()�������� */
					return NULL;
				}
				/* Ŀ¼��������϶�û���ҵ�ƥ����ͷ����Inode��Դ�����˳� */
				u.u_error = User::U_ENOENT;
				goto out;
			}

			/* �Ѷ���Ŀ¼�ļ��ĵ�ǰ�̿飬��Ҫ������һĿ¼�������̿� */
			if (0 == u.u_IOParam.m_Offset % Inode::BLOCK_SIZE)
			{
				if (NULL != pBuf)
				{
					bufMgr.Brelse(pBuf);
				}
				/* ����Ҫ���������̿�� */
				int phyBlkno = pInode->Bmap(u.u_IOParam.m_Offset / Inode::BLOCK_SIZE);
				pBuf = bufMgr.Bread(phyBlkno);
			}

			/* û�ж��굱ǰĿ¼���̿飬���ȡ��һĿ¼����u.u_dent */
			Utility::MemCopy(&u.u_dent, pBuf->b_addr + (u.u_IOParam.m_Offset % Inode::BLOCK_SIZE), sizeof(u.u_dent));

			u.u_IOParam.m_Offset += sizeof(DirectoryEntry);
			u.u_IOParam.m_Count--;

			/* ����ǿ���Ŀ¼���¼����λ��Ŀ¼�ļ���ƫ���� */
			if (0 == u.u_dent.m_ino)
			{
				if (0 == freeEntryOffset)
				{
					freeEntryOffset = u.u_IOParam.m_Offset;
				}
				/* ��������Ŀ¼������Ƚ���һĿ¼�� */
				continue;
			}

			if (!Utility::MemCmp(u.u_dbuf, &u.u_dent.m_name, sizeof(DirectoryEntry) - 4))
				break;
		}

		if (NULL != pBuf)
		{
			bufMgr.Brelse(pBuf);
		}

		/* �����ɾ���������򷵻ظ�Ŀ¼INode����Ҫɾ���ļ���INode����u.dent.m_ino�� */
		if (FileManager::DELETE == mode && nindex >= u.u_dirp.length())
			return pInode;


		/*
		* ƥ��Ŀ¼��ɹ������ͷŵ�ǰĿ¼INode������ƥ��ɹ���
		* Ŀ¼��m_ino�ֶλ�ȡ��Ӧ��һ��Ŀ¼���ļ���INode��
		*/
		this->m_InodeTable->IPut(pInode);
		pInode = this->m_InodeTable->IGet(u.u_dent.m_ino);

		if (NULL == pInode) /* ��ȡʧ�� */
		{
			return NULL;
		}
	}

out:
	this->m_InodeTable->IPut(pInode);
	return NULL;
}


/* ��creat���á�
 * Ϊ�´������ļ�д�µ�i�ڵ���µ�Ŀ¼��
 * ���ص�pInode�����������ڴ�i�ڵ㣬���е�i_count�� 1��
 *
 * �ڳ����������� WriteDir��������������Լ���Ŀ¼��д����Ŀ¼���޸ĸ�Ŀ¼�ļ���i�ڵ� ������д�ش��̡�
 *
 */
Inode* FileManager::MakNode(unsigned int mode)
{
	Inode* pInode;
	User& u = Kernel::Instance().GetUser();

	/* ����һ������DiskInode������������ȫ����� */
	pInode = this->m_FileSystem->IAlloc();
	if (NULL == pInode)
		return NULL;

	pInode->i_flag |= (Inode::IACC | Inode::IUPD);
	pInode->i_mode = mode | Inode::IALLOC;
	pInode->i_nlink = 1;
	/* ��Ŀ¼��д��u.u_dent�����д��Ŀ¼�ļ� */
	this->WriteDir(pInode);
	return pInode;
}

void FileManager::WriteDir(Inode* pInode)
{
	User& u = Kernel::Instance().GetUser();

	/* ����Ŀ¼����INode��Ų��� */
	u.u_dent.m_ino = pInode->i_number;

	/* ����Ŀ¼����pathname�������� */
	for (int i = 0; i < DirectoryEntry::DIRSIZ; i++)
	{
		u.u_dent.m_name[i] = u.u_dbuf[i];
	}

	u.u_IOParam.m_Count = DirectoryEntry::DIRSIZ + 4;
	u.u_IOParam.m_Base = (unsigned char*)&u.u_dent;

	/* ��Ŀ¼��д�븸Ŀ¼�ļ� */
	u.u_pdir->WriteI();
	//this->m_InodeTable->IPut(u.u_pdir);
}

/*
 * ����ֵ��0����ʾӵ�д��ļ���Ȩ�ޣ�1��ʾû������ķ���Ȩ�ޡ��ļ�δ�ܴ򿪵�ԭ���¼��u.u_error�����С�
 */
int FileManager::Access(Inode* pInode, unsigned int mode)
{
	User& u = Kernel::Instance().GetUser();

	if ((pInode->i_mode & mode) != 0)
	{
		return 0;
	}

	u.u_error = User::U_EACCES;
	return 1;
}

/* �ı䵱ǰ����Ŀ¼ */
void FileManager::ChDir()
{
	Inode* pInode;
	User& u = Kernel::Instance().GetUser();

	pInode = this->NameI(FileManager::OPEN);
	if (NULL == pInode)
	{
		return;
	}

	/* ���������ļ�����Ŀ¼�ļ� */
	if ((pInode->i_mode & Inode::IFMT) != Inode::IFDIR)
	{
		u.u_error = User::U_ENOTDIR;
		this->m_InodeTable->IPut(pInode);
		return;
	}

	/* ����u_cdir */
	u.u_cdir = pInode;
	/* ����u_curdir */
	Kernel::Instance().GetUser().SetCurDir(pInode);
}

void FileManager::UnLink()
{
	//ע��ɾ���ļ����д���й¶
	Inode* pInode;
	Inode* pDeleteInode;
	User& u = Kernel::Instance().GetUser();

	pDeleteInode = this->NameI(FileManager::DELETE);
	if (NULL == pDeleteInode)
		return;

	pInode = this->m_InodeTable->IGet(u.u_dent.m_ino);
	if (NULL == pInode)
	{
		Utility::Panic("unlink -- iget", Kernel::Instance().GetUser().U_ENOSPC);
	}

	/* д��������Ŀ¼�� */
	u.u_IOParam.m_Offset -= (DirectoryEntry::DIRSIZ + 4);
	u.u_IOParam.m_Base = (unsigned char*)&u.u_dent;
	u.u_IOParam.m_Count = DirectoryEntry::DIRSIZ + 4;

	u.u_dent.m_ino = 0;
	pDeleteInode->WriteI();

	/* �޸�inode�� */
	pInode->i_nlink--;
	pInode->i_flag |= Inode::IUPD;

	this->m_InodeTable->IPut(pDeleteInode);
	this->m_InodeTable->IPut(pInode);
}


void FileManager::Format()
{
	this->m_OpenFileTable->Format();
	this->m_InodeTable->Format();
	//rootDirInode = m_InodeTable->IGet(FileSystem::ROOTINO);
}

