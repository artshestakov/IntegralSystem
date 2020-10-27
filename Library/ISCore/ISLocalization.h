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
	bool LoadTraslatorQT(); //�������� ���������� QT
	bool LoadResourceFile(const QString &FileName); //������������� ����� �� ��������

private:
	bool InitializeContent(const QString &Content); //�������� ��������� ����������� � �����

private:
	ISLocalization();
	~ISLocalization();
	ISLocalization(ISLocalization const &) {};
	ISLocalization& operator=(ISLocalization const&) { return *this; };

private:
	QString ErrorString;
	ISStringMap Dictionary;
	ISVectorString LoadedFiles;
	ISCriticalSection CriticalSection;
};
//-----------------------------------------------------------------------------
#define LANG(PARAMETER) ISLocalization::Instance().GetString(PARAMETER)
//-----------------------------------------------------------------------------
#endif
