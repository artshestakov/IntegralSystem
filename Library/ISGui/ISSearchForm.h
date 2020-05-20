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
	void LoadFields(); //�������� ��������� ����� ��� ����� � ������
	void AddFieldFromList(const QString &FieldLocalName, const QString &FieldName); //�������� ���� � ������ ����� ��� ������

	void AddFilter(); //�������� ��������� ������
	void Search(); //�����
	void SearchFromField(ISSearchModel &SearchModel, QTreeWidgetItem *TreeWidgetItem);

	void DeleteFieldClicked();
	void DeleteAllFields(); //������� ��� ����
	void DeleteField(QTreeWidgetItem *TreeWidgetItem); //������� ����
	void UpdateInterface(); //���������� ����������

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
