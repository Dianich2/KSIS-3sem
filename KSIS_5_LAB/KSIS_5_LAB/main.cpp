#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <IPExport.h>
#include <string.h>
#include <iomanip>
#include <stdio.h>
#include <conio.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

#define IP_STATUS_BASE 11000
#define IP_SUCCESS 0 // успешно
#define IP_DEST_NET_UNREACHABLE 11002 // сеть недоступна
#define IP_DEST_HOST_UNREACHABLE 11003 // узел недоступен
#define IP_DEST_PROT_UNREACHABLE 11004 // протокол недоступен
#define IP_DEST_PORT_UNREACHABLE 11005
#define IP_REQ_TIMED_OUT 11010 // запрос истек по времени
#define IP_BAD_REQ 11011 // запрос некорректный
#define IP_BAD_ROUTE 11012 // ошибка маршрутизации
#define IP_TTL_EXPIRED_TRANSIT 11013 // время жизни пакета истекло в пути

//typedef struct {
//	unsigned char Ttl; //время доставки
//	unsigned char Tos; //тип сервиса
//	unsigned char Flags; //флаги IP заголовка
//	unsigned char OptionsSize;
//	//размер опций в байтах 
//	unsigned char* OptionsData;
//	//указатель на опции
//} IP_OPTION_INFORMATION, * PMP_OPTION_INFORMATION;


IP_OPTION_INFORMATION option = { 255, 0, 0, 0, 0 }; // опции включения в заголовок IP пакета

using namespace std;

void Ping(const char* cHost,
	unsigned int Timeout,
	unsigned int RequestCount)
{
	HANDLE hIP = IcmpCreateFile(); // создать файл сервиса
	if (hIP == INVALID_HANDLE_VALUE)
	{
		WSACleanup();
		return;
	}
	char SendData[32] = "Data for ping";
	int LostPacketsCount = 0; 
	unsigned int MaxMS = 0;
	int MinMS = -1; 

	PICMP_ECHO_REPLY pIpe =
		(PICMP_ECHO_REPLY)GlobalAlloc(GHND,
			sizeof(ICMP_ECHO_REPLY)
			+ sizeof(SendData)); // выделение памяти для буфера эхо-ответа
	if (pIpe == 0) { // если ошибка
		IcmpCloseHandle(hIP); // закрытие ICMP
		WSACleanup(); // завершение использования  Winsock 2 DLL (Ws2_32.dll).
		return;
	}
	pIpe->Data = SendData;
	pIpe->DataSize = sizeof(SendData);
	unsigned long ipaddr = inet_addr(cHost); // адрес опрашиваемого узла
	for (unsigned int c = 0; c < RequestCount; c++) {
		int dwStatus = IcmpSendEcho(hIP, // манипулятор
			ipaddr, // IP опрашиваемого узла
			SendData, // указатель на отправляемые данные
			sizeof(SendData), // размер этих данных
			&option, // указатель на структуру, которая содержит дополнительные опции запроса
			pIpe, // указатель на буфер для эхо-ответов
			sizeof(ICMP_ECHO_REPLY) + // его размер
			sizeof(SendData),
			Timeout); // время ожидания ответа
		if (dwStatus > 0) { // количество полученных эхо-ответов
			for (int i = 0; i < dwStatus; i++) {
				unsigned char* pIpPtr =
					(unsigned char*)&pIpe->Address;
				cout << "ответ от " << (int)*(pIpPtr) // вывод IP
					<< "." << (int)*(pIpPtr + 1)
					<< "." << (int)*(pIpPtr + 2)
					<< "." << (int)*(pIpPtr + 3)
					<< ": число байт =  " << pIpe->DataSize
					<< " время = " << pIpe->RoundTripTime 
					<< "мс TTL = " << (int)pIpe->Options.Ttl;
				MaxMS = (MaxMS > pIpe->RoundTripTime) ? // максимальное время ответа
					MaxMS : pIpe->RoundTripTime;
				MinMS = (MinMS < (int)pIpe->RoundTripTime // минимальное время ответа
					&& MinMS >= 0) ?
					MinMS : pIpe->RoundTripTime;
				cout << endl;
			}
			Sleep(750);
		}
		else {
			if (pIpe->Status) { // считаем число потерянных пакетов
				LostPacketsCount++;
				switch (pIpe->Status) {
				case IP_DEST_NET_UNREACHABLE:
				case IP_DEST_HOST_UNREACHABLE:
				case IP_DEST_PROT_UNREACHABLE:
				case IP_DEST_PORT_UNREACHABLE:
					cout << "Remote host may be down." << endl;
					break;
				case IP_REQ_TIMED_OUT:
					cout << "Request timed out." << endl;
					break;
				case IP_TTL_EXPIRED_TRANSIT:
					cout << "TTL expired in transit." << endl;
					break;
				default:
					cout << "Error code: %ld"
						<< pIpe->Status << endl;
					break;
				}
			}
		}
	}
	IcmpCloseHandle(hIP); // освобождение ICMP-манипулятора
	WSACleanup();
	if (MinMS < 0) MinMS = 0;
	unsigned char* pByte = (unsigned char*)&pIpe->Address;
	cout << "Статистика Ping "
		<< (int)*(pByte)
		<< "." << (int)*(pByte + 1)
		<< "." << (int)*(pByte + 2)
		<< "." << (int)*(pByte + 3) << endl;
	cout << "\tПакетов: отправлено = " << RequestCount
		<< ", получено = "
		<< RequestCount - LostPacketsCount
		<< ", потеряно = " << LostPacketsCount
		<< "<" << (int)(100 / (float)RequestCount) *
		LostPacketsCount
		<< " % потерь>, " << endl;
	cout << "Приблизительное время приема-передачи:"
		<< endl << "Минимальное = " << MinMS
		<< "мс, Максимальное = " << MaxMS
		<< "мс, Среднее = " << (MaxMS + MinMS) / 2
		<< "мс" << endl;
}
int main(int argc, char** argv)
{
	setlocale(LC_ALL, "RUS");
	string address;
	int timems;
	int countRequest;
	cin >> address;
	cin >> timems;
	cin >> countRequest;
	//Ping("26.242.156.169", 2000, 10);
	Ping(address.c_str(), timems, countRequest);
	_getch();
	return 0;
}