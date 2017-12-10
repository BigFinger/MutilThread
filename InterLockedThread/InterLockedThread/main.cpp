#include <iostream>
#include <process.h>
#include <windows.h>

/*1.��������
    LONG__cdecl InterlockedIncrement(LONG volatile* Addend);
    LONG__cdecl InterlockedDecrement(LONG volatile* Addend);
    
    ���ر���ִ����������֮���ֵ������������ֵ��ע�⣺  �Ӹ��������Ǽ���
    LONG__cdec InterlockedExchangeAdd(LONG volatile* Addend,  LONG Value);

    2.��ֵ����
    Value������ֵ�������᷵��ԭ�ȵ�ֵ��
    LONG__cdeclInterlockedExchange(LONG volatile* Target, LONG Value);
*/

volatile long g_count = 0;
const int threadNum = 50;

unsigned int __stdcall ThreadRun(void *pM){
    Sleep(100);
    /* ����ԭ�Ӳ��� */
    InterlockedIncrement((LPLONG)&g_count);
    Sleep(50);
    return 0;
}

int main(){
    HANDLE threadHandle[threadNum];
    int loginNum = 20;
    while((loginNum--) > 0)
    {
        g_count = 0;
        for (int i = 0; i < threadNum; i++)
        {
            threadHandle[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadRun, NULL, 0, NULL);
        }
        WaitForMultipleObjects(threadNum, threadHandle, true, INFINITE);
        std::cout<<"��¼���ܴ���: "<<g_count<<std::endl;
    }

    getchar();
    return 0;
}

