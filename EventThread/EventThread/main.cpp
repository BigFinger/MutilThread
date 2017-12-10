#include <iostream>
#include <process.h>
#include <windows.h>

static long g_count = 0;
const int threadNum = 10;

CRITICAL_SECTION g_subThreadSection;
HANDLE g_mainThreadEvent;

unsigned int __stdcall threadRun(void *pM){
    int num = *(int*)pM;
    num++;
    SetEvent(g_mainThreadEvent);
    EnterCriticalSection(&g_subThreadSection);
    InterlockedIncrement(&g_count);
    printf("Thread Number: %d, Value: %d\n", num, g_count);
    LeaveCriticalSection(&g_subThreadSection);
    return 0;
}

unsigned int __stdcall fastThreadRun(void *pM){
    Sleep(10);
    printf(" %s �߳��Ѿ�����\n", (char*)pM);
    WaitForSingleObject(g_mainThreadEvent, INFINITE);
    printf(" %s �̵߳ȴ��������¼������н���\n", (char*)pM);
    return 0;
}

unsigned int __stdcall slowThreadRun(void *pM){
    Sleep(100);
    printf(" %s �߳��Ѿ�����\n", (char*)pM);
    WaitForSingleObject(g_mainThreadEvent, INFINITE);
    printf(" %s �̵߳ȴ��������¼������н���\n", (char*)pM);
    return 0;
}

int main(){
    HANDLE *threadHandle = new HANDLE[threadNum];
    InitializeCriticalSection(&g_subThreadSection);
    g_mainThreadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    for (int i = 0; i < threadNum; )
    {
        threadHandle[i] = (HANDLE)_beginthreadex(NULL, 0, threadRun, &i, true, NULL);
        WaitForSingleObject(g_mainThreadEvent, INFINITE);
        i++;
    }
    WaitForMultipleObjects(threadNum, threadHandle, true, INFINITE);
    CloseHandle(g_mainThreadEvent);
    DeleteCriticalSection(&g_subThreadSection);
    delete[] threadHandle;
    threadHandle = NULL;

    char threadName[7][MAX_PATH] = {"���߳�1", "���߳�2", "���߳�3", "���߳�4", "���߳�5", "���߳�1", "���߳�1"};
    g_mainThreadEvent = CreateEvent(NULL, false, false, NULL);
    threadHandle = new HANDLE[7];
    printf("�¼�Ϊ�Զ���λ\n");
    for (int i = 0; i < 7; i++)
    {
        if (i < 5)
        {
            threadHandle[i] = (HANDLE)_beginthreadex(NULL, 0, fastThreadRun, threadName[i], true, NULL);
        }
        else
        {
            threadHandle[i] = (HANDLE)_beginthreadex(NULL, 0, slowThreadRun, threadName[i], true, NULL);
        }
    }
    Sleep(50);
    PulseEvent(g_mainThreadEvent);
    Sleep(3000);
    printf("ʱ�䵽,���߳̽���\n");
    CloseHandle(g_mainThreadEvent);
    delete[] threadHandle;
    threadHandle = NULL;

    g_mainThreadEvent = CreateEvent(NULL, true, false, NULL);
    threadHandle = new HANDLE[7];
    printf("�¼�Ϊ�ֶ���λ\n");
    for (int i = 0; i < 7; i++)
    {
        if (i < 5)
        {
            threadHandle[i] = (HANDLE)_beginthreadex(NULL, 0, fastThreadRun, threadName[i], true, NULL);
        }
        else
        {
            threadHandle[i] = (HANDLE)_beginthreadex(NULL, 0, slowThreadRun, threadName[i], true, NULL);
        }
    }
    Sleep(50);
    PulseEvent(g_mainThreadEvent);
    Sleep(3000);
    printf("ʱ�䵽,���߳̽���\n");
    CloseHandle(g_mainThreadEvent);
    delete[] threadHandle;
    threadHandle = NULL;
    getchar();
    return 0;
}