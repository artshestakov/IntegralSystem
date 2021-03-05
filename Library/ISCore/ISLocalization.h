#pragma once
#ifndef _ISLOCALIZATION_H_INCLUDED
#define _ISLOCALIZATION_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISLocalization
{
public:
	static ISLocalization& Instance();

	QString GetErrorString() const;
	QString GetString(const QString &ParameterName); //�������� �������������� ������
	bool LoadResourceFile(const QString &FileName); //������������� ����� �� ��������
	bool LoadFile(const std::string &FilePath); //������������ ����� � �����

private:
	bool InitializeContent(const QString &Content); //�������� ��������� ����������� � �����

private:
	ISLocalization();
	~ISLocalization();
	ISLocalization(const ISLocalization&) = delete;
	ISLocalization(ISLocalization&&) = delete;
	ISLocalization& operator=(const ISLocalization&) = delete;
	ISLocalization& operator=(ISLocalization&&) = delete;

private:
	QString ErrorString;
	ISStringMap Dictionary;
	ISStringMapSTD DictionarySTD;
	ISVectorString LoadedFiles;
	ISCriticalSection CriticalSection;
};
//-----------------------------------------------------------------------------
#define LANG(PARAMETER) ISLocalization::Instance().GetString(PARAMETER)
//-----------------------------------------------------------------------------
#endif
