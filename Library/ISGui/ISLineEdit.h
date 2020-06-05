#pragma once
//-----------------------------------------------------------------------------
#include "ISFieldEditBase.h"
#include "ISEdits.h"
//-----------------------------------------------------------------------------
class ISLineEdit : public ISFieldEditBase
{
	Q_OBJECT

signals:
	void ClearPressed();
	void EnterClicked();

public:
	Q_INVOKABLE ISLineEdit(QWidget *parent);
	virtual ~ISLineEdit();

	virtual void SetValue(const QVariant &value) override;
	virtual QVariant GetValue() const override;
	virtual void Clear() override;

	void SetReadOnly(bool read_only) override;
	void SetPlaceholderText(const QString &placeholder_text) override;
	void SetRegExp(const QString &RegExp) override;
	void SetColorText(const QColor &Color) override;

	void SetValidator(QValidator *Validator);
	void SetEchoMode(QLineEdit::EchoMode EchoMode);
	void SetCompleter(QCompleter *Completer);
	void SetTextAlignment(Qt::Alignment Alignment);
	void SetInputMask(const QString &InputMask);
	void SetFocusPolicy(Qt::FocusPolicy focus_policy);
	void SetMaxLength(int Length);
	void SetIcon(const QIcon &Icon);
	void SelectAll();

	void SetUppercase(bool uppercase);
	void SetLowercase(bool lowercase);
	void ResetFontcase(); //Сброс контроля регистра

	void AddAction(QAction *Action, QLineEdit::ActionPosition Position);
	
	void CreateCompleter(const QStringList &StringList);
	QStringList GetCompleterList() const;

protected:
	void OnUpperText(const QString &Text);
	void OnLowerText(const QString &Text);
	ISQLineEdit* GetLineEdit();

private:
	ISQLineEdit *LineEdit;
	QCompleter *Completer;
};
//-----------------------------------------------------------------------------
