#include <stdio.h>
#include <windows.h>
#include <stdint.h>
#include <tlhelp32.h>
#include <iostream>
#include <vector>
#include <stdlib.h>

struct handle_data {
	unsigned long process_id;
	HWND best_handle;
};
 
BOOL IsMainWindow(HWND handle)
{   
	return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}

BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam)
{
	handle_data& data = *(handle_data*)lParam;
	unsigned long process_id = 0;
	GetWindowThreadProcessId(handle, &process_id);
	if (data.process_id != process_id || !IsMainWindow(handle)) {
		return TRUE;
	}
	data.best_handle = handle;
	return FALSE;   
}

HWND FindMainWindow(unsigned long process_id)
{
	handle_data data;
	data.process_id = process_id;
	data.best_handle = 0;
	EnumWindows(EnumWindowsCallback, (LPARAM)&data);
	return data.best_handle;
}

typedef struct EnumHWndsArg
{
    std::vector<HWND> *vecHWnds;
    DWORD dwProcessId;
}EnumHWndsArg, *LPEnumHWndsArg;

HANDLE GetProcessHandleByID(int nID)//通过进程ID获取进程句柄
{
    return OpenProcess(PROCESS_ALL_ACCESS, FALSE, nID);
}

DWORD GetProcessIDByName(const char* pName)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hSnapshot) {
        return NULL;
    }
    PROCESSENTRY32 pe = { sizeof(pe) };
    for (BOOL ret = Process32First(hSnapshot, &pe); ret; ret = Process32Next(hSnapshot, &pe)) {
        if (strcmp(pe.szExeFile, pName) == 0) {
            CloseHandle(hSnapshot);
            return pe.th32ProcessID;
        }
        //printf("%-6d %s\n", pe.th32ProcessID, pe.szExeFile);
    }
    CloseHandle(hSnapshot);
    return 0;
}

BOOL CALLBACK lpEnumFunc(HWND hwnd, LPARAM lParam)
{
    EnumHWndsArg *pArg = (LPEnumHWndsArg)lParam;
    DWORD  processId;
    GetWindowThreadProcessId(hwnd, &processId);
    if (processId == pArg->dwProcessId)
    {
        pArg->vecHWnds->push_back(hwnd);
        //printf("%p\n", hwnd);
    }
    return TRUE;
}

void GetHWndsByProcessID(DWORD processID, std::vector<HWND> &vecHWnds)
{
    EnumHWndsArg wi;
    wi.dwProcessId = processID;
    wi.vecHWnds = &vecHWnds;
    EnumWindows(lpEnumFunc, (LPARAM)&wi);
}

BOOL checkIsMainWindow(HWND hWndToBeCompared){
	DWORD pid = GetProcessIDByName("ZXWXStudentClient.exe");//No need for error passby
	HWND mainWindowsHandle = FindMainWindow(pid);
	if (hWndToBeCompared == mainWindowsHandle){
		return true;
	}else{
		return false;
	}
}

//枚举子窗口句柄 抄来的
BOOL CALLBACK EnumChildProc(HWND hWnd,LPARAM lParam){
    return true;
	char WindowText[512];
	if (GetWindowText(hWnd , WindowText , 512) != 0){
		if (WindowText == "智学网校" && checkIsMainWindow(hWnd) == false){
				CloseWindow(hWnd);
				return true;
		}else{
			return false;
		}
	}else{
		return false;
	}
}

void AutoHideTaskBar(BOOL bHide){
      #ifndef   ABM_SETSTATE 
      #define   ABM_SETSTATE             0x0000000a 
      #endif

       LPARAM lParam;
       if(bHide == true)
       {
              lParam = ABS_AUTOHIDE;//自动隐藏
       }
       else
       {
              lParam = ABS_ALWAYSONTOP;//取消自动隐藏
       }

       APPBARDATA  apBar; 
       memset(&apBar,0,sizeof(apBar)); 
       apBar.cbSize  =  sizeof(apBar); 
       apBar.hWnd  =  FindWindow("Shell_TrayWnd", NULL);
       if(apBar.hWnd  !=  NULL) 
       { 
              apBar.lParam   =   lParam; 
              SHAppBarMessage(ABM_SETSTATE,&apBar);  //设置任务栏自动隐藏
       }  
}

int main(int argc, char* argv[])
{
	HWND task;
	task=FindWindow("Shell_TrayWnd",NULL);
	AutoHideTaskBar(true);//then
    AutoHideTaskBar(false);//
	ShowWindow(task,SW_HIDE);//隐藏
	ShowWindow(task,SW_SHOW);//显示
	::SetWindowPos(task,0,0,0,0,0,SWP_SHOWWINDOW);
	printf("恢复了任务栏. \n");
	
	DWORD pid = GetProcessIDByName("ZXWXStudentClient.exe");
	if (pid != NULL){
		HWND mainWindowsHandle = FindMainWindow(pid);
		printf("主窗口句柄%d \n", mainWindowsHandle);
		ShowWindow(mainWindowsHandle,SW_RESTORE);
		printf("正常化滞学网窗口.\n");
		EnumChildWindows(NULL,EnumChildProc,0);
		MessageBox(NULL,"专杀工具完成！!","即将退出程序！",MB_OK);
		exit(EXIT_SUCCESS);
	}else{
		printf("获取进程异常！是否未开启智学网校？\n");
		MessageBox(NULL,"Fatal Error!","Terminating!",MB_OK);
		exit(EXIT_SUCCESS);
	}

	//弃用方法  改用枚举
	//HWND LockWindowHandle = FindWindowExA(HWND   mainWindowsHandle, HWND   NULL, LPCSTR lpszClass, LPCSTR NULL);
	
	
	exit(EXIT_SUCCESS);
}
