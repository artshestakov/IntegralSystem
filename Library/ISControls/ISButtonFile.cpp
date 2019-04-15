#include "StdAfx.h"
#include "ISButtonFile.h"
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
		QString FilePath = MimeData->urls()[0].toString(QUrl::PreferLocalFile);
		emit DragAndDropFile(FilePath);
	}
}
//-----------------------------------------------------------------------------
void ISButtonFile::dragMoveEvent(QDragMoveEvent *e)
{
	const QMimeData *MimeData = e->mimeData();
	if (MimeData->hasUrls())
	{
		e->acceptProposedAction();
	}
}
//-----------------------------------------------------------------------------
