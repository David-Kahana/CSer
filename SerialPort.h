#pragma once

#include "FileUtil.h"
#define MAX_NUMBER_OF_PORTS 256
using namespace std;

class CSerialPort
{
public:
	CSerialPort(unsigned int number, wstring friendlyName);
	CSerialPort(unsigned int number, string friendlyName);
	~CSerialPort();
	bool isEqual(CSerialPort& other);
	bool isEqual(CSerialPort* other);
	bool isSmaller(CSerialPort& other);
	bool isSmaller(CSerialPort* other);
	int getPortNumber();
	wstring getFriendlyNameW();
	int open();
	int close();
	int getPortSettings(); //win32 side - dcb
	int setPortSettings(wchar_t* comSettings); //win32 side - dcb
	int printPortSettings(); //win32 side - dcb
private:
	UINT m_portNumber;
	wchar_t m_portName[12];
	wstring m_friendlyName;
	HANDLE m_hComm = INVALID_HANDLE_VALUE;
	DCB m_dcb;
private:
	int setPortName();
};

