#pragma once

//#include "JsonSettings.h"
#include "FileUtil.h"
using namespace std;

enum SerialProps
{
	BAUD_RATE = 0,
	PARITY = 1,
	STOP_BITS = 2,
	FLOW_CONTROL = 3,
	DATA_BITS = 4,
	LAST_PROP = 5
};

class CSerialPortSettings
{
#pragma region Constants
public:
	static const vector<const char*> propsNames;

	static const vector<wstring> baudRatesStrW;
	static const vector<string> baudRatesStr;
	static const vector<DWORD> baudRates;
	static const vector<DWORD> baudRatesDCB;

	static const vector<string> dataBitsStrings;
	static const vector<wstring> dataBitsStringsW;
	static const vector<string> dataBitsStringsAlt;
	static const vector<wstring> dataBitsStringsAltW;
	static const vector<WORD> dataBits;
	static const vector<BYTE> dataBitsDCB;

	static const vector<string> parityStrings;
	static const vector<wstring> parityStringsW;
	static const vector<string> parityStringsAlt;
	static const vector<wstring> parityStringsAltW;
	static const vector<WORD> parity;
	static const vector<BYTE> parityDCB;

	static const vector<string> stopBitsStrings;
	static const vector<wstring> stopBitsStringsW;
	static const vector<string> stopBitsStringsAlt;
	static const vector<wstring> stopBitsStringsAltW;
	static const vector<WORD> stopBits;
	static const vector<BYTE> stopBitsDCB;
	static const vector<float> stopBitsNum;

	static const vector<string> flowControlStrings;
	static const vector<string> flowControlStringsAlt;
	static const vector<wstring> flowControlStringsW;
	static const vector<wstring> flowControlStringsAltW;
	static const vector<string> flowControlDCBStrings;
	static const vector<wstring> flowControlDCBStringsW;
#pragma endregion	
	
public:
	CSerialPortSettings();
	~CSerialPortSettings();
	int setSettables(_COMMPROP& comProp);
	int clearSettables();
	const vector<unsigned char>& settableProps(SerialProps prop) const { return m_settablePropIndex[prop]; }
	void setPropIndex(SerialProps prop, unsigned char index);
	unsigned char getPropIndex(SerialProps prop);
	int props2DCB(DCB& dcb);
	//int toJsonObject(Document& jsonDoc, Value& portJsonObj);
private:
	static int copyUCHARVector(vector<unsigned char>& src, vector<unsigned char>& dst);
	int setSettableBaudRates(DWORD SettableBaud);
	int setSettableParities(WORD SettableStopParity);
	int setSettableStopBits(WORD SettableStopParity);
	int setSettableFlowControls(DWORD ProvCapabilities);
	int setSettableDataBits(WORD SettableData);
private:
	unsigned char m_propertiesIndex[LAST_PROP];
	vector<unsigned char> m_settablePropIndex[LAST_PROP];
	bool localEchoEnabled;
	//Value m_names[LAST_PROP];
};

