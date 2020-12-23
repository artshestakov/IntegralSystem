#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
#include "ISListWidget.h"
//-----------------------------------------------------------------------------
class ISFavoritesForm : public ISInterfaceForm
{
	Q_OBJECT

public:
	ISFavoritesForm(QWidget *parent = 0, const QString &table_name = QString());
	virtual ~ISFavoritesForm();

private:
	void LoadFavorites(); //Загрузка избранного
	void ReloadFavorites(); //Перезагрузка изранного
	void OpenFavorite(); //Открыть избранный обхект
	void ClearFavorites(); //Очистить избранные объекты
	void EscapeClicked() override;

private:
	void ListWidgetDoubleClicked(QListWidgetItem *Item);

private:
	QString TableName;
	ISListWidget *ListWidget;
};
//-----------------------------------------------------------------------------
