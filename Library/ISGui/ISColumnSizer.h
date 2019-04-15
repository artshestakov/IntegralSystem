#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISColumnSizeItem.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISColumnSizer : public QObject
{
	Q_OBJECT

public:
	ISColumnSizer(const ISColumnSizer &) = delete;
	ISColumnSizer(ISColumnSizer &&) = delete;
	ISColumnSizer &operator=(const ISColumnSizer &) = delete;
	ISColumnSizer &operator=(ISColumnSizer &&) = delete;
	~ISColumnSizer();

	static ISColumnSizer& GetInstance();

	void Initialize();
	void Save(); //Сохранить размеры в базу данных
	void Clear(); //Очистить размеры

	void SetColumnSize(const QString &TableName, const QString &FieldName, int Size); //Изменить размер поля в таблице
	int GetColumnSize(const QString &TableName, const QString &FieldName) const; //Получить размер поля в таблице

private:
	ISColumnSizer();

	QMap<QString, ISColumnSizeItem*> Tables;
};
//-----------------------------------------------------------------------------
