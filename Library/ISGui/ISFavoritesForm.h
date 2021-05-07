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
    ISFavoritesForm(QWidget *parent = 0, const QString &table_name = QString(), const QString &table_local_name = QString());
    virtual ~ISFavoritesForm();

private:
    void Open(); //Открыть избранный обхект
    void Clear(); //Очистить избранные объекты
    void EscapeClicked() override;

private:
    void ListWidgetDoubleClicked(QListWidgetItem *Item);

private:
    QString TableName;
    QAction *ActionClearFavorites;
    ISListWidget *ListWidget;
    QLabel *LabelRowCount;
};
//-----------------------------------------------------------------------------
