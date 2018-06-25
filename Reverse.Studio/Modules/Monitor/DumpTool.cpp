#include "stdafx.h"

#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <string.h>


//���̵ĵ�һ��ģ�鼴Ϊ���̵� ��ַ (����˳���ȡ�����ڴ�ӳ��Ĵ�С)
DWORD GetProcessBaseAndImageSize(DWORD dwPID, DWORD *dwImageSize)
{
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;

	// Take a snapshot of all modules in the specified process.
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if (hModuleSnap == INVALID_HANDLE_VALUE)
	{
		printf("CreateToolhelp32Snapshot (of modules) ,error code :%d\n", GetLastError());
		return FALSE;
	}

	// Set the size of the structure before using it.
	me32.dwSize = sizeof(MODULEENTRY32);

	// Retrieve information about the first module,
	// and exit if unsuccessful
	if (!Module32First(hModuleSnap, &me32))
	{
		printf("Module32First");  // show cause of failure
		CloseHandle(hModuleSnap);           // clean the snapshot object
		return FALSE;
	}

	//printf( "\n     Base address   = 0x%08X", (DWORD) me32.modBaseAddr );   //��һ��ģ�鼴���̻�ַ

	CloseHandle(hModuleSnap);

	if (dwImageSize != NULL)
		*dwImageSize = me32.modBaseSize;  //����ӳ���С
	return (DWORD)me32.modBaseAddr;
}

//�����н��̽��п��գ��ҳ�ָ�����̵�pid
DWORD GetProcessPid(char strPocName[])
{
	HANDLE hProcessSnap;
	HANDLE hProcess;
	PROCESSENTRY32 pe32;

	// Take a snapshot of all processes in the system.
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		printf("Failed to CreateToolhelp32Snapshot...\n");
		return FALSE;
	}

	// Set the size of the structure before using it.
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// Retrieve information about the first process,
	// and exit if unsuccessful
	if (!Process32First(hProcessSnap, &pe32))
	{
		printf("error in Process32First...\n");
		CloseHandle(hProcessSnap);          // clean the snapshot object
		return FALSE;
	}

	// Now walk the snapshot of processes, and
	// display information about each process in turn

	do
	{
		//printf("process name : %s\n",pe32.szExeFile);
		if (_stricmp(strPocName, pe32.szExeFile) == 0)  //���Դ�Сд
		{
			//printf("process name : %s\n",pe32.szExeFile);
			CloseHandle(hProcessSnap);
			return pe32.th32ProcessID;
		}

	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	return FALSE;
}

//��ʽһ��
//ͨ��PE�ļ�ͷȷ������Ĵ�С������ļ�ͷ�����޸����޷��õ������С
DWORD GetImageSize(HANDLE hProc, DWORD dwImageBase)
{
	DWORD dwRetSize;

	IMAGE_DOS_HEADER ImageDosHeader;
	IMAGE_NT_HEADERS ImageNtHeader;

	if (ReadProcessMemory(hProc, (char*)dwImageBase, &ImageDosHeader, sizeof(IMAGE_DOS_HEADER), NULL) == 0)
	{
		printf("lasterror : %d\n", GetLastError());
	}

	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)(dwImageBase + ImageDosHeader.e_lfanew);


	if (ReadProcessMemory(hProc, (char*)(pNTHeader), &ImageNtHeader, sizeof(IMAGE_NT_HEADERS), NULL) == 0)
	{
		printf("lasterror : %d\n", GetLastError());
		return FALSE;
	}

	dwRetSize = ImageNtHeader.OptionalHeader.SizeOfImage;

	return dwRetSize;
}

////��ʽ����
////ͨ��ץȡ���̿��ջ�ȡ�������С(�μ�����GetProcessBaseAndImageSize)
//DWORD GetImageSize(DWORD dwPid)
//{
//	HANDLE hMoudleSnap = INVALID_HANDLE_VALUE;
//	MODULEENTRY32 me32;
//	memset(&me32, 0, sizeof(MODULEENTRY32));
//	me32.dwSize = sizeof(MODULEENTRY32);
//	//�Խ�������ģ����п��մ���
//	hMoudleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);
//	if(hMoudleSnap == INVALID_HANDLE_VALUE)
//	{
//		printf("failed to CreateToolhelp32Snapshot of process modules...\n");
//		return FALSE;
//	}
//	//retrieve information about the first module
//	//and exit if unsucessful
//	if(!Module32First(hMoudleSnap, &me32))
//	{
//		printf("Failed to Module32First...\n");
//		CloseHandle(hMoudleSnap);
//		return FALSE;
//	}
//	CloseHandle(hMoudleSnap);
//	return me32.modBaseSize;   //���̾���Ĵ�С
//}


