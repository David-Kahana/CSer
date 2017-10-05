#include "stdafx.h"
#include "SerialPort.h"
#include <codecvt>

CSerialPort::CSerialPort(unsigned int number, wstring friendlyName): m_portNumber(number), m_friendlyName(friendlyName)
{
}

CSerialPort::CSerialPort(unsigned int number, string friendlyName) : m_portNumber(number)
{
	CA2W ca2w(friendlyName.c_str());
	m_friendlyName = ca2w;
}

CSerialPort::~CSerialPort()
{
}

bool CSerialPort::isEqual(CSerialPort& other)
{
	return ((m_portNumber == other.m_portNumber) && (m_friendlyName.compare(other.m_friendlyName) == 0));
}

bool CSerialPort::isEqual(CSerialPort* other)
{
	return ((m_portNumber == other->m_portNumber) && (m_friendlyName.compare(other->m_friendlyName) == 0));
}

bool CSerialPort::isSmaller(CSerialPort& other)
{
	return (m_portNumber < other.m_portNumber);
}

bool CSerialPort::isSmaller(CSerialPort* other)
{
	return (m_portNumber < other->m_portNumber);
}

int CSerialPort::getPortNumber()
{
	return (int)m_portNumber;
}

wstring CSerialPort::getFriendlyNameW()
{
	return m_friendlyName;
}

int CSerialPort::open()
{
	int ret = setPortName();
	if (ret != OK)
	{
		return ret;
	}
	m_hComm = CreateFileW(m_portName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,//FILE_FLAG_OVERLAPPED,
		NULL);
	if (m_hComm == INVALID_HANDLE_VALUE)
	{// error opening port; abort
		wprintf_s(L"error: openning port : %s\n", m_portName);
		return -3;
	}
	//wprintf_s(L"port %s opened!\n", m_portName);
	return OK;
}

int CSerialPort::close()
{
	if (CloseHandle(m_hComm) != TRUE)
	{
		wprintf_s(L"error: problem closing port : %s\n", m_portName);
		return -1;
	}
	//wprintf_s(L"port %s closed!\n", m_portName);
	m_hComm = INVALID_HANDLE_VALUE;
	return OK;
}

int CSerialPort::getPortSettings() //win32 side
{
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		wprintf_s(L"Port is not open or could not be opened\n");
		return -1;
	}
	SecureZeroMemory(&m_dcb, sizeof(DCB));//  Initialize the DCB structure.
	m_dcb.DCBlength = sizeof(DCB);
	BOOL fSuccess = GetCommState(m_hComm, &m_dcb);//  retrieving all current settings
	if (fSuccess != TRUE)
	{//  Handle the error.
		wprintf_s(L"GetCommState failed with error %d.\n", GetLastError());
		return -2;
	}
	//wprintf_s(L"\nBaudRate = %d, ByteSize = %d, Parity = %d, StopBits = %d\n", m_dcb.BaudRate, m_dcb.ByteSize, m_dcb.Parity, m_dcb.StopBits); //Output to console
	//wprintf_s(L"got settings for port: %s\n", m_portName);
	return OK;
}

int CSerialPort::setPortSettings(wchar_t* comSettings) //win32 side - dcb
{
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		wprintf_s(L"Port is not open or could not be opened\n");
		return -1;
	}
	if (!BuildCommDCB(comSettings, &m_dcb))
	{
		// Couldn't build the DCB. Usually a problem
		// with the communications specification string.
		wprintf_s(L"Couldn't build the DCB\n");
		return -3;
	}
	// DCB is ready for use.       
	//printf_s("DCB built\n");
	if (!SetCommState(m_hComm, &m_dcb))
	{   // Error in SetCommState. Possibly a problem with the communications 
		// port handle or a problem with the DCB structure itself.
		wprintf_s(L"Error in SetCommState\n");
		return -4;
	}	       
	return OK;
}

int CSerialPort::printPortSettings()
{
	wprintf_s(L"BaudRate = %d, ByteSize = %d, Parity = %d, StopBits = %d", m_dcb.BaudRate, m_dcb.ByteSize, m_dcb.Parity, m_dcb.StopBits); //Output to console
	return OK;
}

int CSerialPort::setPortName()
{
	if (m_portNumber >= MAX_NUMBER_OF_PORTS)
	{
		wprintf_s(L"error: port number: %d\n", (int)m_portNumber);
		return -1;
	}
	int ret = swprintf_s(m_portName, L"\\\\.\\COM");
	if (ret == -1)
	{
		wprintf_s(L"error: port name creation");
		return -2;
	}
	errno_t err = _itow_s((int)m_portNumber, &(m_portName[7]), 3, 10);
	if (err != 0)
	{
		wprintf_s(L"error: port number conversion: %d\n", (int)m_portNumber);
		return -3;
	}
	return OK;
}