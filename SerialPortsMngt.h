#pragma once
#include "SerialPort.h"

class CSerialPortsMngt
{
public:
	CSerialPortsMngt();
	~CSerialPortsMngt();
	int scanPorts();
	int getPortsNames(vector<UINT>& ports, vector<wstring>& friendlyNames);
	CSerialPort* getPortByNumber(int num);
	int saveJson();
	int loadJson();
private:
	static int scanForAllPorts(vector<CSerialPort*>& newPorts);
	static int QueryRegistryPortName(ATL::CRegKey& deviceKey, int& nPort);
	static int RegQueryValueString(ATL::CRegKey& key, LPCTSTR lpValueName, LPTSTR& pszValue);
	static int QueryDeviceDescription(HDEVINFO hDevInfoSet, SP_DEVINFO_DATA& devInfo, ATL::CHeapPtr<BYTE>& byFriendlyName);
	static int IsNumeric(LPCWSTR pszString, BOOL bIgnoreColon);
	int findInVect(CSerialPort* port, vector<CSerialPort*>& portVect);
	int insertInVect(CSerialPort* port, vector<CSerialPort*>& portVect);
private:
	CSerialPort* m_currentPorts[MAX_NUMBER_OF_PORTS];
	CSerialPort* m_removedPorts[MAX_NUMBER_OF_PORTS];
};

