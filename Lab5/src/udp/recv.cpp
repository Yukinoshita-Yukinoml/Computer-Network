//recvΪ�����: �����Ϊ�����������ӵ�һ��
//74-76�����߳�
#include "winsock2.h"
#include <WS2tcpip.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string.h>
#include <time.h>
#include <ctime>
#include <thread>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)
using namespace std;
const int		BUF_SIZE = 64;
const int		COMMAND_LENGTH = 4;
WSADATA			wsd;			    //WSADATA����
SOCKET			sServer;		    //�������׽���
SOCKET			sClient;		    //�ͻ����׽���
SOCKADDR_IN		servAddr;		    //��������ַ
SOCKADDR_IN     clientAddr;         //�ͻ��˵�ַ
int				nAddrLen = sizeof(clientAddr);
char			bufSend[BUF_SIZE];	//�������ݻ�����
char			bufRecv[BUF_SIZE];  //�������ݻ�����
int				retVal;			    //����ֵ
char* closeSymbol = (char*)"00";  //����ͨ�ŵı�־
									// ������׽��ֵ�ַ 


void writefile(const char*, map<string, string>);
void autocheck();

int main(int argc, char* argv[])
{
	servAddr.sin_family = AF_INET;        //Э��
	servAddr.sin_port = htons(4999);      //�˿�
	inet_pton(AF_INET, "127.0.0.1", (void*)&servAddr.sin_addr.S_un.S_addr);

	// ��ʼ���׽��ֶ�̬��	
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		cout << "WSAStartup failed !" << endl;
		return 1;
	}
	// ����������׽���
	sServer = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);//SOCK_DGRAM��ζ������һ��UDP(���ݱ�socket)
	if (INVALID_SOCKET == sServer)
	{
		cout << "socket failed!" << endl;
		WSACleanup();			 //�ͷ��׽�����Դ;
		return  -1;
	}
	else
	{
		cout << "�����͵�������'0'�����ʾ������˴�UDPͨ��\n";
		cout << "Server UDP Socket init!" << endl;
		cout << "Server UDP Socket IP: 127.0.0.1" << endl;
		cout << "Server UDP Socket Port: 4999" << endl;
	}
	// �׽��ְ�IP�Ͷ˿�
	if (SOCKET_ERROR == bind(sServer, (LPSOCKADDR)&servAddr, sizeof(SOCKADDR_IN)))
	{
		cout << "bind failed!" << endl;
		closesocket(sServer);	//�رշ�����׽���
		WSACleanup();			//�ͷ��׽�����Դ;
		return -1;
	}
	else
	{
		cout << "Server UDP Socket bind IP & Port !" << endl;
	}
	// ѭ���ȴ������˿ڷ������ݣ��ӿͻ��˽������� & ��ͻ��˷�������
	while (true)
	{
		thread t(autocheck);
		//cout << "sendcheck thread\n";
		t.detach();     //���߳̿��Էŷ�����,�����̲߳�������
		bool			sendFlag;			//�Զ�����
		map<string, string> clientkey;		//��ȡԿ��csv�ļ�
		map<string, string> liscpid;		//��ȡcsv�ļ�
		string liscfilename;
		string tmpStr;
		ifstream inFile("total.csv", ios::in);
		while (getline(inFile, tmpStr))
		{
			// �ָ��ַ���
			int index = tmpStr.find(",");
			string tmppid = tmpStr.substr(0, index);
			string tmplisc = tmpStr.substr(index + 1, tmpStr.size() - 1);
			// ����map
			liscpid[tmppid] = tmplisc;
		}
		inFile.close();

		sendFlag = true;
		// ��ʼ������ռ�
		ZeroMemory(bufRecv, BUF_SIZE);
		// ���տͻ��˷��͵�buf��Ϣ
		retVal = recvfrom(sServer, bufRecv, BUF_SIZE, 0, (sockaddr*)&clientAddr, &nAddrLen);
		/* ��������Ϊ���շ�,ʹ��recvfrom()
		 * �������� socket id,�ַ���������,�����ַ���,�������Եı��ؼ���,ָ��Զ��socket��ַָ��,��ַ����
		 * ����ֵ -1������������ ���� �յ����ַ���
		 * */
		if (SOCKET_ERROR == retVal)
		{// ����ʧ����رշ���˿ͻ����׽���
			cout << "Recv Failed!" << endl;
			closesocket(sServer);	//�رշ�����׽���
			WSACleanup();			//�ͷ��׽�����Դ;
			return -1;
		}


		// ȷ�Ͽͻ��˷��͵���Ϣ

		bufRecv[retVal] = '\0';			// ���յ����һλ��Ϊ\0���������̵�����
		char str[INET_ADDRSTRLEN];
		cout << "Client IP��" << inet_ntop(AF_INET, &clientAddr.sin_addr, str, sizeof(str)) << endl;
		cout << "Data recv from Client UDP Socket: " << bufRecv << endl;

		const char* pid = "0";
		if (!strncmp(bufRecv, "HELO", COMMAND_LENGTH))
		{
			cout << "GOT HEL0\n";

			pid = &bufRecv[COMMAND_LENGTH];
			string liscname;
			//�õ�pid�Ժ�ȥѰ��total.csv�����Ӧ��lisc
			if (liscpid.count(pid) > 0)
			{
				liscname = liscpid[pid];
				const char* license = liscname.c_str();
				//�򿪶�Ӧ��LISC�ļ�
				const char* autoname = ".csv";
				char temp[40];
				strcpy_s(temp, license);
				strcat_s(temp, autoname);
				liscfilename = temp;
				ifstream inFile(liscfilename, ios::in);
				string lineStr;

				while (getline(inFile, lineStr))
				{
					// �ָ��ַ���
					int index = lineStr.find(",");
					string tick = lineStr.substr(0, index);
					string pid = lineStr.substr(index + 1, lineStr.size() - 1);
					// ����map
					clientkey[tick] = pid;
				}
				inFile.close();

				int pidlength = retVal - COMMAND_LENGTH;
				//cout << pid<< endl;
				bool keyflag = false;//����Կ���Ƿ�ɹ�
				for (map<string, string>::iterator iter = clientkey.begin(); iter != clientkey.end(); iter++)
				{
					const char* tmp = iter->second.c_str();
					if (!strcmp(tmp, "0"))
					{
						iter->second = pid;
						//cout << iter->second;

						const char* a = iter->first.c_str();
						char autoSend[10] = "TICK";
						strcat_s(autoSend, a);
						cout << "Data send to Client UDP Socket: " << autoSend;
						sendto(sServer, autoSend, strlen(autoSend), 0, (sockaddr*)&clientAddr, nAddrLen);/**/
						keyflag = true;
						break;
					}
				}
				if (!keyflag)
				{
					const char* a = "no tickets";
					char autoSend[40] = "FAIL";
					strcat_s(autoSend, a);
					cout << "Data send to Client UDP Socket: " << autoSend;
					sendto(sServer, autoSend, strlen(autoSend), 0, (sockaddr*)&clientAddr, nAddrLen);
				}
				else
				{
					const char* tmp = liscfilename.c_str();
					writefile(tmp, clientkey);
				}
				sendFlag = false;
			}
			else
			{
				const char* a = " IS INVALID";
				char autoSend[50] = "YOUR ID";
				strcat_s(autoSend, a);
				cout << "Data send to Client UDP Socket: " << autoSend;
				sendto(sServer, autoSend, strlen(autoSend), 0, (sockaddr*)&clientAddr, nAddrLen);
				sendFlag = false;
			}
		}
		else if (!strncmp(bufRecv, "GBYE", COMMAND_LENGTH))
		{
			cout << "GOT GBYE\n";
			pid = &bufRecv[COMMAND_LENGTH];
			//�õ�pid�Ժ�ҪȥѰ��total.csv�����Ӧ��lisc
			if (liscpid.count(pid) > 0)
			{
				string liscname = liscpid[pid];
				cout << liscname;
				const char* license = liscname.c_str();
				//�򿪴洢�ļ�1111111111.csv
				const char* autoname = ".csv";
				char temp[40];
				strcpy_s(temp, license);
				strcat_s(temp, autoname);
				liscfilename = temp;
				ifstream inFile(liscfilename, ios::in);
				string lineStr;

				while (getline(inFile, lineStr))
				{
					// �ָ��ַ���
					int index = lineStr.find(",");
					string tick = lineStr.substr(0, index);
					string pid = lineStr.substr(index + 1, lineStr.size() - 1);
					// ����map
					clientkey[tick] = pid;
				}
				inFile.close();
				bool findflag = false;
				for (std::map<string, string>::iterator it = clientkey.begin(); it != clientkey.end(); it++)
				{
					if (!strcmp(it->second.c_str(), pid))
					{
						findflag = true;
						it->second = "0";
						break;
					}
				}
				//�ɹ�����Կ��
				if (findflag)
				{
					const char* a = "YOU";
					char autoSend[10] = "THNX";
					strcat_s(autoSend, a);
					cout << "Data send to Client UDP Socket: " << autoSend;
					sendto(sServer, autoSend, strlen(autoSend), 0, (sockaddr*)&clientAddr, nAddrLen);

					const char* wtmp = liscfilename.c_str();
					writefile(wtmp, clientkey);
				}
				else
				{
					const char* a = " ";
					char autoSend[10] = "FAIL";
					strcat_s(autoSend, a);
					cout << "Data send to Client UDP Socket: " << autoSend;
					sendto(sServer, autoSend, strlen(autoSend), 0, (sockaddr*)&clientAddr, nAddrLen);
				}
				sendFlag = false;
			}
			else
			{
				const char* a = " IS INVALID";
				char autoSend[50] = "YOUR ID";
				strcat_s(autoSend, a);
				cout << "Data send to Client UDP Socket: " << autoSend;
				sendto(sServer, autoSend, strlen(autoSend), 0, (sockaddr*)&clientAddr, nAddrLen);
				sendFlag = false;
			}
		}
		else if (!strncmp(bufRecv, "LISC", COMMAND_LENGTH)) {
			cout << "GOT LISC\n";
			string left = &bufRecv[COMMAND_LENGTH];
			int index = left.find(",");
			string lisc = left.substr(0, index);
			string pid = left.substr(index + 1, left.size() - 1);

			const char* license = lisc.c_str();
			const char* autoname = ".csv";
			char temp[40];
			strcpy_s(temp, license);
			strcat_s(temp, autoname);
			liscfilename = temp;

			ifstream inFile(liscfilename, ios::in);
			const char* a;
			if (!inFile.is_open())
			{
				a = " IS INVALID";
			}
			else
			{
				a = "IS VALID";
				//���й���Ϣд�뵽total.csv
				FILE* fp = fopen("total.csv", "a+");//�ļ��Ѵ��� ָ��ָ���ļ�ĩβ����д
				fprintf(fp, "%s,%s\n\0", pid.c_str(), lisc.c_str());//������ļ�
				fclose(fp);
			}
			inFile.close();
			char autoSend[50] = "YOUR LICENSE";
			strcat_s(autoSend, a);
			cout << "Data send to Client UDP Socket: " << autoSend;
			sendto(sServer, autoSend, strlen(autoSend), 0, (sockaddr*)&clientAddr, nAddrLen);
			sendFlag = false;
		}

		else if (!strncmp(bufRecv, "CHCK", COMMAND_LENGTH)) {
			sendFlag = false;
			cout << "GOT CHCK\n";

			string checkpid = &bufRecv[COMMAND_LENGTH];
			//�򿪴洢�ļ�check.csv������Ϣ�洢��pidtime��
			map<string, string> pidtime;
			ifstream inFile("check.csv", ios::in);
			while (getline(inFile, tmpStr))
			{
				// �ָ��ַ���
				int index = tmpStr.find(",");
				string _pid = tmpStr.substr(0, index);
				string _time = tmpStr.substr(index + 1, tmpStr.size() - 1);
				// ����map
				pidtime[_pid] = _time;
			}
			inFile.close();

			long long now = time(0);
			ostringstream os;
			os << now;
			string result;
			istringstream is(os.str());
			is >> result;
			if (pidtime.count(checkpid) > 0)
			{
				pidtime[checkpid] = result;
			}
			else
			{
				//pidtime.insert(checkpid, pidtime[checkpid]);
				pidtime.insert(map<string, string>::value_type(checkpid, result));
			}
			//������ļ�
			writefile("check.csv", pidtime);
		}

		// ���ͻ��˷��͵�������'0'�����ʾ�ͻ���������˴�UDPͨ��		
		if (!strcmp(bufRecv, closeSymbol))
		{
			cout << "Client UDP Socket wants to finish this communication" << endl;
			break;
		}
		if (sendFlag)
		{
			// ��sendBuf��Ϣ���͵��ͻ���
			cout << "Data send to Client UDP Socket: ";
			char a[20];
			cin >> bufSend >> a;
			strcat_s(bufSend, a);
			sendto(sServer, bufSend, strlen(bufSend), 0, (sockaddr*)&clientAddr, nAddrLen);
			// ������˷��͵�������'0'�����ʾ�����������˴�UDPͨ��	
			if (!strcmp(bufSend, closeSymbol))
			{
				cout << "Server UDP Socket wants to finish this communication" << endl;
				break;
			}
		}
	}
	// �˳�
	closesocket(sServer);	//�رշ�����׽���
	WSACleanup();			//�ͷ��׽�����Դ;
	Sleep(5000);
	return 0;
}

