#pragma once
#ifndef _ISCONTEXTMENUBASE_H_INCLUDED
#define _ISCONTEXTMENUBASE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
//������� ����� ������������ ���� ��� �������� �������������� ���������� (��������������� ��� ����� �������������� ���������� IS*Edit)
class ISContextMenuBase : public QMenu
{
	Q_OBJECT

signals:
	void Delete();

public:
	ISContextMenuBase(QWidget *ParentEdit, bool ReadOnly, bool UndoAvailable, bool RedoAvailable, bool HasSelectedText, QLineEdit::EchoMode EchoMode, bool Empty);
	virtual ~ISContextMenuBase();

protected:
	void AppendOtherAction(QAction *Action);
	void BeforeToShow(); //�������� ����� ������������ ����
	void CallVirtualKeyboard(); //����� ����������� ����������

private:
	QAction *ActionOtherMenu;
};
//-----------------------------------------------------------------------------
#endif
