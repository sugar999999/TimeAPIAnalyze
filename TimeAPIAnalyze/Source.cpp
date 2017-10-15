#include <windows.h>
#include <iostream>
#include <shlobj.h>

BOOL OutPutFileTime(FILETIME ft, LPSTR name)
{
	__int64 *i64Ft = (__int64*)&ft;

	std::cout << name << ": " << std::endl <<
		*i64Ft <<
		std::endl << std::endl;

	return TRUE;
}

BOOL OutPutSystemTime(SYSTEMTIME *st, LPSTR name, BOOL bLocal)
{
	for (int roop = 0; roop < 3; roop++)
	{
		if(bLocal)
			GetLocalTime(st);
		std::cout << name << ": " << std::endl <<
			(*st).wYear << "/" <<
			(*st).wMonth << "/" <<
			(*st).wDay << " " <<
			(*st).wHour << ":" <<
			(*st).wMinute << ":" <<
			(*st).wSecond <<
			std::endl << std::endl;

		Sleep(1000);
	}
	
	return TRUE;
}

__int64 GetGapFileTime(FILETIME ft1, FILETIME ft2)
{
	FILETIME fGapTime    = {};
	__int64 *i64ft1      = (__int64*)&ft1;
	__int64 *i64ft2      = (__int64*)&ft2;
	__int64 i64GapTime   = abs(*i64ft1 - *i64ft2);
	__int64 dwGapMin     = i64GapTime / 10000 / 1000 / 60;

	std::cout << "���ԍ�(��): " << i64GapTime / 10000 / 1000 / 60 << std::endl << std::endl;
	
	return dwGapMin;
}

DWORD GetDirectoryOnDirectlyBelowTheRoaming(LPSTR path, LPCSTR dirName, DWORD size)
{
	
	if (!SHGetSpecialFolderPathA(NULL, path, CSIDL_APPDATA, FALSE))
	{
		std::cout << "[ERROR]" << __LINE__ << "::SHGetSpecialFolderPathA is Failed (" << GetLastError() << ")" << std::endl;
		return 1;
	}

	if (dirName && strlen(dirName) > 0)
	{
		strncat_s(path, size, "\\", _TRUNCATE);
		strncat_s(path, size, dirName, _TRUNCATE);
	}

	LPSTR ptr = NULL;

	ptr = strstr(path, "\\Roaming");
	if (ptr)
	{
		ptr++;
		ptr = strchr(ptr, '\\');
		if (!ptr)
		{
			// Roaming�t�H���_�ł���
			goto end;
		} else
		{
			ptr++;
			ptr = strchr(ptr, '\\');
			if (!ptr)
			{
				// Roaming�����̃t�H���_�ł���
				goto end;

			} else
			{
				// �p�X����Roaming�����̃t�H���_�Ɏw�肷��
				*ptr = '\0';
				goto end;
			}
		}
	}

end:
	//dbg
	CHAR dbgMsg[1024] = {};
	sprintf_s(dbgMsg, "[DEBUG]%d::%s %s", __LINE__, __FUNCTION__, path);
	OutputDebugStringA(dbgMsg);

	return 0;
}

int main()
{

	CHAR        trgtDirPath[MAX_PATH] = {};
	const CHAR  trgtDirName[MAX_PATH] = "test\\test\\test\\\test\\test";
	FILETIME    fDirTime              = {};
	FILETIME    fLocalTime            = {};
	SYSTEMTIME  sDirTime              = {};
	SYSTEMTIME  sLocalTime            = {};
	WIN32_FIND_DATAA wfd              = {};

	if (GetDirectoryOnDirectlyBelowTheRoaming(trgtDirPath, trgtDirName, _countof(trgtDirPath)) != 0)
	{
		sprintf_s(trgtDirPath, ".\\Release");
	}

	std::cout << "�Ώۃf�B���N�g��: " << trgtDirPath << std::endl << std::endl;
	
	// �t�H���_�̍쐬�����擾
	// ���E����
	HANDLE hFind = FindFirstFileA(trgtDirPath, &wfd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		std::cout << "[ERROR]" << __LINE__ << "::FindFirstFileA is Failed (" << GetLastError() << ")" << std::endl;
		goto end;
	}
	FindClose(hFind);
	// FILETIME�o��
	if (!OutPutFileTime(wfd.ftCreationTime, "�Ώۃf�B���N�g���쐬����FILETIME(UTC)"))
	{
		std::cout << "[ERROR]" << __LINE__ << "::OutPutFileTime is Failed" << std::endl;
		goto end;
	}
	// FILETIME>>SYSTEMTIME�ϊ�
	if (0 == FileTimeToSystemTime(&wfd.ftCreationTime, &sDirTime))
	{
		std::cout << "[ERROR]" << __LINE__ << "::FileTimeToSystemTime is Failed" << std::endl;
		goto end;
	}
	// SYSTEMTIME�o��
	if (!OutPutSystemTime(&sDirTime, "�Ώۃf�B���N�g���쐬����SYSTEMTIME(UTC)", FALSE))
	{
		std::cout << "[ERROR]" << __LINE__ << "::OutPutSystemTime is Failed" << std::endl;
		goto end;
	}
	
	// ���{���ԕϊ�
	if (0 == FileTimeToLocalFileTime(&wfd.ftCreationTime, &fDirTime))
	{
		std::cout << "[ERROR]" << __LINE__ << "::FileTimeToLocalFileTime is Failed" << std::endl;
		goto end;
	}
	// FILETIME�o��
	if (!OutPutFileTime(fDirTime, "�Ώۃf�B���N�g���쐬����FILETIME(JST)"))
	{
		std::cout << "[ERROR]" << __LINE__ << "::OutPutFileTime is Failed" << std::endl;
		goto end;
	}
	// FILETIME>>SYSTEMTIME�ϊ�
	if (0 == FileTimeToSystemTime(&fDirTime, &sDirTime))
	{
		std::cout << "[ERROR]" << __LINE__ << "::FileTimeToSystemTime is Failed" << std::endl;
		goto end;
	}
	// SYSTEMTIME�o��
	if (!OutPutSystemTime(&sDirTime, "�Ώۃf�B���N�g���쐬����SYSTEMTIME(JST)", FALSE))
	{
		std::cout << "[ERROR]" << __LINE__ << "::OutPutSystemTime is Failed" << std::endl;
		goto end;
	}
	
	// ���[�J���^�C���̎擾
	if (!OutPutSystemTime(&sLocalTime, "���ݓ���SYSTEMTIME(JST)", TRUE))
	{
		std::cout << "[ERROR]" << __LINE__ << "::OutPutSystemTime is Failed" << std::endl;
		goto end;
	}
	// SYSTEMTIME>>FILETIME�ϊ�
	if (0 == SystemTimeToFileTime(&sLocalTime, &fLocalTime))
	{
		std::cout << "[ERROR]" << __LINE__ << "::SystemTimeToFileTime is Failed" << std::endl;
		goto end;
	}
	// FILETIME�o��
	if (!OutPutFileTime(fLocalTime, "���ݓ���FILETIME(JST)"))
	{
		std::cout << "[ERROR]" << __LINE__ << "::OutPutFileTime is Failed" << std::endl;
		goto end;
	}
	
	__int64 i64Gap = 10;
	if (GetGapFileTime(fLocalTime, fDirTime) <= i64Gap)
	{
		std::cout << "����" << i64Gap << "���ȓ�" << std::endl;
	}

end:	
	std::cout << "Please enter any keys.." << std::endl;
	getchar();
	return 0;
}