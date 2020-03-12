#pragma once
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class CGForeign
{
public:
	static bool CreateForeign(PMetaForeign *MetaForeign, QString &ErrorString); //�������� �������� �����
	static bool UpdateForeign(PMetaForeign *MetaForeign, QString &ErrorString); //���������� �������� �����
	static bool CheckExistForeign(PMetaForeign *MetaForeign, bool &Exist, QString &ErrorString); //�������� ������������� �������� �����

private:
	static QString GetForeignName(PMetaForeign *MetaForeign); //��������� ����� �������� �����
};
//-----------------------------------------------------------------------------
