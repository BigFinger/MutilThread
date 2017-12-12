#include <iostream>
#include <process.h>
#include <windows.h>

CRITICAL_SECTION g_subThreadSection;
HANDLE g_threadSemaphore;
const int threadNum = 10;
static volatile long g_count = 0;

unsigned int __stdcall ThreadRun(void* num){
    int number = *(int*)num + 1;
    ReleaseSemaphore(g_threadSemaphore, 1, NULL);
    EnterCriticalSection(&g_subThreadSection);
    InterlockedIncrement(&g_count);
    printf("线程ID:%d, 计数值:%d\n", number, g_count);
    LeaveCriticalSection(&g_subThreadSection);
    return 0;
}

int main(){
    InitializeCriticalSection(&g_subThreadSection);
    g_threadSemaphore = CreateSemaphore(NULL, 0, 1, NULL);
    HANDLE *threadHandle = new HANDLE[threadNum];
    for (int i =0; i < threadNum;)
    {
        threadHandle[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadRun, &i, true, NULL);
        WaitForSingleObject(g_threadSemaphore, INFINITE);
        i++;
    }
    WaitForMultipleObjects(threadNum, threadHandle, true, INFINITE);
    DeleteCriticalSection(&g_subThreadSection);
    for(int i = 0; i < threadNum; i++)
    {
        CloseHandle(threadHandle[i]);
    }
    delete[] threadHandle;
    threadHandle = NULL;
    getchar();
    return 0;
}