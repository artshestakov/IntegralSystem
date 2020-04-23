#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISNamespace.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class ISSqlModelHelper
{
public:
	static QVariant ValueForType(const QVariant &Value, ISNamespace::FieldType Type);
	static QVariant ValueFromTextAlignment(ISNamespace::FieldType FieldType, PMetaForeign *MetaForeign); //�������������� �������� � ���� ��������� ������
	static QString ValueForToolTip(bool ShowToolTip, const QVariant &Value, ISNamespace::FieldType FieldType); //�������������� �������� � ����� ��� ���������
};
//-----------------------------------------------------------------------------
