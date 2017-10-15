#include <windows.h>
#include <iostream>

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

int main()
{

	CHAR        targetDir[MAX_PATH] = ".\\Release";
	FILETIME    fDirTime            = {};
	FILETIME    fLocalTime          = {};
	SYSTEMTIME  sDirTime            = {};
	SYSTEMTIME  sLocalTime          = {};
	WIN32_FIND_DATAA wfd            = {};

	std::cout << "�Ώۃf�B���N�g��: " << targetDir << std::endl << std::endl;
	
	// �t�H���_�̍쐬�����擾
	// ���E����
	HANDLE hFind = FindFirstFileA(targetDir, &wfd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		std::cout << "[ERROR]" << __LINE__ << "::FindFirstFileA is Failed (" << GetLastError() << ")" << std::endl;
		return 1;
	}
	FindClose(hFind);
	// FILETIME�o��
	if (!OutPutFileTime(wfd.ftCreationTime, "�Ώۃf�B���N�g���쐬����FILETIME(UTC)"))
	{
		std::cout << "[ERROR]" << __LINE__ << "::OutPutFileTime is Failed" << std::endl;
		return 1;
	}
	// FILETIME>>SYSTEMTIME�ϊ�
	if (0 == FileTimeToSystemTime(&wfd.ftCreationTime, &sDirTime))
	{
		std::cout << "[ERROR]" << __LINE__ << "::FileTimeToSystemTime is Failed" << std::endl;
		return 1;
	}
	// SYSTEMTIME�o��
	if (!OutPutSystemTime(&sDirTime, "�Ώۃf�B���N�g���쐬����SYSTEMTIME(UTC)", FALSE))
	{
		std::cout << "[ERROR]" << __LINE__ << "::OutPutSystemTime is Failed" << std::endl;
		return 1;
	}
	
	// ���{���ԕϊ�
	if (0 == FileTimeToLocalFileTime(&wfd.ftCreationTime, &fDirTime))
	{
		std::cout << "[ERROR]" << __LINE__ << "::FileTimeToLocalFileTime is Failed" << std::endl;
		return 1;
	}
	// FILETIME�o��
	if (!OutPutFileTime(fDirTime, "�Ώۃf�B���N�g���쐬����FILETIME(JST)"))
	{
		std::cout << "[ERROR]" << __LINE__ << "::OutPutFileTime is Failed" << std::endl;
		return 1;
	}
	// FILETIME>>SYSTEMTIME�ϊ�
	if (0 == FileTimeToSystemTime(&fDirTime, &sDirTime))
	{
		std::cout << "[ERROR]" << __LINE__ << "::FileTimeToSystemTime is Failed" << std::endl;
		return 1;
	}
	// SYSTEMTIME�o��
	if (!OutPutSystemTime(&sDirTime, "�Ώۃf�B���N�g���쐬����SYSTEMTIME(JST)", FALSE))
	{
		std::cout << "[ERROR]" << __LINE__ << "::OutPutSystemTime is Failed" << std::endl;
		return 1;
	}
	
	// ���[�J���^�C���̎擾
	if (!OutPutSystemTime(&sLocalTime, "���ݓ���SYSTEMTIME(JST)", TRUE))
	{
		std::cout << "[ERROR]" << __LINE__ << "::OutPutSystemTime is Failed" << std::endl;
		return 1;
	}
	// SYSTEMTIME>>FILETIME�ϊ�
	if (0 == SystemTimeToFileTime(&sLocalTime, &fLocalTime))
	{
		std::cout << "[ERROR]" << __LINE__ << "::SystemTimeToFileTime is Failed" << std::endl;
		return 1;
	}
	// FILETIME�o��
	if (!OutPutFileTime(fLocalTime, "���ݓ���FILETIME(JST)"))
	{
		std::cout << "[ERROR]" << __LINE__ << "::OutPutFileTime is Failed" << std::endl;
		return 1;
	}
	
	__int64 i64Gap = 10;
	if (GetGapFileTime(fLocalTime, fDirTime) <= i64Gap)
	{
		std::cout << "����" << i64Gap << "���ȓ�" << std::endl;
	}
	
	std::cout << "Please enter any keys.." << std::endl;
	getchar();
	return 0;
}