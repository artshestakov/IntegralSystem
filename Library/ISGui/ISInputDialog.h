#pragma once
#ifndef _ISINPUTDIALOG_H_INCLUDED
#define _ISINPUTDIALOG_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
#include "ISNamespace.h"
#include "ISFieldEditBase.h"
//-----------------------------------------------------------------------------
class ISInputDialog : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISInputDialog(ISNamespace::FieldType DataType, const QString &Title = QString(), const QString &LabelText = QString(), const QVariant &Value = QVariant());
	virtual ~ISInputDialog();

	static QString GetString(const QString &Title = QString(), const QString &LabelText = QString(), const QVariant &Value = QVariant()); //Получить строку
	static QString GetText(const QString &Title = QString(), const QString &LabelText = QString(), const QVariant &Value = QVariant()); //Получить большую строку
	static QString GetText(const QString &Title, const QString &LabelText, const QVariant &Value, bool &Ok); //Получить большую строку
	static int GetInteger(bool &Ok, const QString &Title = QString(), const QString &LabelText = QString(), int Minimum = 0, int Maximum = 0, const QVariant &Value = QVariant()); //Получить целое число
	static QDateTime GetDateTime(const QString &Title = QString(), const QString &LabelText = QString()); //Получить дату и время
	static QDate GetDate(const QString &Title = QString(), const QString &LabelText = QString()); //Получить дату
	static QTime GetTime(const QString &Title = QString(), const QString &LabelText = QString()); //Получить время
	static QString GetPassword(); //Получить пароль
	static QVariant GetList(const QString &Title, const QString &LabelText, const ISStringToVariantMap &VariantMap); //Получить список

	QVariant GetValue() const;
	ISFieldEditBase* GetEditWidget();
	void SetEditWidget(ISFieldEditBase *field_edit_base);

protected:
	void AfterShowEvent() override;

private:
	void Apply();
	void EnterClicked() override;
	void ControlEnterClicked() override;

private:
	QLabel *Label;
	ISFieldEditBase *FieldEditBase;
};
//-----------------------------------------------------------------------------
#endif
