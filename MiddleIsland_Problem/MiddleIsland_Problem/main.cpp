#include <iostream>
#include <windows.h>
#include <process.h>

const int threadNum = 10;
HANDLE threadHandle[threadNum];

HANDLE g_islandSemaphore;
HANDLE g_smMutex;
HANDLE g_mdMutex;
HANDLE g_event;

unsigned int __stdcall StartToDestinationThread(void *pM){
	int number = *(int*)pM;
	SetEvent(g_event);
	WaitForSingleObject(g_islandSemaphore, INFINITE);
	WaitForSingleObject(g_smMutex, INFINITE);
	
	printf("小车%d，从S到中间岛M\n", number);
	ReleaseMutex(g_smMutex);
	WaitForSingleObject(g_mdMutex, INFINITE);
	printf("小车%d, 从中间岛M到D\n", number);
	ReleaseSemaphore(g_islandSemaphore, 1, NULL);
	ReleaseMutex(g_mdMutex);
	
	return 0;
}

unsigned int __stdcall DestinationToStartThread(void *pM){
	int number = *(int*)pM;
	SetEvent(g_event);
	WaitForSingleObject(g_islandSemaphore, INFINITE);
	WaitForSingleObject(g_mdMutex, INFINITE);
	
	printf("小车%d，从D到中间岛M\n", number);
	ReleaseMutex(g_mdMutex);
	WaitForSingleObject(g_smMutex, INFINITE);
	printf("小车%d, 从中间岛M到S\n", number);
	ReleaseSemaphore(g_islandSemaphore, 1, NULL);
	ReleaseMutex(g_smMutex);
	
	return 0;
}

int main(){
	g_islandSemaphore = CreateSemaphore(NULL ,2, 2, NULL);
	g_smMutex = CreateMutex(NULL, false, NULL);
	g_mdMutex = CreateMutex(NULL, false, NULL);
	g_event = CreateEvent(NULL, false, false, NULL);
	for (int i = 0; i < threadNum;)
	{
		if ((i % 2) == 0)
			threadHandle[i] = (HANDLE)_beginthreadex(NULL, 0, StartToDestinationThread, &i, true, NULL);
		else
			threadHandle[i] = (HANDLE)_beginthreadex(NULL, 0, DestinationToStartThread, &i, true, NULL);
		WaitForSingleObject(g_event, INFINITE);
		i++;
	}
	WaitForMultipleObjects(threadNum, threadHandle, true, INFINITE);
	for (int i = 0; i < threadNum; i++)
	{
		CloseHandle(threadHandle[i]);
	}
	CloseHandle(g_islandSemaphore);
	CloseHandle(g_smMutex);
	CloseHandle(g_mdMutex);
	CloseHandle(g_event);
	getchar();
	return 0;
}