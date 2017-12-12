#include <iostream>
#include <process.h>
#include <windows.h>

const int g_productNum = 1000;
volatile unsigned long long g_Buffer;
CRITICAL_SECTION g_criticalSection;
HANDLE g_emptyEvent;
HANDLE g_fullEvent;

BOOL SetConsoleColor(WORD wAttributes){
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hConsole == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	return SetConsoleTextAttribute(hConsole, wAttributes);
}

unsigned int __stdcall producerThread(void* pM){
	for(unsigned long long i = 0; i < g_productNum; i++)
	{
		WaitForSingleObject(g_emptyEvent, INFINITE);
		EnterCriticalSection(&g_criticalSection);
		InterlockedExchange(&g_Buffer, i+1);
		printf("生产者将数据%d放入缓冲池中\n", g_Buffer);
		LeaveCriticalSection(&g_criticalSection);
		SetEvent(g_fullEvent);
	}
	return 0;
}

unsigned int __stdcall consumerThread(void* pM){
	bool finish = false;
	while(!finish)
	{
		WaitForSingleObject(g_fullEvent, INFINITE);
		EnterCriticalSection(&g_criticalSection);
		SetConsoleColor(FOREGROUND_GREEN);
		printf("消费者从缓冲区取走数据:%d\n", g_Buffer);
		if (g_Buffer == g_productNum)
		{
			finish = true;
		}
		InterlockedExchange(&g_Buffer, 0);
		SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		LeaveCriticalSection(&g_criticalSection);
		SetEvent(g_emptyEvent);
	}
	return 0;
}

int main(){
	InitializeCriticalSection(&g_criticalSection);
	g_emptyEvent = CreateEvent(NULL, false, true, NULL);
	g_fullEvent = CreateEvent(NULL, false, false, NULL);
	HANDLE handle[2];
	handle[0] = (HANDLE)_beginthreadex(NULL, 0, producerThread, NULL, true, NULL);
	handle[1] = (HANDLE)_beginthreadex(NULL, 0, consumerThread, NULL, true, NULL);
	WaitForMultipleObjects(2, handle, true, INFINITE);
	CloseHandle(g_emptyEvent);
	CloseHandle(g_fullEvent);
	CloseHandle(handle[0]);
	CloseHandle(handle[1]);
	DeleteCriticalSection(&g_criticalSection);
	getchar();
	return 0;
}
