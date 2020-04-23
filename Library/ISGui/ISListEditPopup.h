#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
#include "ISLineEdit.h"
#include "ISListWidget.h"
#include "PMetaClass.h"
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

	QString GetSqlFilter() const; //�������� ������
	void SetSqlFilter(const QString &sql_filter); //�������� ������
	void ClearSqlFilter(); //�������� ������

protected:
	void Search(const QVariant &value); //�����
	void ItemClicked(QListWidgetItem *ListWidgetItem); //����� �������
	void Add(); //�������� ������
	void LoadDataFromQuery(); //�������� �����������
	void Hide();
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
