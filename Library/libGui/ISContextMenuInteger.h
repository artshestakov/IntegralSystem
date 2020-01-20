#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISContextMenuBase.h"
//-----------------------------------------------------------------------------
//����������� ���� ��� ����� �������������� ����� ����� (��������������� ISQSpinBox)
class ISContextMenuInteger : public ISContextMenuBase
{
	Q_OBJECT

signals:
	void StepUp();
	void StepDown();

public:
	ISContextMenuInteger(QWidget *ParentEdit, bool ReadOnly, bool UndoAvailable, bool RedoAvailable, bool HasSelectedText, QLineEdit::EchoMode EchoMode, bool Empty, int Value, int Minimum, int Maximum);
	virtual ~ISContextMenuInteger();
};
//-----------------------------------------------------------------------------
