#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISStructs.h"
#include "ISTypes.h"
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
	QVectorString GetCreateRecords();

private:
	ISFastAccessEntity();

	QVector<ISMetaExternalTool*> ExternalTools;
	QVectorString CreateRecords;
};
//-----------------------------------------------------------------------------
