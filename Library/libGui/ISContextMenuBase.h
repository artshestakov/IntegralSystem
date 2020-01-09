#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
//-----------------------------------------------------------------------------
//������� ����� ������������ ���� ��� �������� �������������� ���������� (��������������� ��� ����� �������������� ���������� IS*Edit)
class LIBGUI_EXPORT ISContextMenuBase : public QMenu
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
