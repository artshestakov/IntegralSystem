#pragma once
//-----------------------------------------------------------------------------
#include "PMetaClassForeign.h"
//-----------------------------------------------------------------------------
class CGForeign
{
public:
	static bool CreateForeign(PMetaClassForeign *MetaForeign, QString &ErrorString); //�������� �������� �����
	static bool UpdateForeign(PMetaClassForeign *MetaForeign, QString &ErrorString); //���������� �������� �����
	static bool CheckExistForeign(PMetaClassForeign *MetaForeign); //�������� ������������� �������� �����

private:
	static QString OnGetForeignName(PMetaClassForeign *MetaForeign); //��������� ����� �������� �����
};
//-----------------------------------------------------------------------------
