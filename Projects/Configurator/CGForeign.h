#pragma once
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class CGForeign
{
public:
	static bool CreateForeign(PMetaClassForeign *MetaForeign, QString &ErrorString); //�������� �������� �����
	static bool UpdateForeign(PMetaClassForeign *MetaForeign, QString &ErrorString); //���������� �������� �����
	static bool CheckExistForeign(PMetaClassForeign *MetaForeign, bool &Exist, QString &ErrorString); //�������� ������������� �������� �����

private:
	static QString GetForeignName(PMetaClassForeign *MetaForeign); //��������� ����� �������� �����
};
//-----------------------------------------------------------------------------
