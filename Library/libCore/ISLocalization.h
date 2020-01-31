#pragma once
#ifndef _ISLOCALIZATION_H_INCLUDED
#define _ISLOCALIZATION_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTypes.h"
//-----------------------------------------------------------------------------
class ISLocalization
{
public:
	ISLocalization(const ISLocalization &) = delete;
	ISLocalization(ISLocalization &&) = delete;
	ISLocalization &operator=(const ISLocalization &) = delete;
	ISLocalization &operator=(ISLocalization &&) = delete;
	~ISLocalization();

	static ISLocalization& GetInstance();

	QString GetString(const QString &ParameterName) const; //�������� �������������� ������
	void LoadResourceFile(const QString &FileName); //������������� ����� �� ��������
	void InitializeContent(const QString &Content); //�������� ��������� ����������� � �����

private:
	ISLocalization();

	std::map<QString, QString> Dictionary;
	QVectorString  LoadedFiles;
};
//-----------------------------------------------------------------------------
#define LANG(PARAMETER) ISLocalization::GetInstance().GetString(PARAMETER)
//-----------------------------------------------------------------------------
#endif
