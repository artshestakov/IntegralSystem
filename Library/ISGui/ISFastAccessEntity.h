#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISMetaExternalTool.h"
#include "ISTypes.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISFastAccessEntity : public QObject
{
	Q_OBJECT

public:
	ISFastAccessEntity(const ISFastAccessEntity &) = delete;
	ISFastAccessEntity(ISFastAccessEntity &&) = delete;
	ISFastAccessEntity &operator=(const ISFastAccessEntity &) = delete;
	ISFastAccessEntity &operator=(ISFastAccessEntity &&) = delete;
	~ISFastAccessEntity();

	static ISFastAccessEntity& GetInstance();

	void LoadExternalTools(); //��������� ������� �����������
	void ReloadExternalTools(); //������������� ������� �����������
	QVector<ISMetaExternalTool*> GetExternalTools();

	void LoadCreateRecords();
	void ReloadCreateRecords();
	QVectorString GetCreateRecords();

private:
	ISFastAccessEntity();

	QVector<ISMetaExternalTool*> ExternalTools;
	QVectorString CreateRecords;
};
//-----------------------------------------------------------------------------
