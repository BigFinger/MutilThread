#include <iostream>
#include <process.h>
#include <windows.h>
#include <queue>
using namespace std;

const int g_length = 10;
const int g_threadNum = 2;

queue<int> g_Queue;
HANDLE g_emptySemaphore;
HANDLE g_fullSemaphore;
HANDLE g_popMutex;
HANDLE g_event;
HANDLE g_threadHandle[g_threadNum + 1];
CRITICAL_SECTION g_readWriteSection;


BOOL SetConsoleColor(WORD wAttributes){  
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  
	if (hConsole == INVALID_HANDLE_VALUE)  
		return FALSE;         
	return SetConsoleTextAttribute(hConsole, wAttributes); 
}  

void PoperPrintf(char* pContent, ...){
	va_list pArgList;
	va_start(pArgList, pContent);
	EnterCriticalSection(&g_readWriteSection);
	vfprintf(stdout, pContent, pArgList);
	LeaveCriticalSection(&g_readWriteSection);
}

void PusherPrintf(char *pszStr){  
	EnterCriticalSection(&g_readWriteSection);
	SetConsoleColor(FOREGROUND_GREEN);  
	printf("%s\n", pszStr);  
	SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);  
	LeaveCriticalSection(&g_readWriteSection);
}

unsigned int __stdcall poperThread(void* pM){
	int number = *(int*)pM + 1;
	SetEvent(g_event);
	while(true)
	{
		WaitForSingleObject(g_fullSemaphore, INFINITE);
		WaitForSingleObject(g_popMutex, INFINITE);
		if (g_Queue.empty())
		{
			ReleaseMutex(g_popMutex);
			ReleaseSemaphore(g_fullSemaphore, 1, NULL);
			break;
		}
		int nData = g_Queue.front();
		g_Queue.pop();
		PoperPrintf("读取线程%d取出数据:%d\, 队列长度:%d\n", number, nData, g_Queue.size());
		ReleaseMutex(g_popMutex);
		ReleaseSemaphore(g_emptySemaphore, 1, NULL);
	}
	return 0;
}

unsigned int __stdcall pusherThread(void* pM){
	for (int i = 0; i < 100; i++)
	{
		WaitForSingleObject(g_emptySemaphore, INFINITE);
		WaitForSingleObject(g_popMutex, INFINITE);
		g_Queue.push(i);
		PoperPrintf("写入线程将数据%d写入队列, 队列长度:%d\n", i, g_Queue.size());
		ReleaseMutex(g_popMutex);
		ReleaseSemaphore(g_fullSemaphore, 1, NULL);
	}
	ReleaseSemaphore(g_fullSemaphore, 1, NULL);
	return 0;
}

int main(){
	g_fullSemaphore = CreateSemaphore(NULL, 0, g_length, NULL);
	g_emptySemaphore = CreateSemaphore(NULL, g_length, g_length, NULL);
	g_popMutex = CreateMutex(NULL, false, NULL);
	g_event = CreateEvent(NULL, false, false, NULL);
	InitializeCriticalSection(&g_readWriteSection);
	g_threadHandle[0] = (HANDLE)_beginthreadex(NULL, 0, pusherThread, NULL, true, NULL);
	for (int i = 0; i < g_threadNum; )
	{
		g_threadHandle[i + 1] = (HANDLE)_beginthreadex(NULL, 0, poperThread, &i, true, NULL);
		WaitForSingleObject(g_event, INFINITE);
		i++;
	}
	WaitForMultipleObjects(g_threadNum + 1, g_threadHandle, true, INFINITE);
	for (int i = 0; i < g_threadNum + 1; i++)
	{
		CloseHandle(g_threadHandle[i]);
	}
	CloseHandle(g_fullSemaphore);
	CloseHandle(g_emptySemaphore);
	CloseHandle(g_popMutex);
	CloseHandle(g_event);
	DeleteCriticalSection(&g_readWriteSection);
	getchar();
	return 0;
}