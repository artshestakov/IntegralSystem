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
	Q_INVOKABLE ISBIKEdit(QWidget *parent = 0);
	virtual ~ISBIKEdit();

	bool IsValid() const override;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISKPPEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISKPPEdit(QWidget *parent = 0);
	virtual ~ISKPPEdit();

	bool IsValid() const override;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISOGRNEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISOGRNEdit(QWidget *parent = 0);
	virtual ~ISOGRNEdit();

	bool IsValid() const override;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISOKPOEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISOKPOEdit(QWidget *parent = 0);
	virtual ~ISOKPOEdit();

	bool IsValid() const override;
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
	void SetReadOnly(bool read_only) override;

private:
	void Generate();

private:
	ISServiceButton *ButtonGenerate;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISVINEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISVINEdit(QWidget *parent = 0);
	~ISVINEdit();

	bool IsValid() const override;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISBirthdayEdit : public ISDateEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISBirthdayEdit(QWidget *parent = 0);
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

signals:
	void SearchFromINN();

public:
	Q_INVOKABLE ISINNEdit(QWidget *parent = 0);
	virtual ~ISINNEdit();

	bool IsValid() const override;
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
	Q_INVOKABLE ISWeekDayEdit(QWidget *parent = 0);
	virtual ~ISWeekDayEdit();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISPathEditDir : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISPathEditDir(QWidget *parent = 0);
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
	Q_INVOKABLE ISPathEditFile(QWidget *parent = 0);
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
	Q_INVOKABLE ISPasswordEdit(QWidget *parent = 0);
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
	Q_INVOKABLE ISYearEdit(QWidget *parent = 0);
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
	Q_INVOKABLE ISMonthEdit(QWidget *parent = 0);
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
	Q_INVOKABLE ISEMailEdit(QWidget *parent = 0);
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
	Q_INVOKABLE ISDivisionCodeEdit(QWidget *parent = 0);
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
	Q_INVOKABLE ISTaskPriorityEdit(QWidget *parent = 0);
	virtual ~ISTaskPriorityEdit();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISTaskImportantEdit : public ISCheckEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTaskImportantEdit(QWidget *parent = 0);
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
