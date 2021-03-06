#include <iostream>
#include <process.h>
#include <windows.h>
using namespace std;

volatile long g_count = 0;
const int threadNum = 10;
/* �ؼ��ֶ����� */
CRITICAL_SECTION g_MainThreadSection;
CRITICAL_SECTION g_SubThreadSection;
CONDITION_VARIABLE g_MainThreadCondition;

unsigned int _stdcall ThreadRun(void* pM){
    int number = *(int*)pM;
    WakeConditionVariable(&g_MainThreadCondition);
    //LeaveCriticalSection(&g_MainThreadSection);
    Sleep(0);
    EnterCriticalSection(&g_SubThreadSection);
    InterlockedIncrement(&g_count);
    Sleep(0);
    printf("The Thread Num is : %d and the global is %d\n", number, g_count);
    LeaveCriticalSection(&g_SubThreadSection);
    return 0;
}

int main(){
    /* ��ʼ���ؼ��ֶ� */
    InitializeCriticalSection(&g_MainThreadSection);
    InitializeCriticalSection(&g_SubThreadSection);
    InitializeConditionVariable(&g_MainThreadCondition);
    HANDLE *threadHandle = new HANDLE[threadNum];
    for (int i = 0; i < threadNum; i++)
    {
        threadHandle[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadRun,&i, true, NULL);
        //EnterCriticalSection(&g_MainThreadSection);
        SleepConditionVariableCS(&g_MainThreadCondition, &g_MainThreadSection, INFINITE);
    }
    WaitForMultipleObjects(threadNum, threadHandle, true, INFINITE);
    for(int i = 0; i < threadNum; i++)
    {
        CloseHandle(threadHandle[i]);
    }
    delete[] threadHandle;
    threadHandle = NULL;
    DeleteCriticalSection(&g_MainThreadSection);
    DeleteCriticalSection(&g_SubThreadSection);
    getchar();
    return 0;
}