#include "stdafx.h"
#include "SerialPortsMngt.h"

CSerialPortsMngt::CSerialPortsMngt()
{
    for (int i = 0; i < MAX_NUMBER_OF_PORTS; ++i)
    {
        m_currentPorts[i] = nullptr;
        m_removedPorts[i] = nullptr;
    }
}

CSerialPortsMngt::~CSerialPortsMngt()
{
}

/**********************************************************************************************//**
 * @fn  int CSerialPortsMngt::scanPorts()
 *
 * @brief   Scans the ports
 *
 * @author  David Kahana
 * @date    04/10/2017
 *
 * @return      An int.
 * @return  0   Success, task completed.
 **************************************************************************************************/
int CSerialPortsMngt::scanPorts()
{
    vector<CSerialPort*> newPorts;
    int ret = scanForAllPorts(newPorts);
    for (int i = 0; i < MAX_NUMBER_OF_PORTS; ++i)
    {
        if (m_currentPorts[i] != nullptr)
        {
            int found = findInVect(m_currentPorts[i], newPorts);
            if (found < 0)
            {
                if (m_removedPorts[i] != nullptr)
                { //should not happen
                    return -1;
                }
                m_removedPorts[i] = m_currentPorts[i];
                m_currentPorts[i] = nullptr;
            }
        }
    }
    for (int i = 0; i < (int)newPorts.size(); ++i)
    {
        CSerialPort* port = newPorts[i];
        int num = port->getPortNumber();
        if (num <= 0 || num >= MAX_NUMBER_OF_PORTS)
        {
            return -1;
        }
        if (m_currentPorts[num] == nullptr) // port not in current ports
        {
            if (m_removedPorts[num] == nullptr) // port not in removed ports
            {
                m_currentPorts[num] = port;
            }
            else
            {
                if (m_removedPorts[num]->isEqual(port))
                {
                    m_currentPorts[num] = m_removedPorts[num];
                    delete port;
                    newPorts[i] = nullptr;
                }
                else
                {
                    m_currentPorts[num] = port;
                    delete m_removedPorts[num];
                }
                m_removedPorts[num] = nullptr;
            }
        }
        else
        {
            if (!m_currentPorts[num]->isEqual(port))
            {
                delete m_currentPorts[num];
                m_currentPorts[num] = port;
            }
            else
            {
                delete port;
                newPorts[i] = nullptr;
            }
        }
    }
    return OK;
}

/**********************************************************************************************//**
 * @fn  int CSerialPortsMngt::getPortsNames(vector<UINT>& ports, vector<wstring>& friendlyNames)
 *
 * @brief   Gets ports names
 *
 * @author  David Kahana
 * @date    04/10/2017
 *
 * @param [in,out]  ports           The ports.
 * @param [in,out]  friendlyNames   List of names of the friendlies.
 *
 * @return  The ports names.
 **************************************************************************************************/
int CSerialPortsMngt::getPortsNames(vector<UINT>& ports, vector<wstring>& friendlyNames)
{
    ports.clear();
    friendlyNames.clear();
    for (int i = 0; i < MAX_NUMBER_OF_PORTS; ++i)
    {
        if (m_currentPorts[i] != nullptr)
        {
            ports.push_back(m_currentPorts[i]->getPortNumber());
            friendlyNames.push_back(m_currentPorts[i]->getFriendlyNameW());
        }
    }
    return OK;
}

int CSerialPortsMngt::listPorts()
{
    for (int i = 0; i < MAX_NUMBER_OF_PORTS; ++i)
    {
        if (m_currentPorts[i] != nullptr)
        {
            wprintf_s(L"COM%d : %s\n", m_currentPorts[i]->getPortNumber(), m_currentPorts[i]->getFriendlyNameW().c_str());
        }
    }
    return OK;
}

int CSerialPortsMngt::listPortsAndSettings()
{
	for (int i = 0; i < MAX_NUMBER_OF_PORTS; ++i)
	{
		if (m_currentPorts[i] != nullptr)
		{
			wprintf_s(L"COM%d : %s  ", m_currentPorts[i]->getPortNumber(), m_currentPorts[i]->getFriendlyNameW().c_str());
			m_currentPorts[i]->open();
			Sleep(300);
			m_currentPorts[i]->getPortSettings();
			m_currentPorts[i]->printPortSettings();
			m_currentPorts[i]->close();
			
			wprintf_s(L"\n");
		}
	}
	return OK;
}

