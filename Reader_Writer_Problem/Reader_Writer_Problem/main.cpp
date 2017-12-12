#include <iostream>
#include <process.h>
#include <windows.h>

const int readerNum = 12;
volatile unsigned long long g_count = 0;
HANDLE g_readEvent;
HANDLE g_writeEvent;
HANDLE g_semaphore;
HANDLE g_mutex;
CRITICAL_SECTION g_coutSection;

BOOL SetConsoleColor(WORD wAttributes){  
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  
	if (hConsole == INVALID_HANDLE_VALUE)  
		return FALSE;         
	return SetConsoleTextAttribute(hConsole, wAttributes); 
}  

void ReaderPrintf(char* pContent, ...){
	va_list pArgList;
	va_start(pArgList, pContent);
	WaitForSingleObject(g_mutex,INFINITE);
	vfprintf(stdout, pContent, pArgList);
	ReleaseMutex(g_mutex);
}

void WriterPrintf(char *pszStr){  
	WaitForSingleObject(g_mutex,INFINITE); 
	SetConsoleColor(FOREGROUND_GREEN);  
	printf("%s\n", pszStr);  
	SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);  
	ReleaseMutex(g_mutex); 
}

unsigned int __stdcall readerThread(void* pM){
	int number = *(int*)pM;
	ReleaseSemaphore(g_semaphore, 1, NULL);
	ReaderPrintf("读者%d正在等待阅读\n", number);
	WaitForSingleObject(g_readEvent, INFINITE);
	EnterCriticalSection(&g_coutSection);
	InterlockedIncrement(&g_count);
	if (g_count == 1)
	{
		ResetEvent(g_writeEvent);
	}
	LeaveCriticalSection(&g_coutSection);
	SetEvent(g_readEvent);
	ReaderPrintf("读者%d开始进行阅读\n", number);
	Sleep(rand()%100);
	ReaderPrintf("读者%d结束阅读\n", number);
	EnterCriticalSection(&g_coutSection);
	InterlockedDecrement(&g_count);
	if (g_count == 0)
	{
		SetEvent(g_writeEvent);
	}
	LeaveCriticalSection(&g_coutSection);
	return 0;
}

unsigned int __stdcall writerThread(void* pM){
	WriterPrintf("写者进入等待\n");
	WaitForSingleObject(g_writeEvent, INFINITE);
	ResetEvent(g_readEvent);
	WriterPrintf("写者开始进行写作,所有读者等待\n");
	Sleep(rand()%100);
	WriterPrintf("写者结束写作\n");
	SetEvent(g_readEvent);
	return 0;
}

int main(){
	InitializeCriticalSection(&g_coutSection);
	g_mutex = CreateMutex(NULL, false, NULL);
	g_writeEvent = CreateEvent(NULL, false, false, NULL);
	g_readEvent = CreateEvent(NULL, false, true, NULL);
	g_semaphore = CreateSemaphore(NULL, 1, 1, NULL);
	HANDLE *readerHandle = new HANDLE[readerNum];
	HANDLE writerHandle;
	writerHandle = (HANDLE)_beginthreadex(NULL, 0, writerThread, NULL, true, NULL);

	for (int i = 0; i < 6;)
	{
		readerHandle[i] = (HANDLE)_beginthreadex(NULL, 0, readerThread, &i, true, NULL);
		WaitForSingleObject(g_semaphore, INFINITE);
		i++;
	}

	Sleep(50);

	for (int i = 6; i < readerNum;)
	{
		readerHandle[i] = (HANDLE)_beginthreadex(NULL, 0, readerThread, &i, true, NULL);
		WaitForSingleObject(g_semaphore, INFINITE);
		i++;
	}
	WaitForSingleObject(writerHandle, INFINITE);
	WaitForMultipleObjects(readerNum, readerHandle, true, INFINITE);
	DeleteCriticalSection(&g_coutSection);
	CloseHandle(g_mutex);
	CloseHandle(g_writeEvent);
	CloseHandle(g_readEvent);
	CloseHandle(writerHandle);
	CloseHandle(g_semaphore);
	for (int i = 0; i < readerNum; i++)
	{
		CloseHandle(readerHandle[i]);		
	}
	delete[] readerHandle;
	readerHandle = NULL;
	getchar();
	return 0;
}
