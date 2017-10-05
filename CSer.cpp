// CSer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "SerialPortsMngt.h"
#define DEFAULT_COM_CONFIG "9600,n,8,1"

int main()
{
	CSerialPortsMngt ports;
	int ret = ports.scanPorts();
	printf_s("scan ports %s\n", ret == OK ? "OK" : "FAILED");
    // show all available COM ports
    ret = ports.listPortsAndSettings();
	//CSerialPort* port = ports.getPortByNumber(3);
	//port->openPort();
	//port->setPortSettings(CA2W(DEFAULT_COM_CONFIG));
	//port->closePort();
	//ret = ports.listPortsAndSettings();
	CSerialPort* port = nullptr;
    bool portOpened = false;
	while (!portOpened)
	{
		printf_s("\nType port number to and configuration string to connect:\nExample: 1 9600,n,8,1\nFor default configuration type anything less than 4 characters\n");
		int portNum = 0;
		char comData[128] = { 0 };
		scanf_s("%d %s", &portNum, comData, (unsigned)_countof(comData));
		if (strlen(comData) < 4)
		{
			sprintf_s(comData, DEFAULT_COM_CONFIG);
		}
		printf_s("\nport: COM%d, configuration: %s\n", portNum, comData);
		port = ports.getPortByNumber(portNum);
		if (port != nullptr)
		{
			if (port->open() == OK)
			{
				if (port->setPortSettings(CA2W(comData)) == OK)
				{
					portOpened = true;	
				}
			}
		}
	}
	wprintf_s(L"port %s opened!\n", port->getFriendlyNameW().c_str());
	Sleep(500);
	port->close();

    //    wchar_t m_portName[128] = { 0 };
    //    swprintf_s(m_portName, L"\\\\.\\COM");
    //    _itow_s(portNum, &(m_portName[7]), 3, 10);
    //    m_hComm = CreateFileW(m_portName,
    //        GENERIC_READ | GENERIC_WRITE,
    //        0,
    //        NULL,
    //        OPEN_EXISTING,
    //        0,//FILE_FLAG_OVERLAPPED,
    //        NULL);
    //    if (m_hComm == INVALID_HANDLE_VALUE)
    //    {// error opening port; abort
    //        wprintf_s(L"error: opening port : %s\n", m_portName);
    //    }
    //    wprintf_s(L"port %s opened!\n", m_portName);
    //    DCB dcb;
    //    FillMemory(&dcb, sizeof(dcb), 0);
    //    dcb.DCBlength = sizeof(dcb);
    //    if (!BuildCommDCB(CA2W(comData), &dcb))
    //    {
    //        // Couldn't build the DCB. Usually a problem
    //        // with the communications specification string.
    //        printf_s("Couldn't build the DCB\n");
    //    }
    //    else // DCB is ready for use.
    //    {
    //        printf_s("DCB built\n");
    //        if (!SetCommState(m_hComm, &dcb))
    //        {   // Error in SetCommState. Possibly a problem with the communications 
    //            // port handle or a problem with the DCB structure itself.
    //            printf_s("Error in SetCommState\n");
    //        }
    //        else
    //        {
    //            printf_s("Baudrate: %d\n", dcb.BaudRate);
    //            portOpened = true;
    //        }
    //    }
    //}
    //CloseHandle(m_hComm);
    return 0;
}

