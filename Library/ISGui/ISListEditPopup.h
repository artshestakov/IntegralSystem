#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISInterfaceForm.h"
#include "ISLineEdit.h"
#include "ISListWidget.h"
#include "PMetaClassForeign.h"
#include "PMetaClassTable.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISListEditPopup : public ISInterfaceForm
{
	Q_OBJECT

signals:
	void Selected(const QVariant &ID, const QString &Text);
	void Hided();

public:
	ISListEditPopup(PMetaClassForeign *meta_foreign, QWidget *ComboBox = 0);
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
	PMetaClassForeign *MetaForeign;
	PMetaClassTable *MetaTableForeign;
	QVariant CurrentValue;
};
//-----------------------------------------------------------------------------
