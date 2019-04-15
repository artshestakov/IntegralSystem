#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISInterfaceDialogForm.h"
#include "ISNamespace.h"
#include "ISFieldEditBase.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISInputDialog : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISInputDialog(const QString &Title, const QString &LabelText = QString(), QWidget *parent = 0);
	virtual ~ISInputDialog();

	void showEvent(QShowEvent *e);

	static QVariant GetString(QWidget *parent, const QString &Title, const QString &LabelText, const QVariant &Value = QVariant()); //�������� ������
	static QVariant GetText(QWidget *parent, const QString &Title, const QString &LabelText, const QVariant &Value = QVariant()); //�������� ������� ������
	static QVariant GetInteger(QWidget *parent, const QString &Title, const QString &LabelText, int Minimum = 0, int Maximum = 0, const QVariant &Value = QVariant()); //�������� ����� �����
	static QVariant GetDateTime(QWidget *parent, const QString &Title, const QString &LabelText); //�������� ���� � �����
	static QVariant GetDate(QWidget *parent, const QString &Title, const QString &LabelText); //�������� ����
	static QVariant GetTime(QWidget *parent, const QString &Title, const QString &LabelText); //�������� �����
	static QVariant GetPassword(QWidget *parent, const QString &Title, const QString &LabelText); //�������� ������
	static QVariant GetColor(QWidget *parent, const QString &Title, const QString &LabelText, const QVariant &Color = QVariant()); //�������� ����
	static QVariant GetList(QWidget *parent, const QString &Title, const QString &LabelText, const QVariantMap &VariantMap); //�������� ������

    void SetEditWidget(ISNamespace::FieldType DataType, const QString &WidgetEditName = QString());
	void SetEditWidget(ISFieldEditBase *EditWidget);
	void SetValue(const QVariant &Value);
	QVariant GetValue() const;
	ISFieldEditBase* GetEditWidget();

protected:
	void Apply();
	void EnterClicked() override;
	void ControlEnterClicked() override;

private:
	QLabel *Label;
	QHBoxLayout *LayoutField;
	ISFieldEditBase *FieldEditBase;
	bool AddingFieldEdit;

	QVariant Value;
};
//-----------------------------------------------------------------------------
