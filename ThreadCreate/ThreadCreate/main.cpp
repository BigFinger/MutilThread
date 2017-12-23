#include <stdio.h>
#include <process.h>
#include <windows.h>

const int threadNum = 10;
int g_count = 0;
CRITICAL_SECTION g_section;

 * @Brief: CreateThread子线程运行函数
 */
DWORD WINAPI CreateThreadRun(LPVOID pM){
	printf("通过CreateThread创建的子线程,ID： %d Hello World！\n", GetCurrentThreadId());
	return 0;
}

/**
 * @Brief: BeginThread子线程运行函数
 */
unsigned int _stdcall BeginThreadRun(PVOID pM){
	printf("通过beginthread创建的子线程,ID： %d Hello World！\n", GetCurrentThreadId());
	return 0;
}

/**
 * @Brief: BeginThread子线程运行函数
 */
unsigned int _stdcall CountThreadRun(void* arg){
	int number = *(int*)arg;
    ReleaseMutex(g_orderMutex);
    //EnterCriticalSection(&g_section);
    for (int i = 0; i < 10; i++)
    {
        
        Sleep(100);
        g_count++;
    }
    //LeaveCriticalSection(&g_section);
    printf("线程%d启动，计数:%d\n", number, g_count);
    Sleep(0);
	return 0;
}

int main(){
    InitializeCriticalSection(&g_section);
	printf("主线程启动\n");
	LPDWORD threadId = 0;
	HANDLE createHandle = CreateThread(NULL, 0, CreateThreadRun, NULL, 0, threadId);
	HANDLE beginHandle = (HANDLE)_beginthreadex(NULL,0, BeginThreadRun, NULL, 0, NULL);
	WaitForSingleObject(createHandle, INFINITE);
	WaitForSingleObject(beginHandle, INFINITE);
    CloseHandle(createHandle);
    CloseHandle(beginHandle);

	HANDLE countHandle[threadNum];
	for (int i = 0; i < threadNum; )
	{
		countHandle[i] = (HANDLE)_beginthreadex(NULL, 0, CountThreadRun, &i, 0, NULL);
        i++;
    }
	WaitForMultipleObjects(threadNum, countHandle, TRUE, INFINITE);
	for(int i = 0; i < threadNum; i++)
	{
		CloseHandle(countHandle[i]);
	}
    DeleteCriticalSection(&g_section);
	getchar();
	return 0;
}