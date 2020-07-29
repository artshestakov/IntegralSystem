#include "ISLabels.h"
#include "ISStyleSheet.h"
#include "ISConstants.h"
#include "ISGui.h"
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
void ISQLabel::mousePressEvent(QMouseEvent *e)
{
	if (e->button() == Qt::LeftButton)
	{
		emit Clicked();
	}
}
//-----------------------------------------------------------------------------
void ISQLabel::mouseDoubleClickEvent(QMouseEvent *e)
{
	if (e->button() == Qt::LeftButton)
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
void ISLabelLink::keyPressEvent(QKeyEvent *e)
{
	ISQLabel::keyPressEvent(e);
	if (e->key() == Qt::Key_Space)
	{
		emit Clicked();
	}
}
//-----------------------------------------------------------------------------
void ISLabelLink::enterEvent(QEvent *e)
{
	ISQLabel::enterEvent(e);
	ISGui::SetFontWidgetUnderline(this, isEnabled());
}
//-----------------------------------------------------------------------------
void ISLabelLink::leaveEvent(QEvent *e)
{
	ISQLabel::leaveEvent(e);
	ISGui::SetFontWidgetUnderline(this, !isEnabled());
}
//-----------------------------------------------------------------------------
