#include "stdafx.h"
#include "SerialPort.h"
#include <codecvt>

CSerialPort::CSerialPort(unsigned int number, wstring friendlyName): m_portNumber(number), m_friendlyName(friendlyName)
{
	//int ret = setPortName();
}

CSerialPort::CSerialPort(unsigned int number, string friendlyName) : m_portNumber(number)
{
	CA2W ca2w(friendlyName.c_str());
	m_friendlyName = ca2w;
	//int ret = setPortName();
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

CSerialPortSettings& CSerialPort::getPortSettingsRef()
{
	return m_portSettings;
}

CSerialPortSettings* CSerialPort::getPortSettingsPtr()
{
	return &(m_portSettings);
}

int CSerialPort::openPort()
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
	wprintf_s(L"port %s opened!\n", m_portName);
	return OK;
}

int CSerialPort::closePort()
{
	if (CloseHandle(m_hComm) != TRUE)
	{
		wprintf_s(L"error: problem closing port : %s\n", m_portName);
		return -1;
	}
	wprintf_s(L"port %s closed!\n", m_portName);
	return OK;
}

int CSerialPort::getPortSettings() //win32 side
{
	SecureZeroMemory(&m_dcb, sizeof(DCB));//  Initialize the DCB structure.
	m_dcb.DCBlength = sizeof(DCB);
	BOOL fSuccess = GetCommState(m_hComm, &m_dcb);//  retrieving all current settings
	if (fSuccess != TRUE)
	{//  Handle the error.
		wprintf_s(L"GetCommState failed with error %d.\n", GetLastError());
		return -1;
	}

	wprintf_s(L"\nBaudRate = %d, ByteSize = %d, Parity = %d, StopBits = %d\n", m_dcb.BaudRate, m_dcb.ByteSize, m_dcb.Parity, m_dcb.StopBits); //Output to console

	wprintf_s(L"got settings for port: %s\n", m_portName);
	return OK;
}

int CSerialPort::toJsonObject(Document& jsonDoc, Value& portJsonObj)
{
	Value portName;
	std::string port_name = "COM";
	port_name += std::to_string(m_portNumber);
	portName.SetString(port_name.c_str(), jsonDoc.GetAllocator());

	Value portfName;
	using convert_type = std::codecvt_utf8<wchar_t>;
	wstring_convert<convert_type, wchar_t> converter;
	string port_fname = converter.to_bytes(m_friendlyName);
	port_fname.push_back(0);
	portfName.SetString(port_fname.c_str(), jsonDoc.GetAllocator());

	if (portJsonObj.HasMember("PortName"))
	{
		portJsonObj["PortName"] = portName;
	}
	else
	{
		portJsonObj.AddMember("PortName", portName, jsonDoc.GetAllocator());
	}
	if (portJsonObj.HasMember("PortFriendlyName"))
	{
		portJsonObj["PortFriendlyName"] = portfName;
	}
	else
	{
		portJsonObj.AddMember("PortFriendlyName", portfName, jsonDoc.GetAllocator());
	}
	int ret = m_portSettings.toJsonObject(jsonDoc, portJsonObj);
	return OK;
}

int CSerialPort::getPortCapabilities()
{
	int ret = 0;
	_COMMPROP comProp;
	HANDLE hComm;
	std::wstring portName = L"\\\\.\\COM";
	portName += std::to_wstring(m_portNumber);
	//port name //Read/Write // No Sharing // No Security// Open existing port only// Non Overlapped I/O// Null for Comm Devices
	hComm = CreateFile(portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hComm == INVALID_HANDLE_VALUE)
	{
		return -1;
	}
	ret = GetCommProperties(hComm, &comProp);
	if (ret == 0)
	{
		CloseHandle(hComm);//Closing the Serial Port
		return -2;
	}
	ret = m_portSettings.setSettables(comProp);
	CloseHandle(hComm);//Closing the Serial Port
	return ret;
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