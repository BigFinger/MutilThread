#include <iostream>
#include <process.h>
#include <windows.h>

SRWLOCK g_srwLock;
const int threadNum = 11;
HANDLE threadHandle[threadNum];
HANDLE g_mutex,g_event;

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
	SetEvent(g_event);
	ReaderPrintf("读者线程%d等待阅读...\n", number);
	AcquireSRWLockShared(&g_srwLock);
	ReaderPrintf("读者线程%d正在阅读\n", number);
	Sleep(50);
	ReleaseSRWLockShared(&g_srwLock);
	ReaderPrintf("读者线程%d结束阅读\n", number);
	return 0;
}

unsigned int __stdcall writerThread(void* pM){
	//SetEvent(g_event);
	WriterPrintf("写者线程正在等待...\n");
	AcquireSRWLockExclusive(&g_srwLock);
	WriterPrintf("写者线程正在写小说，所有读者等待\n");
	Sleep(500);
	WriterPrintf("写者线程结束写作\n");
	ReleaseSRWLockExclusive(&g_srwLock);
	return 0;
}

int main(){
	g_mutex = CreateMutex(NULL, false, NULL);
	g_event = CreateEvent(NULL ,false, false, NULL);
	InitializeSRWLock(&g_srwLock);
	for (int i = 1; i < 3;)
	{
		threadHandle[i] = (HANDLE)_beginthreadex(NULL, 0, readerThread, &i, true, NULL);
		WaitForSingleObject(g_event, INFINITE);
		i++;
	}
	threadHandle[0] = (HANDLE)_beginthreadex(NULL, 0, writerThread, NULL, true, NULL);
	Sleep(100);
	for (int i = 3; i < threadNum;)
	{
		threadHandle[i] = (HANDLE)_beginthreadex(NULL, 0, readerThread, &i, true, NULL);
		WaitForSingleObject(g_event, INFINITE);
		i++;
	}
	WaitForMultipleObjects(threadNum, threadHandle, true, INFINITE);
	for (int i = 0; i < threadNum; i++)
	{
		CloseHandle(threadHandle[i]);
	}
	CloseHandle(g_mutex);
	CloseHandle(g_event);
	getchar();
	return 0;
}