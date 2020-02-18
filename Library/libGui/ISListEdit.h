#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISFieldEditBase.h"
#include "PMetaClass.h"
#include "ISButtons.h"
#include "ISListEditPopup.h"
//-----------------------------------------------------------------------------
class ISListEdit : public ISFieldEditBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISListEdit(QWidget *parent);
	virtual ~ISListEdit();

	void SetValue(const QVariant &value) override; //Приходит идентификатор
	QVariant GetValue() const override; //Возвращает идентификатор
	void Clear() override;
	void SetVisibleClear(bool Visible) override;

	void SetReadOnly(bool read_only) override;

	virtual void InvokeList(PMetaClassForeign *meta_foreign);

	QString GetCurrentText() const; //Получить локальное наименование объекта
	void SetEnabled(bool Enabled);
	
	void SetSqlFilter(const QString &sql_filter); //Изменить фильтр
	void ClearSqlFilter(); //Очистить SQL-фильтр

protected:
	void SelectedValue(const QVariant &id, const QString &text);
	
	void ShowPopup();
	void HidedPopup();
	void ShowListForm();
	
	void CreateObject();
	void EditObject();

private:
	PMetaClassForeign *MetaForeign;
	PMetaClassTable *MetaTable;

	ISPushButton *ButtonMain;
	QToolButton *ButtonList;
	QAction *ActionCreate;
	QAction *ActionEdit;
	ISListEditPopup *ListEditPopup;

	QVariant ID;
};
//-----------------------------------------------------------------------------
