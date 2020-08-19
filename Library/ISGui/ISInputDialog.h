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

	static QString GetString(const QString &Title = QString(), const QString &LabelText = QString(), const QVariant &Value = QVariant()); //�������� ������
	static QString GetText(const QString &Title = QString(), const QString &LabelText = QString(), const QVariant &Value = QVariant()); //�������� ������� ������
	static QString GetText(const QString &Title, const QString &LabelText, const QVariant &Value, bool &Ok); //�������� ������� ������
	static int GetInteger(bool &Ok, const QString &Title = QString(), const QString &LabelText = QString(), int Minimum = 0, int Maximum = 0, const QVariant &Value = QVariant()); //�������� ����� �����
	static QDateTime GetDateTime(const QString &Title = QString(), const QString &LabelText = QString()); //�������� ���� � �����
	static QDate GetDate(const QString &Title = QString(), const QString &LabelText = QString()); //�������� ����
	static QTime GetTime(const QString &Title = QString(), const QString &LabelText = QString()); //�������� �����
	static QString GetPassword(); //�������� ������
	static QVariant GetList(const QString &Title, const QString &LabelText, const ISStringToVariantMap &VariantMap); //�������� ������

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
