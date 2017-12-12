#include <stdio.h>
#include <process.h>
#include <windows.h>

/**
 * @Brief: CreateThread���߳����к���
 */
DWORD WINAPI CreateThreadRun(LPVOID pM){
	printf("ͨ��CreateThread���������߳�,ID�� %d Hello World��\n", GetCurrentThreadId());
	return 0;
}

/**
 * @Brief: BeginThread���߳����к���
 */
unsigned int _stdcall BeginThreadRun(PVOID pM){
	printf("ͨ��beginthread���������߳�,ID�� %d Hello World��\n", GetCurrentThreadId());
	return 0;
}

/**
 * @Brief: BeginThread���߳����к���
 */
unsigned int _stdcall CountThreadRun(void* arg){
	int number = *(int*)arg;
	int count = *((char*)arg + sizeof(int));
	printf("�߳�%d����������:%d\n", number, count);
	return 0;
}

int main(){
	static int g_count = 0;
	char arg[10] = {0};
	printf("���߳�����\n");
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