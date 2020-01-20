#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISContextMenuBase.h"
//-----------------------------------------------------------------------------
//����������� ���� ��� ����� �������������� ������ (��������������� ISLineEdit � ISTextEdit)
class ISContextMenuText : public ISContextMenuBase
{
	Q_OBJECT

signals:
	void UppercaseText();
	void LowercaseText();
	void SearchGoogle();
	void SearchYandex();
	void SearchWikipedia();
	void SearchMail();
	void SearchBing();
	void SearchYahoo();

public:
	ISContextMenuText(QWidget *ParentEdit, bool ReadOnly, bool UndoAvailable, bool RedoAvailable, bool HasSelectedText, QLineEdit::EchoMode EchoMode, bool Empty, int SelectedTextCount);
	virtual ~ISContextMenuText();
};
//-----------------------------------------------------------------------------
