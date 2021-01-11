#pragma once
#ifndef _ISVERSIONINFO_H_INCLUDED
#define _ISVERSIONINFO_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISVersionInfo
{
public:
	ISBuildInfoStruct Info; //���������� � ������
	ISConfigurationInfo ConfigurationInfo; //���������� � ������������

public:
	static ISVersionInfo& Instance();
	void SelectConfiguration(const QString &ConfigurationName); //������� ������������ �� �����

private:
	ISVersionInfo();
	~ISVersionInfo();
	ISVersionInfo(ISVersionInfo const &) {};
	ISVersionInfo& operator=(ISVersionInfo const&) { return *this; };

private:
	std::map<QString, ISConfigurationInfo> Configurations;
};
//-----------------------------------------------------------------------------
#endif
