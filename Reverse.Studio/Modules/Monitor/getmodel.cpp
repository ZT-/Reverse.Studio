// -----------------------// FileName: 
// ProcessInfo.h
// remarks:
// 基于应用层实现，有的进程，如杀软进程等获取不到调用的dll列表。
// -----------------------

#pragma once
#include <vector>

struct ProInfo
{
    // 保存进程PID
    unsigned int uPID;
    // 保存进程名
    CString strPrceName;
    // 保存进程路径
    CString strFullPath;
    // 保存该进程调用dll名和路径
    std::vector<CString> strDLLNameArr;
};

class CProcessInfo
{
private:
    // 这个用于提权的
    BOOL EnableDebugPrivilege (BOOL fEnable);
public:
    // 保存进程名
    std::vector<ProInfo> strPrceInfoArr;

    CProcessInfo();
    ~CProcessInfo();

    // 获取进程名
    void GetProcessName (void);
};



// ------------------------------------------------------------------------------------------------------------------------
// FileName: 
//     ProcessInfo.cpp
// remarks:
//      基于应用层实现，有的进程，如杀软进程等获取不到调用的dll列表。
// ------------------------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "ProcessInfo.h"
#include "TlHelp32.h"
#include "StrSafe.h"
#include "Psapi.h"
// 防止错误 error LNK2019
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

    // 得到进程的访问令牌
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES,&hToken))
    {    
        TOKEN_PRIVILEGES tp;
        tp.PrivilegeCount = 1;
        // 查看系统特权值并返回一个LUID结构体 
        LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
        tp.Privileges[0].Attributes = fEnable ? SE_PRIVILEGE_ENABLED : 0;
        // 启用/关闭 特权
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
    // 初始化dwSize为0，不然Process32First执行失败
    PROCESSENTRY32 pe32 = {0};
    MODULEENTRY32 me32;
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dwError;
    ProInfo proinfo;
    LPCTSTR pszFormat = TEXT("开始服务时遇到错误! %s");

    // 创建一个进程快照

    if(!EnableDebugPrivilege(1))
    {
        MessageBox(NULL, _T("提权失败！"), _T("提示"), MB_OK|MB_ICONEXCLAMATION);
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

        // 格式化字符串
        StringCchPrintf(
            (LPTSTR)lpDisplayBuf,
            LocalSize(lpDisplayBuf),            // 字节数
            pszFormat,
            lpMsgBuf);

        CString strTemp;
        strTemp.Format(TEXT("错误编码为:%d"), dwError);
        ::MessageBox(NULL, (LPCTSTR)lpDisplayBuf, strTemp, MB_OK|MB_ICONEXCLAMATION);
        // 清理分配的内存
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
                // 枚举进程
                EnumProcessModules(hProcess, &hModule, sizeof(hModule), &needed); 
                // 获取进程的全路径
                GetModuleFileNameEx(hProcess, hModule, path, sizeof(path));
                // 保存路径
                proinfo.strFullPath = path;
            }
            else
            {
                proinfo.strFullPath = _T("无法获得进程路径");
            }
            strPrceInfoArr.push_back(proinfo);
        } 
        while (Process32Next(hProcessSnap, &pe32)); 
    } 

    std::vector<ProInfo>::iterator iter;
    for (iter = strPrceInfoArr.begin(); iter != strPrceInfoArr.end(); iter++)
    {
        // 获取该进程的快照
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

    // 关闭特权
    EnableDebugPrivilege(0);
    // 关闭内核对象
    CloseHandle(hProcessSnap ); 
}

