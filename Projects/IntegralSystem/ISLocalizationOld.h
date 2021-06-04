#pragma once
#ifndef _ISLOCALIZATIONOLD_H_INCLUDED
#define _ISLOCALIZATIONOLD_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTypedefsOld.h"
//-----------------------------------------------------------------------------
class ISLocalizationOld
{
public:
	static ISLocalizationOld& Instance();

	QString GetErrorString() const;
	QString GetString(const QString &ParameterName); //�������� �������������� ������
	bool LoadResourceFile(const QString &FileName); //������������� ����� �� ��������

private:
	bool InitializeContent(const QString &Content); //�������� ��������� ����������� � �����

private:
    ISLocalizationOld();
	~ISLocalizationOld();
	ISLocalizationOld(const ISLocalizationOld&) = delete;
	ISLocalizationOld(ISLocalizationOld&&) = delete;
	ISLocalizationOld& operator=(const ISLocalizationOld&) = delete;
	ISLocalizationOld& operator=(ISLocalizationOld&&) = delete;

private:
	QString ErrorString;
	ISStringMap Dictionary;
	ISCriticalSection CriticalSection;
};
//-----------------------------------------------------------------------------
#define LANG(PARAMETER) ISLocalizationOld::Instance().GetString(PARAMETER)
//-----------------------------------------------------------------------------
#endif
