#pragma once
#ifndef _ISFIELDEDITS_H_INCLUDED
#define _ISFIELDEDITS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISLineEdit.h"
#include "ISDoubleEdit.h"
#include "ISDateEdit.h"
#include "ISComboEdit.h"
#include "ISIntegerEdit.h"
#include "ISRadioEdit.h"
#include "ISCheckEdit.h"
#include "ISTextEdit.h"
//-----------------------------------------------------------------------------
class ISBIKEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISBIKEdit(QWidget *parent);
	virtual ~ISBIKEdit();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISKPPEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISKPPEdit(QWidget *parent);
	virtual ~ISKPPEdit();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISOGRNEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISOGRNEdit(QWidget *parent);
	virtual ~ISOGRNEdit();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISOKPOEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISOKPOEdit(QWidget *parent);
	virtual ~ISOKPOEdit();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISPercentEdit : public ISDoubleEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISPercentEdit(QWidget *parent);
	virtual ~ISPercentEdit();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISUuidEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISUuidEdit(QWidget *parent = 0);
	virtual ~ISUuidEdit();

	void SetValue(const QVariant &value) override;
	bool IsValid() const override;

private:
	void Generate();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISVINEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISVINEdit(QWidget *parent);
	~ISVINEdit();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISBirthdayEdit : public ISDateEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISBirthdayEdit(QWidget *parent);
	virtual ~ISBirthdayEdit();

	void SetValue(const QVariant &value) override;

protected:
	void UpdateLabel(const QDate &Date);

private:
	QLabel *Label;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISINNEdit : public ISLineEdit
{
	Q_OBJECT

		signals :
	void SearchFromINN();

public:
	Q_INVOKABLE ISINNEdit(QWidget *parent);
	virtual ~ISINNEdit();

	bool IsValid() const;
	void SetEnabledSearch(bool Enabled);

private:
	ISServiceButton *ButtonSearch;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISWeekDayEdit : public ISComboEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISWeekDayEdit(QWidget *parent);
	virtual ~ISWeekDayEdit();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISPathEditDir : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISPathEditDir(QWidget *parent);
	virtual ~ISPathEditDir();

	void SetValue(const QVariant &value) override;
	void Clear() override;

protected:
	void SelectDir();
	void OpenDir();

private:
	ISServiceButton *ButtonOpenDir;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISPathEditFile : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISPathEditFile(QWidget *parent);
	virtual ~ISPathEditFile();

	void SetValue(const QVariant &value) override;
	void SetFilterFile(const QString &filter_file);

protected:
	void SelectFile();

private:
	QString FilterFile;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISPasswordEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISPasswordEdit(QWidget *parent);
	virtual ~ISPasswordEdit();

	void SetVisibleGenerate(bool Visible); //Показывать/непоказывать кнопку генерации пароля
	void SetVisibleCheckBox(bool Visible);

private:
	void PasswordVisibleChanged(int State);
	void GeneratePassword(); //Генерация пароля

private:
	QCheckBox *CheckPasswordVisible;
	ISServiceButton *ButtonGeneratePassword;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISUrlEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISUrlEdit(QWidget *parent = 0);
	virtual ~ISUrlEdit();

private:
	void OpenUrl();
	void UrlChanged();

private:
	ISServiceButton *ButtonUrl;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISYearEdit : public ISIntegerEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISYearEdit(QWidget *parent);
	virtual ~ISYearEdit();

	void SelectCurrentYear();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISMonthEdit : public ISComboEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISMonthEdit(QWidget *parent);
	virtual ~ISMonthEdit();

	void SelectCurrentMonth();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISEMailEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISEMailEdit(QWidget *parent);
	virtual ~ISEMailEdit();

private:
	void Send();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISDivisionCodeEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDivisionCodeEdit(QWidget *parent);
	virtual ~ISDivisionCodeEdit();

	bool IsValid() const override;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISTaskPriorityEdit : public ISRadioEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTaskPriorityEdit(QWidget *parent);
	virtual ~ISTaskPriorityEdit();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISTaskNameEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTaskNameEdit(QWidget *parent = 0);
	virtual ~ISTaskNameEdit();

	void Invoke() override;

private:
	QStringList StringList;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISTaskImportantEdit : public ISCheckEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTaskImportantEdit(QWidget *parent);
	virtual ~ISTaskImportantEdit();

private:
	void ImportantChanged(const QVariant &value);
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISTaskDescriptionEdit : public ISTextEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTaskDescriptionEdit(QWidget *parent = 0);
	virtual ~ISTaskDescriptionEdit();
};
//-----------------------------------------------------------------------------
#endif
