#include <iostream>
#include <process.h>
#include <windows.h>

HANDLE g_mutexHandle;
const char MUTEX_NAME[] = "MutilThread";
int main(){
	g_mutexHandle = CreateMutex(NULL, true, MUTEX_NAME);
	printf("�����������ɹ��������������\n");
	getchar();
	exit(0);
	ReleaseMutex(g_mutexHandle);
	printf("����������\n");
	//CloseHandle(g_mutexHandle);
	

	/*HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, TRUE, MUTEX_NAME); //�򿪻�����  
	if (hMutex == NULL)  
	{  
	    printf("�򿪻�����ʧ��\n");  
		return 0;  
	}  
	printf("�ȴ���....\n");  
	DWORD dwResult = WaitForSingleObject(hMutex, 20 * 1000); //�ȴ�������������  
	switch (dwResult)  
	{  
		case WAIT_ABANDONED:  
			printf("ӵ�л������Ľ���������ֹ\n");  
			break;  
		case WAIT_OBJECT_0:  
		    printf("�Ѿ��յ��ź�\n");  
			break;  
		case WAIT_TIMEOUT:  
			printf("�ź�δ�ڹ涨��ʱ�����͵�\n");  
			break;  
	}  
	CloseHandle(hMutex); */
	getchar();
	return 0;
}