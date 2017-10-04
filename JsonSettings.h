#pragma once
#include "FileUtil.h"
#include "rapidjson\document.h"
#include "rapidjson\writer.h"
#include "rapidjson\stringbuffer.h"
#include "rapidjson\prettywriter.h" // for stringify JSON
#include "rapidjson\filewritestream.h"

using namespace std;
using namespace rapidjson;

class CJsonSettings
{
public:
	static CJsonSettings& getInstance()
	{
		static CJsonSettings instance; // Guaranteed to be destroyed.
									   // Instantiated on first use.
		return instance;
	}
public:
	~CJsonSettings();
	int saveConfig(wstring configFileName);
	int saveConfig(string configFileName);
	const string& getFileName();
private:
	CJsonSettings();
	Document m_doc;
	StringBuffer m_buffer;
	string m_settingsFileName;
};

