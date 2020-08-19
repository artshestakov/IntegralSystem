#include "ISButtons.h"
#include "ISConstants.h"
#include "ISDefinesGui.h"
#include "ISBuffer.h"
#include "ISStyleSheet.h"
#include "ISLocalization.h"
#include "ISGui.h"
#include "ISControls.h"
//-----------------------------------------------------------------------------
ISPushButton::ISPushButton(const QIcon &Icon, const QString &Text, const QString &ToolTip, QWidget *parent) : QPushButton(Icon, Text, parent)
{
	setToolTip(ToolTip);
	setStyleSheet(STYLE_SHEET("ISPushButton"));
	setMinimumWidth(ISPUSHBUTTON_MINIMUM_WIDTH);
	setFixedHeight(ISPUSHBUTTON_MINIMUM_HEIGHT);
}
//-----------------------------------------------------------------------------
ISPushButton::ISPushButton(const QIcon &Icon, const QString &Text, QWidget *parent) : ISPushButton(Icon, Text, QString(), parent)
{

}
//-----------------------------------------------------------------------------
ISPushButton::ISPushButton(const QString &Text, QWidget *parent) : ISPushButton(QIcon(), Text, QString(), parent)
{

}
//-----------------------------------------------------------------------------
ISPushButton::ISPushButton(const QIcon &Icon, QWidget *parent) : ISPushButton(Icon, QString(), QString(), parent)
{

}
//-----------------------------------------------------------------------------
ISPushButton::ISPushButton(QWidget *parent) : ISPushButton(QIcon(), QString(), QString(), parent)
{

}
//-----------------------------------------------------------------------------
ISPushButton::~ISPushButton()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISButtonClear::ISButtonClear(QWidget *parent) : ISPushButton(parent)
{
	setFixedSize(ISDefines::Gui::SIZE_22_22);
	setCursor(CURSOR_POINTING_HAND);
	setToolTip(LANG("Clear.Field"));
	setFocusPolicy(Qt::NoFocus);
	setIcon(BUFFER_ICONS("ButtonClear"));
	setFlat(true);
}
//-----------------------------------------------------------------------------
ISButtonClear::~ISButtonClear()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISButtonClose::ISButtonClose(QWidget *parent) : ISPushButton(parent)
{
	setText(LANG("Close"));
	setIcon(BUFFER_ICONS("Close"));
}
//-----------------------------------------------------------------------------
ISButtonClose::~ISButtonClose()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISServiceButton::ISServiceButton(const QIcon &Icon, const QString &ToolTip, QWidget *parent) : ISPushButton(Icon, QString(), ToolTip, parent)
{
	setFixedSize(ISPUSHBUTTON_MINIMUM_HEIGHT, ISPUSHBUTTON_MINIMUM_HEIGHT);
	setCursor(CURSOR_POINTING_HAND);
}
//-----------------------------------------------------------------------------
ISServiceButton::ISServiceButton(const QIcon &Icon, QWidget *parent) : ISServiceButton(Icon, QString(), parent)
{

}
//-----------------------------------------------------------------------------
ISServiceButton::ISServiceButton(const QString &ToolTip, QWidget *parent) : ISServiceButton(QIcon(), ToolTip, parent)
{

}
//-----------------------------------------------------------------------------
ISServiceButton::ISServiceButton(QWidget *parent) : ISServiceButton(QIcon(), QString(), parent)
{

}
//-----------------------------------------------------------------------------
ISServiceButton::~ISServiceButton()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISToolButton::ISToolButton(QWidget *parent) : QToolButton(parent)
{
	setStyleSheet(STYLE_SHEET("ISPushButton"));
	setMinimumWidth(ISPUSHBUTTON_MINIMUM_WIDTH);
	setFixedHeight(ISPUSHBUTTON_MINIMUM_HEIGHT);
}
//-----------------------------------------------------------------------------
ISToolButton::~ISToolButton()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISButtonMainMenu::ISButtonMainMenu(QWidget *parent)
	: QToolButton(parent),
	IconDefault(BUFFER_ICONS("MainPanel.Menu")),
	IconActive(BUFFER_ICONS("MainPanel.Menu.Active"))
{
	setText(LANG("MainMenu"));
	setFont(ISDefines::Gui::FONT_TAHOMA_10);
	setAutoRaise(true);
	setIcon(IconDefault);
	setIconSize(ISDefines::Gui::SIZE_24_24);
	setCursor(CURSOR_POINTING_HAND);
	setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	setStyleSheet(STYLE_SHEET("QToolButtonMainMenu"));
	setPopupMode(QToolButton::InstantPopup);
}
//-----------------------------------------------------------------------------
ISButtonMainMenu::~ISButtonMainMenu()
{

}
//-----------------------------------------------------------------------------
void ISButtonMainMenu::enterEvent(QEvent *e)
{
	QToolButton::enterEvent(e);
	setIcon(IconActive);
}
//-----------------------------------------------------------------------------
void ISButtonMainMenu::leaveEvent(QEvent *e)
{
	QToolButton::leaveEvent(e);
	setIcon(IconDefault);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISButtonFile::ISButtonFile(QWidget *parent) : ISPushButton(parent)
{
	setAcceptDrops(true);
}
//-----------------------------------------------------------------------------
ISButtonFile::~ISButtonFile()
{

}
//-----------------------------------------------------------------------------
void ISButtonFile::dragEnterEvent(QDragEnterEvent *e)
{
	const QMimeData *MimeData = e->mimeData();
	if (MimeData->hasFormat("text/uri-list"))
	{
		if (MimeData->urls().count() > 1) //Если пользователь перетаскивает более одного файла
		{
			return;
		}
		else
		{
			e->acceptProposedAction();
		}
	}
}
//-----------------------------------------------------------------------------
void ISButtonFile::dropEvent(QDropEvent *e)
{
	const QMimeData* MimeData = e->mimeData();
	if (MimeData->hasUrls())
	{
		emit DragAndDropFile(MimeData->urls()[0].toString(QUrl::PreferLocalFile));
	}
}
//-----------------------------------------------------------------------------
void ISButtonFile::dragMoveEvent(QDragMoveEvent *e)
{
	if (e->mimeData()->hasUrls())
	{
		e->acceptProposedAction();
	}
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISParagraphButton::ISParagraphButton(ISMetaParagraph *MetaParagraph, QWidget *parent) : QWidget(parent)
{
	QVBoxLayout *Layout = new QVBoxLayout();
	Layout->setSpacing(0);
	Layout->setContentsMargins(0, 3, 0, 0);
	setLayout(Layout);

	QIcon Icon = BUFFER_ICONS(MetaParagraph->Icon);
	Icon.addPixmap(BUFFER_ICONS(MetaParagraph->Icon + ".Active").pixmap(ISDefines::Gui::SIZE_32_32), QIcon::Active);

	ToolButton = new QToolButton(this);
	ToolButton->setToolTip(MetaParagraph->ToolTip);
	ToolButton->setIcon(Icon);
	ToolButton->setAutoRaise(true);
	ToolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	ToolButton->setIconSize(ISDefines::Gui::SIZE_32_32);
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
	ISControls::SetBackgroundColorWidget(LineLeft, ISDefines::Gui::COLOR_WHITE);

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
	ISControls::SetBackgroundColorWidget(LineRight, ISDefines::Gui::COLOR_WHITE);

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
