#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISNamespace.h"
//-----------------------------------------------------------------------------
class ISPhoneLineEdit : public QLineEdit
{
	Q_OBJECT

signals:
	void Pasted(const QString &String);
	void ClearClicked();

public:
	ISPhoneLineEdit(QWidget *parent = 0);
	virtual ~ISPhoneLineEdit();

	void SetIcon(ISNamespace::PhoneIconType IconType);
	void ResetLastPosition(); //—бросить индекс последней позции курсора

protected:
	void CutClicked();
	void CopyClicked();
	void PasteClicked();

	void CursorPositionChanged(int OldPos, int NewPos);
	void paintEvent(QPaintEvent *e);
	void focusInEvent(QFocusEvent *e);
	void keyPressEvent(QKeyEvent *e);
	void contextMenuEvent(QContextMenuEvent *e);

private:
	QIcon Icon;
	int LastPosition; //»ндекс последней позиции каретки курсора

	QAction *ActionCut;
	QAction *ActionCopy;
	QAction *ActionPaste;

	QAction *ActionClear;
};
//-----------------------------------------------------------------------------
