#pragma once
//-----------------------------------------------------------------------------
#include "isconfigurator_global.h"
#include "PMetaClassForeign.h"
//-----------------------------------------------------------------------------
class ISCONFIGURATOR_EXPORT CGForeign : public QObject
{
	Q_OBJECT

public:
	CGForeign();
	virtual ~CGForeign();

	static bool CreateForeign(PMetaClassForeign *MetaForeign, QString &ErrorString); //�������� �������� �����
	static bool UpdateForeign(PMetaClassForeign *MetaForeign, QString &ErrorString); //���������� �������� �����
	static bool CheckExistForeign(PMetaClassForeign *MetaForeign); //�������� ������������� �������� �����

private:
	static QString OnGetForeignName(PMetaClassForeign *MetaForeign); //��������� ����� �������� �����
};
//-----------------------------------------------------------------------------
