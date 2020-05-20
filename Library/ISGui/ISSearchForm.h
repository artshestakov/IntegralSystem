#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
#include "PMetaClass.h"
#include "ISButtons.h"
#include "ISButtons.h"
#include "ISSearchModel.h"
//-----------------------------------------------------------------------------
class ISSearchForm : public ISInterfaceForm
{
	Q_OBJECT

signals:
	void StartSearch(const QString &SearchString, const QVariantMap &VariantMap);

public:
	ISSearchForm(PMetaTable *meta_table, QWidget *parent = 0);
	virtual ~ISSearchForm();

protected:
	void closeEvent(QCloseEvent *e) override;
	void EscapeClicked() override;
	void EnterClicked() override;
	
	bool CheckExistField(const QString &FieldName) const;
	void LoadFields(); //Загрузка доступных полей для поиск в список
	void AddFieldFromList(const QString &FieldLocalName, const QString &FieldName); //Добавить поле в список полей для поиска

	void AddFilter(); //Добавить поисковой фильтр
	void Search(); //Поиск
	void SearchFromField(ISSearchModel &SearchModel, QTreeWidgetItem *TreeWidgetItem);

	void DeleteFieldClicked();
	void DeleteAllFields(); //Удалить все поля
	void DeleteField(QTreeWidgetItem *TreeWidgetItem); //Удалить поле
	void UpdateInterface(); //Обновление интерфейса

private:
	PMetaTable *MetaTable;
	QTreeWidget *TreeWidget;

	ISPushButton *ButtonAddFilter;
	ISPushButton *ButtonClear;
	ISPushButton *ButtonSearch;
	ISPushButton *ButtonHide;

	QMap<ISServiceButton*, QTreeWidgetItem*> MapItems;
};
//-----------------------------------------------------------------------------