//�޸�dump�ļ�������ͷ�ṹ�� �ļ�ƫ�� �ļ���С ֵ ��ʹ�����ڴ�ƫ�ƣ��ڴ��С���
BOOL ModifySectionHeader(char *strDumpFileName)
{
	HANDLE hFile;
	HANDLE hFileMapping;
	LPVOID lpFileBase;
	hFile = CreateFile(strDumpFileName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("Failed to CreateFile ...\n");
		return FALSE;
	}
	//���ļ�ӳ�䵽�ڴ�
	hFileMapping = CreateFileMapping(hFile,
		NULL,
		PAGE_READWRITE,
		0, 0, NULL);
	if (hFileMapping == 0)
	{
		CloseHandle(hFile);
		printf("Failed to CreateFileMapping...\n");
		return FALSE;
	}
	lpFileBase = MapViewOfFile(hFileMapping,
		FILE_MAP_ALL_ACCESS,
		0, 0, 0);
	if (lpFileBase == 0)
	{
		CloseHandle(hFileMapping);
		CloseHandle(hFile);
		printf("Failed to MapViewOfFile...\n");
		return FALSE;
	}

	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)lpFileBase;
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)lpFileBase + pDosHeader->e_lfanew);
	DWORD dwSectionCount;
	dwSectionCount = pNtHeader->FileHeader.NumberOfSections;

	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pNtHeader + sizeof(IMAGE_NT_HEADERS));
	for (int i = 0; i < dwSectionCount; i++)
	{
		//printf("before:\nsection%d  VOffset : %x   VSize : %x   ROffset : %x    RSize : %x \n",
		//	i,
		//	pSectionHeader->VirtualAddress,
		//	pSectionHeader->Misc.VirtualSize,
		//	pSectionHeader->PointerToRawData,
		//	pSectionHeader->SizeOfRawData);
		//modify data
		//��Ϊ���ڴ���dump������ֵ��Ӧ�ñ����������ڴ�ƫ�ƺʹ�Сһ��
		pSectionHeader->PointerToRawData = pSectionHeader->VirtualAddress;
		pSectionHeader->SizeOfRawData = pSectionHeader->Misc.VirtualSize;


		pSectionHeader++;  //next section
	}
	//FlushViewOfFile()
	UnmapViewOfFile(lpFileBase);
	CloseHandle(hFileMapping);
	CloseHandle(hFile);
	return TRUE;
}



//dump process
DWORD DumpProcess(char strProcName[])
{
	DWORD dwPid;
	DWORD dwImageSize;
	DWORD dwProcBase;

	dwPid = GetProcessPid(strProcName);
	if (dwPid == FALSE)
		return FALSE;

	dwProcBase = GetProcessBaseAndImageSize(dwPid, &dwImageSize);
	if (dwProcBase == FALSE)
		return FALSE;

	printf("\nBase address   = 0x%08X    , pid = %d, ImageSize = %d ...\n", dwProcBase, dwPid, dwImageSize);   //��һ��ģ�鼴���̻�ַ


	//dump����ӳ��
	HANDLE hProc = OpenProcess(PROCESS_VM_READ, FALSE, dwPid);
	if (hProc == NULL)
	{
		printf("Failed to open %d process...\n", dwPid);
		return false;
	}

	//dwImageSize = 0;
	//dwImageSize = GetImageSize(hProc, dwProcBase);
	//if(dwImageSize == 0)
	//	return false;
	//printf("Image size:%d\n",dwImageSize);


	char *procBuff = (char*)malloc(dwImageSize);

	if (ReadProcessMemory(hProc, (char*)dwProcBase, procBuff, dwImageSize, NULL) == 0)
	{
		printf("lasterror : %d\n", GetLastError());
		return false;
	}
	char strFile[MAX_PATH] = "dump.";
	strcat(strFile, strProcName);
	FILE *fp;
	fp = fopen(strFile, "wb");
	fwrite(procBuff, dwImageSize, 1, fp);
	fclose(fp);

	if (procBuff != NULL)
		free(procBuff);

	CloseHandle(hProc);

	ModifySectionHeader(strFile);
	return true;
}


void main(int argc, char** argv)
{
	if (argc != 2)
	{
		printf("error argv...\n");
		return;
	}
	printf("dump %s...\n", argv[1]);
	DumpProcess(argv[1]);
	system("pause");
}
