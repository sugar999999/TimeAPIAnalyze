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

	std::cout << "時間差(分): " << i64GapTime / 10000 / 1000 / 60 << std::endl << std::endl;
	
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

	std::cout << "対象ディレクトリ: " << targetDir << std::endl << std::endl;
	
	// フォルダの作成日時取得
	// 世界時間
	HANDLE hFind = FindFirstFileA(targetDir, &wfd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		std::cout << "[ERROR]" << __LINE__ << "::FindFirstFileA is Failed (" << GetLastError() << ")" << std::endl;
		return 1;
	}
	FindClose(hFind);
	// FILETIME出力
	if (!OutPutFileTime(wfd.ftCreationTime, "対象ディレクトリ作成日時FILETIME(UTC)"))
	{
		std::cout << "[ERROR]" << __LINE__ << "::OutPutFileTime is Failed" << std::endl;
		return 1;
	}
	// FILETIME>>SYSTEMTIME変換
	if (0 == FileTimeToSystemTime(&wfd.ftCreationTime, &sDirTime))
	{
		std::cout << "[ERROR]" << __LINE__ << "::FileTimeToSystemTime is Failed" << std::endl;
		return 1;
	}
	// SYSTEMTIME出力
	if (!OutPutSystemTime(&sDirTime, "対象ディレクトリ作成日時SYSTEMTIME(UTC)", FALSE))
	{
		std::cout << "[ERROR]" << __LINE__ << "::OutPutSystemTime is Failed" << std::endl;
		return 1;
	}
	
	// 日本時間変換
	if (0 == FileTimeToLocalFileTime(&wfd.ftCreationTime, &fDirTime))
	{
		std::cout << "[ERROR]" << __LINE__ << "::FileTimeToLocalFileTime is Failed" << std::endl;
		return 1;
	}
	// FILETIME出力
	if (!OutPutFileTime(fDirTime, "対象ディレクトリ作成日時FILETIME(JST)"))
	{
		std::cout << "[ERROR]" << __LINE__ << "::OutPutFileTime is Failed" << std::endl;
		return 1;
	}
	// FILETIME>>SYSTEMTIME変換
	if (0 == FileTimeToSystemTime(&fDirTime, &sDirTime))
	{
		std::cout << "[ERROR]" << __LINE__ << "::FileTimeToSystemTime is Failed" << std::endl;
		return 1;
	}
	// SYSTEMTIME出力
	if (!OutPutSystemTime(&sDirTime, "対象ディレクトリ作成日時SYSTEMTIME(JST)", FALSE))
	{
		std::cout << "[ERROR]" << __LINE__ << "::OutPutSystemTime is Failed" << std::endl;
		return 1;
	}
	
	// ローカルタイムの取得
	if (!OutPutSystemTime(&sLocalTime, "現在日時SYSTEMTIME(JST)", TRUE))
	{
		std::cout << "[ERROR]" << __LINE__ << "::OutPutSystemTime is Failed" << std::endl;
		return 1;
	}
	// SYSTEMTIME>>FILETIME変換
	if (0 == SystemTimeToFileTime(&sLocalTime, &fLocalTime))
	{
		std::cout << "[ERROR]" << __LINE__ << "::SystemTimeToFileTime is Failed" << std::endl;
		return 1;
	}
	// FILETIME出力
	if (!OutPutFileTime(fLocalTime, "現在日時FILETIME(JST)"))
	{
		std::cout << "[ERROR]" << __LINE__ << "::OutPutFileTime is Failed" << std::endl;
		return 1;
	}
	
	__int64 i64Gap = 10;
	if (GetGapFileTime(fLocalTime, fDirTime) <= i64Gap)
	{
		std::cout << "時差" << i64Gap << "分以内" << std::endl;
	}
	
	std::cout << "Please enter any keys.." << std::endl;
	getchar();
	return 0;
}