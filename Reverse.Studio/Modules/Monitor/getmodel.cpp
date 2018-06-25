// -----------------------// FileName: 
// ProcessInfo.h
// remarks:
// ����Ӧ�ò�ʵ�֣��еĽ��̣���ɱ����̵Ȼ�ȡ�������õ�dll�б�
// -----------------------

#pragma once
#include <vector>

struct ProInfo
{
    // �������PID
    unsigned int uPID;
    // ���������
    CString strPrceName;
    // �������·��
    CString strFullPath;
    // ����ý��̵���dll����·��
    std::vector<CString> strDLLNameArr;
};

class CProcessInfo
{
private:
    // ���������Ȩ��
    BOOL EnableDebugPrivilege (BOOL fEnable);
public:
    // ���������
    std::vector<ProInfo> strPrceInfoArr;

    CProcessInfo();
    ~CProcessInfo();

    // ��ȡ������
    void GetProcessName (void);
};



// ------------------------------------------------------------------------------------------------------------------------
// FileName: 
//     ProcessInfo.cpp
// remarks:
//      ����Ӧ�ò�ʵ�֣��еĽ��̣���ɱ����̵Ȼ�ȡ�������õ�dll�б�
// ------------------------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "ProcessInfo.h"
#include "TlHelp32.h"
#include "StrSafe.h"
#include "Psapi.h"
// ��ֹ���� error LNK2019
#pragma comment(lib, "psapi.lib")

CProcessInfo::CProcessInfo()
{

}

CProcessInfo::~CProcessInfo()
{

}

BOOL CProcessInfo::EnableDebugPrivilege(BOOL fEnable)
{  
    BOOL fOk = FALSE;   
    HANDLE hToken;

    // �õ����̵ķ�������
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES,&hToken))
    {    
        TOKEN_PRIVILEGES tp;
        tp.PrivilegeCount = 1;
        // �鿴ϵͳ��Ȩֵ������һ��LUID�ṹ�� 
        LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
        tp.Privileges[0].Attributes = fEnable ? SE_PRIVILEGE_ENABLED : 0;
        // ����/�ر� ��Ȩ
        AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
        fOk = (GetLastError() == ERROR_SUCCESS);
        CloseHandle(hToken);
    }
    else
    {
        return 0;
    }
    return(fOk);
}

void CProcessInfo::GetProcessName (void)
{
    HANDLE hProcessSnap = NULL;
    HANDLE hProcessDll = NULL;
    BOOL bRet = FALSE; 
    // ��ʼ��dwSizeΪ0����ȻProcess32Firstִ��ʧ��
    PROCESSENTRY32 pe32 = {0};
    MODULEENTRY32 me32;
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dwError;
    ProInfo proinfo;
    LPCTSTR pszFormat = TEXT("��ʼ����ʱ��������! %s");

    // ����һ�����̿���

    if(!EnableDebugPrivilege(1))
    {
        MessageBox(NULL, _T("��Ȩʧ�ܣ�"), _T("��ʾ"), MB_OK|MB_ICONEXCLAMATION);
    }

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        dwError = GetLastError();
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER|
            FORMAT_MESSAGE_FROM_SYSTEM|
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            dwError,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            LPTSTR(&lpMsgBuf),
            0,
            NULL);

        lpDisplayBuf = (LPVOID)LocalAlloc(
            LMEM_ZEROINIT,
            (lstrlen((LPCTSTR)lpMsgBuf)+lstrlen(pszFormat))*sizeof(TCHAR));

        // ��ʽ���ַ���
        StringCchPrintf(
            (LPTSTR)lpDisplayBuf,
            LocalSize(lpDisplayBuf),            // �ֽ���
            pszFormat,
            lpMsgBuf);

        CString strTemp;
        strTemp.Format(TEXT("�������Ϊ:%d"), dwError);
        ::MessageBox(NULL, (LPCTSTR)lpDisplayBuf, strTemp, MB_OK|MB_ICONEXCLAMATION);
        // ���������ڴ�
        LocalFree(lpMsgBuf);
        LocalFree(lpDisplayBuf);

        return;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32); 

    Module32First(hProcessSnap, &me32);

    if (Process32First(hProcessSnap, &pe32)) 
    { 
        do 
        {     
            WCHAR path[MAX_PATH]={0};

            proinfo.uPID = pe32.th32ProcessID;
            proinfo.strPrceName = pe32.szExeFile;

            HMODULE hModule;
            HANDLE hProcess;
            DWORD needed;
            hProcess=OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, pe32.th32ProcessID); 
            if (hProcess) 
            {
                // ö�ٽ���
                EnumProcessModules(hProcess, &hModule, sizeof(hModule), &needed); 
                // ��ȡ���̵�ȫ·��
                GetModuleFileNameEx(hProcess, hModule, path, sizeof(path));
                // ����·��
                proinfo.strFullPath = path;
            }
            else
            {
                proinfo.strFullPath = _T("�޷���ý���·��");
            }
            strPrceInfoArr.push_back(proinfo);
        } 
        while (Process32Next(hProcessSnap, &pe32)); 
    } 

    std::vector<ProInfo>::iterator iter;
    for (iter = strPrceInfoArr.begin(); iter != strPrceInfoArr.end(); iter++)
    {
        // ��ȡ�ý��̵Ŀ���
        hProcessDll = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, iter->uPID);
        me32.dwSize = sizeof(MODULEENTRY32);
        if (!Module32First(hProcessDll, &me32 ) || iter->uPID==0)
        {
            continue;
        }
        do
        {  
            iter->strDLLNameArr.push_back(me32.szExePath);
        } 
        while( Module32Next(hProcessDll, &me32));
    }

    // �ر���Ȩ
    EnableDebugPrivilege(0);
    // �ر��ں˶���
    CloseHandle(hProcessSnap ); 
}

