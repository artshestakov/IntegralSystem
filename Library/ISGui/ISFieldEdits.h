#pragma once
#ifndef _ISFIELDEDITS_H_INCLUDED
#define _ISFIELDEDITS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISLineEdit.h"
#include "ISComboEdit.h"
#include "ISListEdit.h"
#include "ISRadioEdit.h"
#include "ISCheckEdit.h"
#include "ISTextEdit.h"
#include "ISPhoneBaseEdit.h"
#include "ISDateTimeEdit.h"
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
class ISDateEdit : public ISDateTimeEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDateEdit(QWidget *parent = 0);
	virtual ~ISDateEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;

	void SetMinimumDate(const QDate &Date);
	void SetMaximumDate(const QDate &Date);
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
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISTimeEdit : public ISDateTimeEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTimeEdit(QWidget *parent = 0);
	virtual ~ISTimeEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISDoubleEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDoubleEdit(QWidget *parent = 0);
	virtual ~ISDoubleEdit();

	void SetValue(const QVariant &value) override;

protected:
	void TextChanged(const QString &Text) override;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISComboTimeEdit : public ISComboEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboTimeEdit(QWidget *parent = 0);
	virtual ~ISComboTimeEdit();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISExecutorEdit : public ISListEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISExecutorEdit(QWidget *parent = 0);
	virtual ~ISExecutorEdit();

	void SetReadOnly(bool read_only) override;

protected:
	void DesignateMe();

private:
	ISPushButton *ButtonDesignateMe;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISIntegerEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISIntegerEdit(QWidget *parent = 0);
	virtual ~ISIntegerEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;

	void SetMinimum(int Minimum);
	void SetMaximum(int Maximum);
	void SetRange(int Minimum, int Maximum);
	void ResetRange();

private:
	QIntValidator *IntValidator;
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
class ISPassportEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISPassportEdit(QWidget *parent = 0);
	virtual ~ISPassportEdit();

	void SetValue(const QVariant &value);
	QVariant GetValue() const override;
	void Clear() override;

protected:
	virtual void Edit(); //Метод сделан виртуальным на случай новых подобных полей (заграничный паспорт, паспорты других стран)
	QString PreparePassport(const QString &passport_string);

private:
	QString PassportString;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISPhoneEdit : public ISPhoneBaseEdit
{
	Q_OBJECT

signals:
	void Called();

public:
	Q_INVOKABLE ISPhoneEdit(QWidget *parent = 0);
	virtual ~ISPhoneEdit();

protected:
	void Call() override;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISSearchEdit : public ISLineEdit
{
	Q_OBJECT

		signals :
	void Search(const QString &SearchValue);

public:
	ISSearchEdit(QWidget *parent);
	virtual ~ISSearchEdit();

	void Updated();

protected:
	void SearchChanged();
	void AboutToShow();
	void AboutToHide();
	void LastSearchClicked();
	void Timeout();

private:
	ISServiceButton *ButtonLastSearch;
	QTimer *Timer;
};
//-----------------------------------------------------------------------------
#endif
