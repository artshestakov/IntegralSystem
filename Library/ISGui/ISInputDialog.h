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

	static QVariant GetString(QWidget *parent, const QString &Title, const QString &LabelText, const QVariant &Value = QVariant()); //Получить строку
	static QVariant GetText(QWidget *parent, const QString &Title, const QString &LabelText, const QVariant &Value = QVariant()); //Получить большую строку
	static QVariant GetInteger(QWidget *parent, const QString &Title, const QString &LabelText, int Minimum = 0, int Maximum = 0, const QVariant &Value = QVariant()); //Получить целое число
	static QVariant GetDateTime(QWidget *parent, const QString &Title, const QString &LabelText); //Получить дату и время
	static QVariant GetDate(QWidget *parent, const QString &Title, const QString &LabelText); //Получить дату
	static QVariant GetTime(QWidget *parent, const QString &Title, const QString &LabelText); //Получить время
	static QVariant GetPassword(QWidget *parent, const QString &Title, const QString &LabelText); //Получить пароль
	static QVariant GetColor(QWidget *parent, const QString &Title, const QString &LabelText, const QVariant &Color = QVariant()); //Получить цвет
	static QVariant GetList(QWidget *parent, const QString &Title, const QString &LabelText, const QVariantMap &VariantMap); //Получить список

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
