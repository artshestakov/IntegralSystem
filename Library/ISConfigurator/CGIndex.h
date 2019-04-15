#pragma once
//-----------------------------------------------------------------------------
#include "isconfigurator_global.h"
#include "PMetaClassIndex.h"
//-----------------------------------------------------------------------------
class ISCONFIGURATOR_EXPORT CGIndex : public QObject
{
	Q_OBJECT

public:
	CGIndex();
	virtual ~CGIndex();

	static bool CreateIndex(PMetaClassIndex *Index, QString &ErrorString); //Создание индекса
	static bool UpdateIndex(PMetaClassIndex *Index, QString &ErrorString); //Обновление индекса
	static bool CheckExistIndex(PMetaClassIndex *Index); //Проверка существования индекса
	static bool CheckIndexForeign(PMetaClassIndex *Index); //Проверить ссылается ли на этого индекс внешний ключ
	static bool ReindexIndex(PMetaClassIndex *Index, QString &ErrorString); //Перестроить индекс

private:
	static QString GetIndexName(PMetaClassIndex *Index); //Получение имени индекса
};
//-----------------------------------------------------------------------------
