#include "stdafx.h"
#include "JsonSettings.h"

CJsonSettings::CJsonSettings()
{
	m_settingsFileName = "PortSettings.json";
}

CJsonSettings::~CJsonSettings()
{
}

int CJsonSettings::saveConfig(wstring configFileName)
{

	return OK;
}

int CJsonSettings::saveConfig(string configFileName) 
{

	return OK;
}

const string& CJsonSettings::getFileName()
{
	return m_settingsFileName;
}