/**********************************************************************************************//**
 * @fn CSerialPort* CSerialPortsMngt::getPortByNumber(int num)
 *
 * @brief Gets port by number
 *
 * @author David Kahana
 * @date 04/10/2017
 *
 * @param num port number.
 *
 * @return Null if it fails, else the port by number.
 **************************************************************************************************/
CSerialPort* CSerialPortsMngt::getPortByNumber(int num)
{
    if (num <= 0 || num >= MAX_NUMBER_OF_PORTS)
    {
        return nullptr;
    }
    return m_currentPorts[num];
}

int CSerialPortsMngt::findInVect(CSerialPort* port, vector<CSerialPort*>& portVect)
{
    int i = (int)portVect.size() - 1;
    while ((i >= 0) && !(portVect[i]->isEqual(port))) --i;
    return i;
}

int CSerialPortsMngt::insertInVect(CSerialPort* port, vector<CSerialPort*>& portVect)
{
    vector<CSerialPort*>::iterator iter = portVect.begin();
    while (iter != portVect.end() && ((*iter)->isSmaller(port))) iter++;
    if ((*iter)->isEqual(port)) //should not happen
    {
        return -1;
    }
    portVect.insert(iter, port);
    return OK;
}

int CSerialPortsMngt::scanForAllPorts(vector<CSerialPort*>& newPorts)
{
    newPorts.clear();
    unsigned int newPortNum = 0;
    wstring newFriendlyName;
    //DWORD dwFlags = DIGCF_PRESENT | DIGCF_DEVICEINTERFACE;
    //GUID guid = GUID_DEVINTERFACE_COMPORT;
    DWORD dwFlags = DIGCF_PRESENT;
    GUID guid = GUID_DEVINTERFACE_SERENUM_BUS_ENUMERATOR;
    //Create a "device information set" for the specified GUID
    HDEVINFO hDevInfoSet = SetupDiGetClassDevs(&guid, NULL, NULL, dwFlags);
    if (hDevInfoSet == INVALID_HANDLE_VALUE)
    {
        return -1;
    }
    //Finally do the enumeration
    BOOL bMoreItems = TRUE;
    int nIndex = 0;
    SP_DEVINFO_DATA devInfo;
    while (bMoreItems)
    {
        //Enumerate the current device
        devInfo.cbSize = sizeof(SP_DEVINFO_DATA);
        bMoreItems = SetupDiEnumDeviceInfo(hDevInfoSet, nIndex, &devInfo);
        if (bMoreItems)
        {
            //Did we find a serial port for this device
            //Get the registry key which stores the ports settings
            ATL::CRegKey deviceKey;
            deviceKey.Attach(SetupDiOpenDevRegKey(hDevInfoSet, &devInfo, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE));
            if (deviceKey != INVALID_HANDLE_VALUE)
            {
                int nPort = 0;
                if (QueryRegistryPortName(deviceKey, nPort))
                {
                    newPortNum = (UINT)nPort;
                    ATL::CHeapPtr<BYTE> byFriendlyName;
                    if (QueryDeviceDescription(hDevInfoSet, devInfo, byFriendlyName))
                    {
                        newFriendlyName = reinterpret_cast<LPCTSTR>(byFriendlyName.m_pData);
                    }
                    else
                    {
                        newFriendlyName = L"";
                    }
                    newPorts.push_back(new CSerialPort(newPortNum, newFriendlyName));
                }
            }
        }
        ++nIndex;
    }
    //Free up the "device information set" now that we are finished with it
    SetupDiDestroyDeviceInfoList(hDevInfoSet);
    return OK;
}

