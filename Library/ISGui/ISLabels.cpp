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
void ISQLabel::paintEvent(QPaintEvent *PaintEvent)
{
	QLabel::paintEvent(PaintEvent);
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
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISLabelElided::ISLabelElided(const QString &Text, QWidget *parent)
	: QFrame(parent),
	Elided(false),
	ElidedToolTip(false),
	Content(Text),
	ColorText(ISDefines::Gui::COLOR_BLACK)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}
//-----------------------------------------------------------------------------
ISLabelElided::ISLabelElided(QWidget *parent) : ISLabelElided(QString(), parent)
{

}
//-----------------------------------------------------------------------------
ISLabelElided::~ISLabelElided()
{

}
//-----------------------------------------------------------------------------
void ISLabelElided::SetText(const QString &Text)
{
	Content = Text;
	update();
}
//-----------------------------------------------------------------------------
const QString& ISLabelElided::GetText() const
{
	return Content;
}
//-----------------------------------------------------------------------------
void ISLabelElided::SetColorText(QColor &Color)
{
	ColorText = Color;
}
//-----------------------------------------------------------------------------
void ISLabelElided::SetElidedToolTip(bool elided_tool_tip)
{
	ElidedToolTip = elided_tool_tip;
}
//-----------------------------------------------------------------------------
bool ISLabelElided::GetElidedToolTip() const
{
	return ElidedToolTip;
}
//-----------------------------------------------------------------------------
bool ISLabelElided::IsElided() const
{
	return Elided;
}
//-----------------------------------------------------------------------------
void ISLabelElided::paintEvent(QPaintEvent *PaintEvent)
{
	QFrame::paintEvent(PaintEvent);

	QPainter Painter(this);
	Painter.setPen(QPen(ColorText));
	QFontMetrics FontMetrics = Painter.fontMetrics();

	bool DidElide = false;
	int LineSpacing = FontMetrics.lineSpacing();
	int Y = 0;

	QTextLayout TextLayout(Content, Painter.font());
	TextLayout.beginLayout();
	forever
	{
		QTextLine TextLine = TextLayout.createLine();
		if (!TextLine.isValid())
		{
			break;
		}
		TextLine.setLineWidth(width());
		int NextLineY = Y + LineSpacing;

		if (height() >= NextLineY + LineSpacing)
		{
			TextLine.draw(&Painter, QPoint(0, Y));
			Y = NextLineY;
		}
		else
		{
			QString LastLine = Content.mid(TextLine.textStart());
			QString ElidedLastLine = FontMetrics.elidedText(LastLine, Qt::ElideRight, width());
			Painter.drawText(QPoint(0, Y + FontMetrics.ascent()), ElidedLastLine);
			TextLine = TextLayout.createLine();
			DidElide = TextLine.isValid();
			break;
		}
	}
	TextLayout.endLayout();

	if (DidElide != Elided)
	{
		Elided = DidElide;
		setToolTip(Elided && ElidedToolTip ? ISGui::PrepareLongToolTip(Content, 400) : QString());
		emit ElisionChanged(DidElide);
	}
}
//-----------------------------------------------------------------------------
