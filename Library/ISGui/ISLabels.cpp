#include "ISLabels.h"
#include "ISStyleSheet.h"
#include "ISConstants.h"
#include "ISGui.h"
#include "ISBuffer.h"
#include "ISLocalization.h"
#include "ISControls.h"
#include "ISDefinesGui.h"
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
	if (hasSelectedText() && MouseEvent->button() == Qt::LeftButton) //Если текст был выбран - вызываем меню для предложения копирования
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
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISLabelPixmapText::ISLabelPixmapText(const QPixmap &Pixmap, const QString &Text, QWidget *parent) : QWidget(parent)
{
	setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

	Layout = new QBoxLayout(QBoxLayout::Direction::LeftToRight);
	Layout->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);
	setLayout(Layout);

	LabelPixmap = new QLabel(this);
	LabelPixmap->setPixmap(Pixmap);
	Layout->addWidget(LabelPixmap);

	LabelText = new QLabel(Text, this);
	Layout->addWidget(LabelText);
}
//-----------------------------------------------------------------------------
ISLabelPixmapText::ISLabelPixmapText(const QPixmap &Pixmap, QWidget *parent) : ISLabelPixmapText(Pixmap, QString(), parent)
{

}
//-----------------------------------------------------------------------------
ISLabelPixmapText::ISLabelPixmapText(const QString &Text, QWidget *parent) : ISLabelPixmapText(QPixmap(), Text, parent)
{

}
//-----------------------------------------------------------------------------
ISLabelPixmapText::ISLabelPixmapText(QWidget *parent) : ISLabelPixmapText(QPixmap(), QString(), parent)
{

}
//-----------------------------------------------------------------------------
ISLabelPixmapText::~ISLabelPixmapText()
{

}
//-----------------------------------------------------------------------------
QLabel* ISLabelPixmapText::GetLabelPixmap() const
{
	return LabelPixmap;
}
//-----------------------------------------------------------------------------
QLabel* ISLabelPixmapText::GetLabelText() const
{
	return LabelText;
}
//-----------------------------------------------------------------------------
void ISLabelPixmapText::SetDirection(QBoxLayout::Direction Direction)
{
	Layout->setDirection(Direction);
}
//-----------------------------------------------------------------------------
void ISLabelPixmapText::SetPixmapText(const QPixmap &Pixmap, const QString &Text)
{
	LabelPixmap->setPixmap(Pixmap);
	LabelText->setText(Text);
}
//-----------------------------------------------------------------------------
void ISLabelPixmapText::SetPixmap(const QPixmap &Pixmap)
{
	LabelPixmap->setPixmap(Pixmap);
}
//-----------------------------------------------------------------------------
void ISLabelPixmapText::SetText(const QString &Text)
{
	LabelText->setText(Text);
}
//-----------------------------------------------------------------------------
QFont ISLabelPixmapText::GetFont() const
{
	return LabelText->font();
}
//-----------------------------------------------------------------------------
void ISLabelPixmapText::SetFont(const QFont &Font)
{
	LabelText->setFont(Font);
}
//-----------------------------------------------------------------------------
void ISLabelPixmapText::Clear()
{
	SetPixmapText(QPixmap(), QString());
}
//-----------------------------------------------------------------------------
void ISLabelPixmapText::ClearPixmap()
{
	SetPixmap(QPixmap());
}
//-----------------------------------------------------------------------------
void ISLabelPixmapText::ClearText()
{
	SetText(QString());
}
//-----------------------------------------------------------------------------
