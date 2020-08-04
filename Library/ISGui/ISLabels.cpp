#include "ISLabels.h"
#include "ISStyleSheet.h"
#include "ISConstants.h"
#include "ISGui.h"
#include "ISBuffer.h"
#include "ISLocalization.h"
#include "ISControls.h"
//-----------------------------------------------------------------------------
ISQLabel::ISQLabel(const QString &Text, QWidget *parent) : QLabel(Text, parent)
{

}
//-----------------------------------------------------------------------------
ISQLabel::ISQLabel(QWidget *parent) : QLabel(parent)
{

}
//-----------------------------------------------------------------------------
ISQLabel::~ISQLabel()
{

}
//-----------------------------------------------------------------------------
void ISQLabel::mouseReleaseEvent(QMouseEvent *MouseEvent)
{
	QLabel::mouseReleaseEvent(MouseEvent);
	if (MouseEvent->button() == Qt::LeftButton)
	{
		emit Clicked();
	}
}
//-----------------------------------------------------------------------------
void ISQLabel::mouseDoubleClickEvent(QMouseEvent *MouseEvent)
{
	QLabel::mouseDoubleClickEvent(MouseEvent);
	if (MouseEvent->button() == Qt::LeftButton)
	{
		emit DoubleClicked();
	}
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISLabelLink::ISLabelLink(const QString &Text, QWidget *parent) : ISQLabel(Text, parent)
{
	setFocusPolicy(Qt::StrongFocus);
	setStyleSheet(STYLE_SHEET("ISLabelLink"));
	setCursor(CURSOR_POINTING_HAND);
}
//-----------------------------------------------------------------------------
ISLabelLink::ISLabelLink(QWidget *parent) : ISLabelLink(QString(), parent)
{
	
}
//-----------------------------------------------------------------------------
ISLabelLink::~ISLabelLink()
{

}
//-----------------------------------------------------------------------------
void ISLabelLink::keyPressEvent(QKeyEvent *KeyEvent)
{
	ISQLabel::keyPressEvent(KeyEvent);
	if (KeyEvent->key() == Qt::Key_Space)
	{
		emit Clicked();
	}
}
//-----------------------------------------------------------------------------
void ISLabelLink::enterEvent(QEvent *Event)
{
	ISQLabel::enterEvent(Event);
	ISGui::SetFontWidgetUnderline(this, isEnabled());
}
//-----------------------------------------------------------------------------
void ISLabelLink::leaveEvent(QEvent *Event)
{
	ISQLabel::leaveEvent(Event);
	ISGui::SetFontWidgetUnderline(this, !isEnabled());
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISLabelSelectionText::ISLabelSelectionText(const QString &Text, QWidget *parent) : ISQLabel(Text, parent)
{
	setTextInteractionFlags(Qt::TextSelectableByMouse);
}
//-----------------------------------------------------------------------------
ISLabelSelectionText::ISLabelSelectionText(QWidget *parent) : ISLabelSelectionText(QString(), parent)
{

}
//-----------------------------------------------------------------------------
ISLabelSelectionText::~ISLabelSelectionText()
{

}
//-----------------------------------------------------------------------------
void ISLabelSelectionText::mouseReleaseEvent(QMouseEvent *MouseEvent)
{
	ISQLabel::mouseReleaseEvent(MouseEvent);
	if (hasSelectedText()) //Если текст был выбран - вызываем меню для предложения копирования
	{
		QMenu Menu;

		QAction *ActionCopy = ISControls::GetActionContextCopy(&Menu);
		connect(ActionCopy, &QAction::triggered, this, &ISLabelSelectionText::CopySelectedText);
		Menu.addAction(ActionCopy);

		Menu.exec(MouseEvent->globalPos());
	}
}
//-----------------------------------------------------------------------------
void ISLabelSelectionText::CopySelectedText()
{
	QApplication::clipboard()->setText(selectedText());
	setSelection(0, 0);
}
//-----------------------------------------------------------------------------
