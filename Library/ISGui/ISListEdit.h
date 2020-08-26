#pragma once
#ifndef _ISLISTEDIT_H_INCLUDED
#define _ISLISTEDIT_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISInterfaceForm.h"
#include "PMetaClass.h"
#include "ISListWidget.h"
#include "ISFieldEdits.h"
//-----------------------------------------------------------------------------
class ISListEditPopup : public ISInterfaceForm
{
	Q_OBJECT

signals:
	void Selected(const QVariant &ID, const QString &Text);
	void Hided();

public:
	ISListEditPopup(PMetaForeign *meta_foreign, QWidget *ComboBox = 0);
	virtual ~ISListEditPopup();

	void showEvent(QShowEvent *e);
	void hideEvent(QHideEvent *e);
	void paintEvent(QPaintEvent *e);

	void SetCurrentValue(const QVariant &current_value);

	QString GetSqlFilter() const; //Получить фильтр
	void SetSqlFilter(const QString &sql_filter); //Изменить фильтр
	void ClearSqlFilter(); //Очистить фильтр

protected:
	void Search(const QVariant &value); //Поиск
	void ItemClicked(QListWidgetItem *ListWidgetItem); //Выбор позиции
	void Add(); //Добавить запись
	void LoadDataFromQuery(); //Загрузка справочника
	void EnterClicked() override;
	void EscapeClicked() override;

private:
	ISLineEdit *LineEdit;
	QLabel *LabelName;
	ISListWidget *ListWidget;
	QLabel *LabelCountRow;
	QLabel *LabelSearch;
	QLabel *LabelEmpty;

	QString SqlFilter;
	PMetaForeign *MetaForeign;
	PMetaTable *MetaTableForeign;
	QVariant CurrentValue;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISListEdit : public ISFieldEditBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISListEdit(QWidget *parent = 0);
	virtual ~ISListEdit();

	void SetValue(const QVariant &value) override; //Приходит идентификатор
	QVariant GetValue() const override; //Возвращает идентификатор
	void Clear() override;

	void SetReadOnly(bool read_only) override;

	virtual void InvokeList(PMetaForeign *meta_foreign);

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
	PMetaForeign *MetaForeign;
	PMetaTable *MetaTable;

	ISPushButton *ButtonMain;
	QToolButton *ButtonList;
	QAction *ActionCreate;
	QAction *ActionEdit;
	ISListEditPopup *ListEditPopup;

	QVariant ID;
};	
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISExecutorEdit : public ISListEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISExecutorEdit(QWidget *parent = 0);
	virtual ~ISExecutorEdit();

	void SetReadOnly(bool read_only) override;

private:
	void DesignateMe();

private:
	ISPushButton *ButtonDesignateMe;
};
//-----------------------------------------------------------------------------
#endif
