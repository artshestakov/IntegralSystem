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
ISButtonDialog::ISButtonDialog(QWidget *parent, const QString &ApplyText, const QString &CloseText) : QWidget(parent)
{
	QHBoxLayout *Layout = new QHBoxLayout();
	Layout->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_5_PX);
	Layout->addStretch();
	setLayout(Layout);

	ButtonApply = new ISPushButton(BUFFER_ICONS("Apply.Blue"), ApplyText.isEmpty() ? LANG("Apply") : ApplyText, this);
	connect(ButtonApply, &ISPushButton::clicked, this, &ISButtonDialog::Apply);
	Layout->addWidget(ButtonApply);

	ButtonClose = new ISPushButton(BUFFER_ICONS("Close"), CloseText.isEmpty() ? LANG("Close") : CloseText, this);
	connect(ButtonClose, &QAbstractButton::clicked, this, &ISButtonDialog::Close);
	Layout->addWidget(ButtonClose);
}
//-----------------------------------------------------------------------------
ISButtonDialog::~ISButtonDialog()
{

}
//-----------------------------------------------------------------------------
void ISButtonDialog::SetApplyIcon(const QIcon &ApplyIcon)
{
	ButtonApply->setIcon(ApplyIcon);
}
//-----------------------------------------------------------------------------
void ISButtonDialog::SetApplyEnabled(bool Enabled)
{
	ButtonApply->setEnabled(Enabled);
}
//-----------------------------------------------------------------------------
void ISButtonDialog::SetApplyVisible(bool Visible)
{
	ButtonApply->setVisible(Visible);
}
//-----------------------------------------------------------------------------
void ISButtonDialog::SetApplyCursor(const QCursor &Cursor)
{
	ButtonApply->setCursor(Cursor);
}
//-----------------------------------------------------------------------------
void ISButtonDialog::SetApplyFocus()
{
	ButtonApply->setFocus();
}
//-----------------------------------------------------------------------------
void ISButtonDialog::SetCloseIcon(const QIcon &Icon)
{
	ButtonClose->setIcon(Icon);
}
//-----------------------------------------------------------------------------
void ISButtonDialog::SetCloseEnabled(bool Enabled)
{
	ButtonClose->setEnabled(Enabled);
}
//-----------------------------------------------------------------------------
void ISButtonDialog::SetCloseCursor(const QCursor &Cursor)
{
	ButtonClose->setCursor(Cursor);
}
//-----------------------------------------------------------------------------
