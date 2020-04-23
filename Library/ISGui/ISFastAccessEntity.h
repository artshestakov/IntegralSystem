#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISFastAccessEntity
{
public:
	ISFastAccessEntity(const ISFastAccessEntity &) = delete;
	ISFastAccessEntity(ISFastAccessEntity &&) = delete;
	ISFastAccessEntity &operator=(const ISFastAccessEntity &) = delete;
	ISFastAccessEntity &operator=(ISFastAccessEntity &&) = delete;
	~ISFastAccessEntity();

	static ISFastAccessEntity& GetInstance();

	void LoadExternalTools(); //Загрузить внешние инструменты
	void ReloadExternalTools(); //Перезагрузить внешние инструменты
	QVector<ISMetaExternalTool*> GetExternalTools();

	void LoadCreateRecords();
	void ReloadCreateRecords();
	ISVectorString GetCreateRecords();

private:
	ISFastAccessEntity();

	QVector<ISMetaExternalTool*> ExternalTools;
	ISVectorString CreateRecords;
};
//-----------------------------------------------------------------------------
