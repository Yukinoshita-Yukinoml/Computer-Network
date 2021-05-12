//�����ǿͻ��ˣ���

#include "winsock2.h"
#include <WS2tcpip.h>
#include <iostream>
#include<Windows.h>
#include<time.h>
#include<thread>
#include <cassert>
#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif
#pragma comment(lib, "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
using namespace std;

static double send_time;
static SOCKET			sClient;			//�ͻ����׽���
SOCKADDR_IN		servAddr;			//��������ַ
char pid[10];
bool havegotten = false;

void helpmenu();

void sendcheck()
{
	char	bufSend[64] = { "CHCK" };	//�������ݻ�����
	strcat_s(bufSend, pid);
	cout << "send check";
	sendto(sClient, bufSend, strlen(bufSend), 0, (sockaddr*)&servAddr, sizeof(servAddr));
	while (true)
	{
		Sleep(108000000);  //��ʱ���趨Ϊ5s����һ�Σ����ڲ��ԣ�108000000Ϊ30min
		cout << "send check";
		sendto(sClient, bufSend, strlen(bufSend), 0, (sockaddr*)&servAddr, sizeof(servAddr));
	}
}
int main(int argc, char* argv[])
{
	const int		BUF_SIZE = 64;
	const int		COMMAND_LENGTH = 4;
	WSADATA			wsd;				//WSADATA����
	char			bufSend[BUF_SIZE];	//�������ݻ�����
	char			bufRecv[BUF_SIZE];  //�������ݻ�����
	int				retVal;				//����ֵ
	int temppid = (int)_getpid();
	char* closeSymbol = (char*)"00";//����ͨ�ŵı�־
									  // ���÷���˵�ַ
	servAddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", (void*)&servAddr.sin_addr.S_un.S_addr);
	servAddr.sin_port = htons((short)4999);
	int	nServAddlen = sizeof(servAddr);
	// ��ʼ���׽��ֶ�̬��
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		cout << "WSAStartup failed!" << endl;
		return -1;
	}
	// ����������׽���
	sClient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == sClient)
	{
		cout << "Socket failed !" << endl;
		WSACleanup();               //�ͷ��׽�����Դ
		return  -1;
	}
	else
	{
		cout << "ԭ���ϱ��ι����е�IDֵΪ" << temppid << " (������Կ������޸�)" << endl;
		char t[10];
		cout << "���ι����е�IDֵΪ:";
		cin >> t;
		ZeroMemory(pid, 10);
		strcat_s(pid, t);
		//cout << "�����͵�������'0 0'�����ʾ������˴�UDPͨ��\n";
		//cout << "Client UDP Socket init !" << endl;
		helpmenu();
	}
	//std::thread t(sendcheck);
	//std::cout << "sendcheck thread\n";
	//t.detach();     //���߳̿�������

	// ѭ���ȴ��������˷������� & �ӷ���˽�������
	while (true)
	{
		// ��ʼ��buf�ռ�
		ZeroMemory(bufSend, BUF_SIZE);
		// �����˷�������buf
		cout << "\nData send: ";
		cin >> bufSend;
		if (!strcmp(bufSend, closeSymbol))
		{
			cout << "Client UDP Socket wants to finish this communication" << endl;
			break;
		}
		else if (!strcmp(bufSend, "LISC"))
		{
			char a[20];
			cout << "������������֤�ţ�";
			cin >> a;
			strcat_s(bufSend, a);
			strcat_s(bufSend, ",");
			strcat_s(bufSend, pid);
		}
		else if (!strcmp(bufSend, "HELO"))
		{
			strcat_s(bufSend, pid);
			if (havegotten == true) {
				cout << "���Ѿ�ӵ������ɣ������ظ����롣" << endl;
				continue;
			}
			cout << "��������ѷ��ͣ����Ե�Ƭ��" << endl;
		}
		else if (!strcmp(bufSend, "GBYE"))
		{
			strcat_s(bufSend, pid);
			cout << "��ɹ黹�����ѷ��ͣ����Ե�Ƭ��" << endl;
		}
		else if (!strcmp(bufSend, "STOP"))
		{
			return 0;
		}
		else if (!strcmp(bufSend, "HELP"))
		{
			helpmenu();
			continue;
		}
		else
		{
			cout << "���벻���Ϲ淶������������" << endl;
			continue;
		}

		retVal = sendto(sClient, bufSend, strlen(bufSend), 0, (sockaddr*)&servAddr, nServAddlen);
		if (SOCKET_ERROR == retVal)
		{
			cout << "send failed!" << endl;
			closesocket(sClient);	//�رշ�����׽���			
			WSACleanup();		    //�ͷ��׽�����Դ
			return -1;
		}


		// �ӷ���˽�������bufRecv
		retVal = recvfrom(sClient, bufRecv, BUF_SIZE, 0, (sockaddr*)&servAddr, &nServAddlen);
		bufRecv[retVal] = '\0';
		char str[INET_ADDRSTRLEN];
		//cout << "Server IP��" << inet_ntop(AF_INET, &servAddr.sin_addr, str, sizeof(str)) << endl;
		cout << "Data recv from Server: " << bufRecv << endl;

		/*�ɹ����ȷ��*/
		if (!strncmp(bufRecv, "TICK", COMMAND_LENGTH))
		{
			cout << "˵�������ѻ�����Կ�ף�������ʹ��\n";
			havegotten = true;
			char* ticketid;
			ticketid = &bufRecv[COMMAND_LENGTH];

			thread t(sendcheck);
			//cout << "sendcheck thread\n";
			t.detach();     //���߳̿��Էŷ�����,�����̲߳�������
		}
		//�������ܾ�����
		else if (!strncmp(bufRecv, "FAIL", COMMAND_LENGTH)) {
			cout << "�ǳ���Ǹ�������֤������Կ���ѱ�ռ�ã����Ժ�����\n";

		}
		else if (!strncmp(bufRecv, "THNX", COMMAND_LENGTH)) {
			cout << "˵�������黹���Կ�ף���л����ʹ��\n";
			havegotten = false;

		}
		// ������˷��͵�������'0'�����ʾ�����������˴�UDPͨ��	
		if (!strcmp(bufRecv, closeSymbol))
		{
			cout << "Server UDP Socket wants to finish this communication" << endl;
			break;
		}

	}
	//�˳�
	closesocket(sClient);	//�رշ�����׽���
	WSACleanup();
	//�ͷ��׽�����Դ
	Sleep(5000);
	return 0;
}

void helpmenu() {
	cout << "\n������������Ҫ�������²�����С�" << endl;
	cout << "1.���������� LISC ��֤������֤��" << endl;
	cout << "2.��������� HELO �����ö�Ӧ��ɡ�" << endl;
	cout << "3.Ȼ�������� GBYE ������Ӧ��ɡ� " << endl;
	cout << "4.��������� STOP ��ȫ�˳����� " << endl;
	cout << "ʹ�ù����������������� HELP ��ð���" << endl;
}