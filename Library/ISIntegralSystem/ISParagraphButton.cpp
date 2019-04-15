#include "StdAfx.h"
#include "ISParagraphButton.h"
#include "EXDefines.h"
#include "ISStyleSheet.h"
#include "ISBuffer.h"
#include "ISControls.h"
//-----------------------------------------------------------------------------
ISParagraphButton::ISParagraphButton(ISMetaParagraph *MetaParagraph, QWidget *parent) : QWidget(parent)
{
	QVBoxLayout *Layout = new QVBoxLayout();
	Layout->setSpacing(0);
	Layout->setContentsMargins(0, 3, 0, 0);
	setLayout(Layout);

	QIcon Icon = BUFFER_ICONS(MetaParagraph->GetIcon());
	Icon.addPixmap(BUFFER_ICONS(MetaParagraph->GetIcon() + ".Active").pixmap(SIZE_32_32), QIcon::Active);

	ToolButton = new QToolButton(this);
	ToolButton->setToolTip(MetaParagraph->GetToolTip());
	ToolButton->setIcon(Icon);
	ToolButton->setAutoRaise(true);
	ToolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	ToolButton->setIconSize(SIZE_32_32);
	ToolButton->setCursor(CURSOR_POINTING_HAND);
	ToolButton->setStyleSheet(STYLE_SHEET("ISParagraphButton"));
	connect(ToolButton, &QToolButton::clicked, this, &ISParagraphButton::Clicked);
	Layout->addWidget(ToolButton, 0, Qt::AlignHCenter);

	QHBoxLayout *LayoutLine = new QHBoxLayout();
	Layout->addLayout(LayoutLine, 0);

	LineLeft = new QWidget(this);
	LineLeft->setAutoFillBackground(true);
	LineLeft->setFixedHeight(5);
	LineLeft->setFixedWidth(45);
	LineLeft->setSizePolicy(QSizePolicy::Minimum, LineLeft->sizePolicy().verticalPolicy());
	LayoutLine->addWidget(LineLeft, 0, Qt::AlignBottom);
	ISControls::SetBackgroundColorWidget(LineLeft, COLOR_WHITE);

	LineCenter = new QLabel(this);
	LineCenter->setPixmap(BUFFER_PIXMAPS("MainMenuButton.Bottom"));
	LineCenter->setFixedSize(LineCenter->pixmap()->size());
	LayoutLine->addWidget(LineCenter);

	LineRight = new QWidget(this);
	LineRight->setAutoFillBackground(true);
	LineRight->setFixedHeight(5);
	LineRight->setFixedWidth(45);
	LineRight->setSizePolicy(QSizePolicy::Minimum, LineRight->sizePolicy().verticalPolicy());
	LayoutLine->addWidget(LineRight, 0, Qt::AlignBottom);
	ISControls::SetBackgroundColorWidget(LineRight, COLOR_WHITE);

	setFixedWidth(LineLeft->width() + LineCenter->width() + LineRight->width());
	SetVisibleLine(false);
}
//-----------------------------------------------------------------------------
ISParagraphButton::~ISParagraphButton()
{

}
//-----------------------------------------------------------------------------
void ISParagraphButton::SetText(const QString &Text)
{
	ToolButton->setText(Text);
}
//-----------------------------------------------------------------------------
void ISParagraphButton::SetToolTip(const QString &ToolTip)
{
	ToolButton->setToolTip(ToolTip);
}
//-----------------------------------------------------------------------------
void ISParagraphButton::SetCursor(const QCursor &Cursor)
{
	ToolButton->setCursor(Cursor);
}
//-----------------------------------------------------------------------------
void ISParagraphButton::SetVisibleLine(bool Visible)
{
	LineLeft->setVisible(Visible);
	LineCenter->setVisible(Visible);
	LineRight->setVisible(Visible);
}
//-----------------------------------------------------------------------------
