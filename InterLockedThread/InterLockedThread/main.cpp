#include <iostream>
#include <process.h>
#include <windows.h>

/*1.增减操作
    LONG__cdecl InterlockedIncrement(LONG volatile* Addend);
    LONG__cdecl InterlockedDecrement(LONG volatile* Addend);
    
    返回变量执行增减操作之后的值。返回运算后的值，注意：  加个负数就是减。
    LONG__cdec InterlockedExchangeAdd(LONG volatile* Addend,  LONG Value);

    2.赋值操作
    Value就是新值，函数会返回原先的值。
    LONG__cdeclInterlockedExchange(LONG volatile* Target, LONG Value);
*/

volatile long g_count = 0;
const int threadNum = 50;

unsigned int __stdcall ThreadRun(void *pM){
    Sleep(100);
    /* 自增原子操作 */
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
        std::cout<<"登录的总次数: "<<g_count<<std::endl;
    }

    getchar();
    return 0;
}

