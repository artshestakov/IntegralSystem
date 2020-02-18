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

	void SetValue(const QVariant &value) override; //�������� �������������
	QVariant GetValue() const override; //���������� �������������
	void Clear() override;
	void SetVisibleClear(bool Visible) override;

	void SetReadOnly(bool read_only) override;

	virtual void InvokeList(PMetaClassForeign *meta_foreign);

	QString GetCurrentText() const; //�������� ��������� ������������ �������
	void SetEnabled(bool Enabled);
	
	void SetSqlFilter(const QString &sql_filter); //�������� ������
	void ClearSqlFilter(); //�������� SQL-������

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
