#pragma once

#include "SerialPortSettings.h"
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
	int getPortCapabilities();
	CSerialPortSettings& getPortSettingsRef();
	CSerialPortSettings* getPortSettingsPtr();
	int openPort();
	int closePort();
	int getPortSettings(); //win32 side
	int toJsonObject(Document& jsonDoc, Value& portJsonObj);
private:
	UINT m_portNumber;
	wchar_t m_portName[12];
	wstring m_friendlyName;
	CSerialPortSettings m_portSettings;
	HANDLE m_hComm;
	DCB m_dcb;
private:
	int setPortName();
};

