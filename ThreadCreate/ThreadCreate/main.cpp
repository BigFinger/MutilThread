#include <stdio.h>
#include <process.h>
#include <windows.h>

/**
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
	int count = *((char*)arg + sizeof(int));
	printf("线程%d启动，计数:%d\n", number, count);
	return 0;
}

int main(){
	static int g_count = 0;
	char arg[10] = {0};
	printf("主线程启动\n");
	LPDWORD threadId = 0;
	HANDLE createHandle = CreateThread(NULL, 0, CreateThreadRun, NULL, 0, threadId);
	HANDLE beginHandle = (HANDLE)_beginthreadex(NULL,0, BeginThreadRun, NULL, 0, NULL);
	WaitForSingleObject(createHandle, INFINITE);
	WaitForSingleObject(beginHandle, INFINITE);
	HANDLE countHandle[10];
	for (int i = 0; i < 10; i++)
	{
		g_count++;
		memset(arg, 0, sizeof(char) * 10);
		memcpy(arg, &i, sizeof(int));
		memcpy(arg + sizeof(int), &(g_count), sizeof(int));
		int number = *(int*)arg;
		int count = *((char*)arg + sizeof(int));
		countHandle[i] = (HANDLE)_beginthreadex(NULL, 0, CountThreadRun, arg, 0, NULL);
	}
	WaitForMultipleObjects(10, countHandle, TRUE, INFINITE);
	for(int i = 0; i < threadNum; i++)
	{
		CloseHandle(countHandle[i]);
	}
	getchar();
	return 0;
}