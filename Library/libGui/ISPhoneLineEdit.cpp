#include "ISPhoneLineEdit.h"
#include "EXDefines.h"
#include "ISBuffer.h"
#include "ISStyleSheet.h"
#include "ISControls.h"
#include "ISPhoneNumberParser.h"
#include "ISLocalization.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISPhoneLineEdit::ISPhoneLineEdit(QWidget *parent) : QLineEdit(parent)
{
	LastPosition = 0;

	setInputMask("(000) 000-00-00;_");
	setTextMargins(40, 0, 0, 0);
	setStyleSheet(STYLE_SHEET("ISLineEdit"));
	setFixedHeight(SIZE_MINIMUM_HEIGHT_EDIT_FIELD);
	connect(this, &ISPhoneLineEdit::cursorPositionChanged, this, &ISPhoneLineEdit::CursorPositionChanged);

	ActionCut = ISControls::GetActionContextCut(this);
	connect(ActionCut, &QAction::triggered, this, &ISPhoneLineEdit::CutClicked);

	ActionCopy = ISControls::GetActionContextCopy(this);
	connect(ActionCopy, &QAction::triggered, this, &ISPhoneLineEdit::CopyClicked);

	ActionPaste = ISControls::GetActionContextPaste(this);
	connect(ActionPaste, &QAction::triggered, this, &ISPhoneLineEdit::PasteClicked);

	ActionClear = new QAction(this);
	ActionClear->setToolTip(LANG("Clear.Field"));
	ActionClear->setIcon(BUFFER_ICONS("ButtonClear"));
	connect(ActionClear, &QAction::triggered, this, &ISPhoneLineEdit::ClearClicked);
	addAction(ActionClear, QLineEdit::TrailingPosition);
}
//-----------------------------------------------------------------------------
ISPhoneLineEdit::~ISPhoneLineEdit()
{

}
//-----------------------------------------------------------------------------
void ISPhoneLineEdit::SetIcon(ISNamespace::PhoneIconType IconType)
{
	switch (IconType)
	{
	case ISNamespace::PIT_Standart: Icon = BUFFER_ICONS("Phone.Correcting.Standart").pixmap(SIZE_16_16); break;
	case ISNamespace::PIT_TRUE: Icon = BUFFER_ICONS("Phone.Correcting.True").pixmap(SIZE_16_16); break;
	case ISNamespace::PIT_FALSE: Icon = BUFFER_ICONS("Phone.Correcting.False").pixmap(SIZE_16_16); break;
	}
}
//-----------------------------------------------------------------------------
void ISPhoneLineEdit::ResetLastPosition()
{
	LastPosition = 0;
}
//-----------------------------------------------------------------------------
void ISPhoneLineEdit::CutClicked()
{
	CopyClicked();
	clear();
}
//-----------------------------------------------------------------------------
void ISPhoneLineEdit::CopyClicked()
{
	QApplication::clipboard()->setText("+7" + ISPhoneNumberParser::RemoveNotDigits(text()));
}
//-----------------------------------------------------------------------------
void ISPhoneLineEdit::PasteClicked()
{
	emit Pasted(QApplication::clipboard()->text());
}
//-----------------------------------------------------------------------------
void ISPhoneLineEdit::CursorPositionChanged(int OldPos, int NewPos)
{
    Q_UNUSED(OldPos);
	LastPosition = NewPos;
}
//-----------------------------------------------------------------------------
void ISPhoneLineEdit::paintEvent(QPaintEvent *e)
{
	QLineEdit::paintEvent(e);

	QPainter Painter(this);
	QPixmap Pixmap = Icon.pixmap(height() - 6, height() - 6);
	
	Painter.drawPixmap(2, 3, Pixmap);
	Painter.setPen(Qt::lightGray);
	Painter.drawLine(Pixmap.width() + 3, 3, Pixmap.width() + 3, height() - 4);
	Painter.setPen(Qt::black);
	Painter.drawText(25, 17, "+7");
}
//-----------------------------------------------------------------------------
void ISPhoneLineEdit::focusInEvent(QFocusEvent *e)
{
	QLineEdit::focusInEvent(e);

	int TextLenght = ISPhoneNumberParser::RemoveNotDigits(text()).length();
	if (LastPosition > 0 && LastPosition <= TextLenght) //Если номер телефона не закончили вводить
	{
		setCursorPosition(LastPosition);
	}
	else if (LastPosition == 0 || TextLenght == 10) //Номер телефона введен корректно или его нет вообще
	{
		QTimer::singleShot(10, this, &ISPhoneLineEdit::selectAll);
	}
	else
	{
		QTimer::singleShot(10, this, &ISPhoneLineEdit::selectAll);
	}
}
//-----------------------------------------------------------------------------
void ISPhoneLineEdit::keyPressEvent(QKeyEvent *e)
{
	if (e->modifiers() == Qt::CTRL && e->key() == Qt::Key_V || e->modifiers() == Qt::SHIFT && e->key() == Qt::Key_Insert) //Если происходит вставка номера
	{
		e->ignore();
		PasteClicked();
	}
	else //Обработка нажатия остальных кнопок
	{
		QLineEdit::keyPressEvent(e);
	}
}
//-----------------------------------------------------------------------------
void ISPhoneLineEdit::contextMenuEvent(QContextMenuEvent *e)
{
	QString PhoneString = ISPhoneNumberParser::RemoveNotDigits(text());
	if (PhoneString.length() == 0)
	{
		ActionCut->setEnabled(false);
		ActionCopy->setEnabled(false);
	}
	else if (PhoneString.length() == 10)
	{
		ActionCut->setEnabled(true);
		ActionCopy->setEnabled(true);
	}
	else
	{
		ActionCut->setEnabled(false);
		ActionCopy->setEnabled(false);
	}

	ISGui::SetWaitGlobalCursor(true);
	QMenu ContextMenu;
	ContextMenu.addAction(ActionCut);
	ContextMenu.addAction(ActionCopy);
	ContextMenu.addAction(ActionPaste);
	ISGui::SetWaitGlobalCursor(false);
	ContextMenu.exec(e->globalPos());
}
//-----------------------------------------------------------------------------