int CSerialPortsMngt::QueryRegistryPortName(ATL::CRegKey& deviceKey, int& nPort)
{
    //What will be the return value from the method (assume the worst)
    BOOL bAdded = FALSE;
    //Read in the name of the port
    LPTSTR pszPortName = NULL;
    if (RegQueryValueString(deviceKey, _T("PortName"), pszPortName))
    {
        //If it looks like "COMX" then
        //add it to the array which will be returned
        size_t nLen = _tcslen(pszPortName);
        if (nLen > 3)
        {
            if ((_tcsnicmp(pszPortName, _T("COM"), 3) == 0) && IsNumeric((pszPortName + 3), FALSE))
            {
                //Work out the port number
                nPort = _ttoi(pszPortName + 3);
                bAdded = TRUE;
            }
        }
        LocalFree(pszPortName);
    }
    return bAdded;
}

int CSerialPortsMngt::RegQueryValueString(ATL::CRegKey& key, LPCTSTR lpValueName, LPTSTR& pszValue)
{
    //Initialize the output parameter
    pszValue = NULL;
    //First query for the size of the registry value 
    ULONG nChars = 0;
    LSTATUS nStatus = key.QueryStringValue(lpValueName, NULL, &nChars);
    if (nStatus != ERROR_SUCCESS)
    {
        SetLastError(nStatus);
        return -1;
    }
    //Allocate enough bytes for the return value
    DWORD dwAllocatedSize = ((nChars + 1) * sizeof(TCHAR)); //+1 is to allow us to NULL terminate the data if required
    pszValue = reinterpret_cast<LPTSTR>(LocalAlloc(LMEM_FIXED, dwAllocatedSize));
    if (pszValue == NULL)
    {
        return -2;
    }
    //We will use RegQueryValueEx directly here because ATL::CRegKey::QueryStringValue does not handle non-Null terminated data 
    DWORD dwType = 0;
    ULONG nBytes = dwAllocatedSize;
    pszValue[0] = _T('\0');
    nStatus = RegQueryValueEx(key, lpValueName, NULL, &dwType, reinterpret_cast<LPBYTE>(pszValue), &nBytes);
    if (nStatus != ERROR_SUCCESS)
    {
        LocalFree(pszValue);
        pszValue = NULL;
        SetLastError(nStatus);
        return -3;
    }
    if ((dwType != REG_SZ) && (dwType != REG_EXPAND_SZ))
    {
        LocalFree(pszValue);
        pszValue = NULL;
        SetLastError(ERROR_INVALID_DATA);
        return -4;
    }
    if ((nBytes % sizeof(TCHAR)) != 0)
    {
        LocalFree(pszValue);
        pszValue = NULL;
        SetLastError(ERROR_INVALID_DATA);
        return -5;
    }
    if (pszValue[(nBytes / sizeof(TCHAR)) - 1] != _T('\0'))
    {
        //Forcibly NULL terminate the data ourselves
        pszValue[(nBytes / sizeof(TCHAR))] = _T('\0');
    }
    return TRUE;
}

int CSerialPortsMngt::QueryDeviceDescription(HDEVINFO hDevInfoSet, SP_DEVINFO_DATA& devInfo, ATL::CHeapPtr<BYTE>& byFriendlyName)
{
    DWORD dwType = 0;
    DWORD dwSize = 0;
    //Query initially to get the buffer size required
    if (!SetupDiGetDeviceRegistryProperty(hDevInfoSet, &devInfo, SPDRP_DEVICEDESC, &dwType, NULL, 0, &dwSize))
    {
        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
        {
            return FALSE;
        }
    }
    if (!byFriendlyName.Allocate(dwSize))
    {
        SetLastError(ERROR_OUTOFMEMORY);
        {
            return FALSE;
        }
    }
    return SetupDiGetDeviceRegistryProperty(hDevInfoSet, &devInfo, SPDRP_DEVICEDESC, &dwType, byFriendlyName.m_pData, dwSize, &dwSize) && (dwType == REG_SZ);
}

int CSerialPortsMngt::IsNumeric(LPCWSTR pszString, BOOL bIgnoreColon)
{
    size_t nLen = wcslen(pszString);
    if (nLen == 0)
    {
        return FALSE;
    }
    //What will be the return value from this function (assume the best)
    BOOL bNumeric = TRUE;
    for (size_t i = 0; i < nLen && bNumeric; i++)
    {
        bNumeric = (iswdigit(pszString[i]) != 0);
        if (bIgnoreColon && (pszString[i] == L':'))
        {
            bNumeric = TRUE;
        }
    }
    return bNumeric;
}