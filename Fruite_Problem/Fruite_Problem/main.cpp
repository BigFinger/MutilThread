#include <iostream>
#include <process.h>
#include <windows.h>

HANDLE g_plateMutex;
HANDLE g_emptySemaphore;
HANDLE g_orangeSemaphore;
HANDLE g_appleSemaphore;
const int fruiteNum = 10;
int g_buffer[fruiteNum + 1];
int g_i, g_j;

unsigned int __stdcall sonThread(void *pM){
	while(true)
	{
		WaitForSingleObject(g_orangeSemaphore,INFINITE);
		WaitForSingleObject(g_plateMutex, INFINITE);

		if (g_buffer[g_i] == 2)
		{
			printf("儿子回去写作业了，并问妹妹是否一起\n");
			g_j = g_i;
			ReleaseSemaphore(g_appleSemaphore, 1, NULL);
			ReleaseMutex(g_plateMutex);
			break;
		}
		if (g_buffer[g_i] == 0)
			printf("儿子从盘中取走橘子\n");

		ReleaseSemaphore(g_emptySemaphore, 1, NULL);
		ReleaseMutex(g_plateMutex);
	}
	return 0;
}

unsigned int __stdcall daughterThread(void *pM){
	while(true)
	{
		WaitForSingleObject(g_appleSemaphore, INFINITE);
		WaitForSingleObject(g_plateMutex, INFINITE);

		if (g_buffer[g_j] == 2)
		{
			printf("妹妹和哥哥一起去写作业了\n");
			ReleaseSemaphore(g_orangeSemaphore, 1, NULL);
			ReleaseMutex(g_plateMutex);
			break;
		}
		if (g_buffer[g_j] == 1)
			printf("女儿从盘中取走苹果\n");

		ReleaseSemaphore(g_emptySemaphore, 1, NULL);
		ReleaseMutex(g_plateMutex);
	}
	return 0;
}

unsigned int __stdcall fatherThread(void *pM){
	for (int i = 0; i < fruiteNum; i++)
	{
		WaitForSingleObject(g_emptySemaphore, INFINITE);
		WaitForSingleObject(g_plateMutex, INFINITE);

		if((i % 2) == 0)
		{
			g_i = i;
			printf("父亲向盘子中放了一个橘子\n");
			g_buffer[i] = 0;
			ReleaseSemaphore(g_orangeSemaphore, 1, NULL);
		}
		else
		{
			g_j = i;
			printf("父亲向盘中放了一个苹果\n");
			g_buffer[i] = 1;
			ReleaseSemaphore(g_appleSemaphore, 1, NULL);
		}
		ReleaseMutex(g_plateMutex);
	}
	WaitForSingleObject(g_emptySemaphore, INFINITE);
	WaitForSingleObject(g_plateMutex, INFINITE);
	g_i = fruiteNum;
	g_buffer[g_i] = 2;
	ReleaseSemaphore(g_orangeSemaphore, 1, NULL);
	ReleaseSemaphore(g_orangeSemaphore, 1, NULL);
	ReleaseMutex(g_plateMutex);
	return 0;
}

int main(){
	g_plateMutex = CreateMutex(NULL, false, NULL);
	g_appleSemaphore = CreateSemaphore(NULL , 0, 1, NULL);
	g_orangeSemaphore = CreateSemaphore(NULL, 0, 1, NULL);
	g_emptySemaphore = CreateSemaphore(NULL, 1, 1, NULL);

	HANDLE handle[3];
	handle[0] = (HANDLE)_beginthreadex(NULL, 0, fatherThread, NULL, true, NULL);
	handle[1] = (HANDLE)_beginthreadex(NULL, 0, sonThread, NULL, true, NULL);
	handle[2] = (HANDLE)_beginthreadex(NULL, 0, daughterThread, NULL, true, NULL);

	WaitForMultipleObjects(3, handle, true, INFINITE);
	CloseHandle(g_plateMutex);
	CloseHandle(g_appleSemaphore);
	CloseHandle(g_orangeSemaphore);
	CloseHandle(g_emptySemaphore);
	getchar();
	return 0;
}