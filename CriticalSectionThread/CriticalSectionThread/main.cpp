#include <iostream>
#include <process.h>
#include <windows.h>
using namespace std;

volatile long g_count = 0;
const int threadNum = 10;
/* 关键字段区域 */
CRITICAL_SECTION g_MainThreadSection;
CRITICAL_SECTION g_SubThreadSection;

unsigned int _stdcall ThreadRun(void* pM){
    int* number = (int*)pM;
    LeaveCriticalSection(&g_MainThreadSection);
    Sleep(0);
    EnterCriticalSection(&g_SubThreadSection);
    InterlockedIncrement(&g_count);
    Sleep(0);
    printf("The Thread Num is : %d and the global is %d\n", *number, g_count);
    LeaveCriticalSection(&g_SubThreadSection);
    return 0;
}

int main(){
    /* 初始化关键字段 */
    InitializeCriticalSection(&g_MainThreadSection);
    InitializeCriticalSection(&g_SubThreadSection);

    HANDLE *threadHandle = new HANDLE[threadNum];
    for (int i = 0; i < threadNum; i++)
    {
        EnterCriticalSection(&g_MainThreadSection);
        threadHandle[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadRun,&i, true, NULL);
    }
    WaitForMultipleObjects(threadNum, threadHandle, true, INFINITE);
    delete[] threadHandle;
    threadHandle = NULL;
    DeleteCriticalSection(&g_MainThreadSection);
    DeleteCriticalSection(&g_SubThreadSection);
    getchar();
    return 0;
}