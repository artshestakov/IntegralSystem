#pragma once
#ifndef _ISCONTEXTMENU_H_INCLUDED
#define _ISCONTEXTMENU_H_INCLUDED
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

private:
    QAction *ActionOtherMenu;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISContextMenuImage : public QMenu
{
    Q_OBJECT

signals:
    void Select();
    void Cut();
    void Copy();
    void Paste();
    void Save();
    void OpenView();

public:
    ISContextMenuImage(QWidget *parent, bool PixmapIsNull);
    virtual ~ISContextMenuImage();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISContextMenuText : public ISContextMenuBase
{
    Q_OBJECT

signals:
    void UppercaseText();
    void LowercaseText();

public:
    ISContextMenuText(QWidget *ParentEdit, bool ReadOnly, bool UndoAvailable, bool RedoAvailable, bool HasSelectedText, QLineEdit::EchoMode EchoMode, bool Empty, int SelectedTextCount);
    virtual ~ISContextMenuText();
};
//-----------------------------------------------------------------------------
#endif
