#include <iostream>
#include <process.h>
#include <windows.h>

const int g_bufferNum = 4;
const int g_resourceNum = 8;
volatile long long g_buffer[g_bufferNum];
volatile unsigned long long g_i = 0;
volatile unsigned long long g_j = 0;
HANDLE g_emptySemaphore;
HANDLE g_fullSemaphore;
CRITICAL_SECTION g_criticalSection;

BOOL SetConsoleColor(WORD wAttributes){
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hConsole == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	return SetConsoleTextAttribute(hConsole, wAttributes);
}

unsigned int __stdcall consumerThread(void* pM){
	int number = *(int*)pM;
	bool finish = false;
	while(!finish)
	{
		WaitForSingleObject(g_fullSemaphore, INFINITE);
		EnterCriticalSection(&g_criticalSection);
		SetConsoleColor(FOREGROUND_GREEN);
		printf("消费者%d,从缓冲区%d取走资源数据%d\n", number, g_j, g_buffer[g_j]);

		if (g_buffer[g_j] == g_resourceNum)
		{
			LeaveCriticalSection(&g_criticalSection);
			ReleaseSemaphore(g_fullSemaphore, 1, NULL);
			printf("消费者%d运行结束\n", number);
			break;
		}
		InterlockedIncrement(&g_j);
		InterlockedExchange(&g_j, g_j % g_bufferNum);
		LeaveCriticalSection(&g_criticalSection);
		ReleaseSemaphore(g_emptySemaphore, 1, NULL);
	}
	return 0;
}

unsigned int __stdcall producerThread(void *pM){
	for (int i = 0; i < g_resourceNum; i++)
	{
		WaitForSingleObject(g_emptySemaphore, INFINITE);
		EnterCriticalSection(&g_criticalSection);
		SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);  
		printf("生产者向缓冲区%d投放资源%d\n", g_i, i+1);
		g_buffer[g_i] = i + 1;
		InterlockedIncrement(&g_i);
		InterlockedExchange(&g_i, g_i % g_bufferNum);
		LeaveCriticalSection(&g_criticalSection);
		ReleaseSemaphore(g_fullSemaphore, 1, NULL);
	}
	return 0;
}

int main(){
	InitializeCriticalSection(&g_criticalSection);
	g_emptySemaphore = CreateSemaphore(NULL, g_bufferNum, g_bufferNum, NULL);
	g_fullSemaphore  = CreateSemaphore(NULL, 0, g_bufferNum, NULL);
	HANDLE handle[3];
	int consumer1 = 1;
	int consumer2 = 2;
	handle[0] = (HANDLE)_beginthreadex(NULL, 0, producerThread, NULL, true, NULL);
	handle[1] = (HANDLE)_beginthreadex(NULL, 0, consumerThread, &consumer1, true, NULL);
	handle[2] = (HANDLE)_beginthreadex(NULL, 0, consumerThread, &consumer2, true, NULL);
	WaitForMultipleObjects(3, handle, true, INFINITE);
	DeleteCriticalSection(&g_criticalSection);
	CloseHandle(g_emptySemaphore);
	CloseHandle(g_fullSemaphore);
	CloseHandle(handle[0]);
	CloseHandle(handle[1]);
	CloseHandle(handle[2]);
	getchar();
	return 0;
}