#include "ISLabels.h"
#include "ISStyleSheet.h"
#include "ISConstants.h"
#include "ISGui.h"
#include "ISBuffer.h"
#include "ISLocalization.h"
#include "ISControls.h"
#include "ISDefinesGui.h"
//-----------------------------------------------------------------------------
ISQLabel::ISQLabel(const QString &Text, QWidget *parent)
	: QLabel(Text, parent),
	IsLinked(false),
	FocusPolicyDefault(focusPolicy()),
	Elided(false)
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
void ISQLabel::SetIsLinked(bool is_linked)
{
	IsLinked = is_linked;
	setFocusPolicy(IsLinked ? Qt::StrongFocus : FocusPolicyDefault);
	setStyleSheet(IsLinked ? STYLE_SHEET("ISLabelLink") : QString());
	setCursor(IsLinked ? CURSOR_POINTING_HAND : CURSOR_ARROW);
}
//-----------------------------------------------------------------------------
void ISQLabel::SetElided(bool elided)
{
	Elided = elided;
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
void ISQLabel::keyPressEvent(QKeyEvent *KeyEvent)
{
	QLabel::keyPressEvent(KeyEvent);
	if (IsLinked && KeyEvent->key() == Qt::Key_Space)
	{
		emit Clicked();
	}
}
//-----------------------------------------------------------------------------
void ISQLabel::enterEvent(QEvent *Event)
{
	QLabel::enterEvent(Event);
	if (IsLinked)
	{
		ISGui::SetFontWidgetUnderline(this, isEnabled());
	}
}
//-----------------------------------------------------------------------------
void ISQLabel::leaveEvent(QEvent *Event)
{
	QLabel::leaveEvent(Event);
	if (IsLinked)
	{
		ISGui::SetFontWidgetUnderline(this, !isEnabled());
	}
}
//-----------------------------------------------------------------------------
void ISQLabel::resizeEvent(QResizeEvent *ResizeEvent)
{
	QLabel::resizeEvent(ResizeEvent);
	if (Elided)
	{
		QString NewText = QFontMetrics(font()).elidedText(text(), Qt::ElideRight, width());
		setText(NewText);
		setToolTip(NewText.right(3) == "..." ? NewText : QString());
	}
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
	Layout = new QBoxLayout(QBoxLayout::Direction::LeftToRight);
	Layout->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);
	setLayout(Layout);

	LabelPixmap = new QLabel(this);
	LabelPixmap->setPixmap(Pixmap);
	LabelPixmap->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	Layout->addWidget(LabelPixmap);

	LabelText = new ISQLabel(Text, this);
	LabelText->adjustSize();
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
ISQLabel* ISLabelPixmapText::GetLabelText() const
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