//д�뵽csv�ļ�
void writefile(const char* myfile, map<string, string> clientkey)
{
	FILE* fp = fopen(myfile, "w");//�ļ��Ѵ��� ��ղ���д
	for (map<string, string>::iterator iter = clientkey.begin(); iter != clientkey.end(); iter++)
	{
		fprintf(fp, "%s,%s\n\0", iter->first.c_str(), iter->second.c_str());//���map���ļ�
	}
	fclose(fp);
}

//���ڼ��
void autocheck()
{
	while (true)
	{
		Sleep(900000);//ÿ15���Ӽ��һ��
		map<string, string> checktime;
		string tmpStr;
		ifstream inFile("check.csv", ios::in);
		while (getline(inFile, tmpStr))
		{
			// �ָ��ַ���
			int index = tmpStr.find(",");
			string _pid = tmpStr.substr(0, index);
			string _time = tmpStr.substr(index + 1, tmpStr.size() - 1);
			// ����map
			checktime[_pid] = _time;
		}
		inFile.close();
		long long now = time(0);
		//pid-time
		for (map<string, string>::iterator iter = checktime.begin(); iter != checktime.end(); iter++)
		{
			//������Сʱ������
			if (now - atoi(iter->second.c_str()) > 1800)
			{
				//ͬGBYE����
				//�õ�lisc
				map<string, string> getlisc;
				ifstream inFile("total.csv", ios::in);
				while (getline(inFile, tmpStr))
				{
					// �ָ��ַ���
					int index = tmpStr.find(",");
					string _pid = tmpStr.substr(0, index);
					string _lisc = tmpStr.substr(index + 1, tmpStr.size() - 1);
					// ����map
					//pid-lisc
					getlisc[_pid] = _lisc;
				}
				inFile.close();
				string tmplisc;
				if (getlisc.count(iter->first) > 0)
				{
					tmplisc = getlisc[iter->first];
				}
				//�õ�key
				const char* license = tmplisc.c_str();
				const char* autoname = ".csv";
				char temp[40];
				strcpy_s(temp, license);
				strcat_s(temp, autoname);
				map<string, string> getkey;
				ifstream inkeyFile(temp, ios::in);
				if (inkeyFile.is_open())
				{
					while (getline(inkeyFile, tmpStr))
					{
						// �ָ��ַ���
						int index = tmpStr.find(",");
						string _key = tmpStr.substr(0, index);
						string _pid = tmpStr.substr(index + 1, tmpStr.size() - 1);
						// ����map
						//key-pid
						getkey[_key] = _pid;
					}
				}
				inkeyFile.close();
				FILE* fp = fopen(temp, "w");//�ļ��Ѵ��� ��ղ���д
				for (map<string, string>::iterator it = getkey.begin(); it != getkey.end(); it++)
				{
					if (!strcmp(it->second.c_str(), iter->first.c_str()))
					{
						it->second = "0";
					}
					fprintf(fp, "%s,%s\n\0", it->second.c_str(), it->first.c_str());//���map���ļ�
				}
				fclose(fp);
			}

		}

	}
}
