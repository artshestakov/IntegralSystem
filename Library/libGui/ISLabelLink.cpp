#include "ISLabelLink.h"
#include "ISDefines.h"
#include "ISStyleSheet.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISLabelLink::ISLabelLink(const QString &Text, QWidget *parent) : ISQLabel(Text, parent)
{

}
//-----------------------------------------------------------------------------
ISLabelLink::ISLabelLink(QWidget *parent) : ISQLabel(parent)
{
	setFocusPolicy(Qt::StrongFocus);
	setStyleSheet(STYLE_SHEET("ISLabelLink"));
	setCursor(CURSOR_POINTING_HAND);
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
	if (isEnabled())
	{
		ISGui::SetFontWidgetUnderline(this, true);
	}
}
//-----------------------------------------------------------------------------
void ISLabelLink::leaveEvent(QEvent *e)
{
	ISQLabel::leaveEvent(e);
	if (isEnabled())
	{
		ISGui::SetFontWidgetUnderline(this, false);
	}
}
//-----------------------------------------------------------------------------
