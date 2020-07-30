#include "ISImageWidget.h"
#include "ISDefinesGui.h"
#include "ISContextMenu.h"
#include "ISStyleSheet.h"
#include "ISFileDialog.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISImageViewerForm.h"
#include "ISNetwork.h"
#include "ISMessageBox.h"
#include "ISProcessForm.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISImageWidget::ISImageWidget(QWidget *parent) : QLabel(parent)
{
	setStyleSheet(STYLE_SHEET("ISLabelImage"));
	setAlignment(Qt::AlignCenter);
	setMouseTracking(true);
	setAutoFillBackground(true);
	setAcceptDrops(true);
	setPixmap(BUFFER_PIXMAPS("ImageEditBackgroup"));
}
//-----------------------------------------------------------------------------
ISImageWidget::~ISImageWidget()
{

}
//-----------------------------------------------------------------------------
void ISImageWidget::contextMenuEvent(QContextMenuEvent *e)
{
	ISGui::SetWaitGlobalCursor(true);
	ISContextMenuImage ContextMenu(this, ByteArray.isNull());
	connect(&ContextMenu, &ISContextMenuImage::Select, this, &ISImageWidget::Select);
	connect(&ContextMenu, &ISContextMenuImage::Cut, this, &ISImageWidget::Cut);
	connect(&ContextMenu, &ISContextMenuImage::Copy, this, &ISImageWidget::Copy);
	connect(&ContextMenu, &ISContextMenuImage::Paste, this, &ISImageWidget::Paste);
	connect(&ContextMenu, &ISContextMenuImage::PasteFromLink, this, &ISImageWidget::PasteFromLink);
	connect(&ContextMenu, &ISContextMenuImage::Save, this, &ISImageWidget::Save);
	connect(&ContextMenu, &ISContextMenuImage::OpenView, this, &ISImageWidget::OpenView);
	ISGui::SetWaitGlobalCursor(false);
	ContextMenu.exec(e->globalPos());
}
//-----------------------------------------------------------------------------
void ISImageWidget::dragEnterEvent(QDragEnterEvent *e)
{
	const QMimeData *MimeData = e->mimeData();
	if (MimeData->hasFormat("text/uri-list"))
	{
		if (MimeData->urls().count() > 1) //Если пользователь перетаскивает более одного файла
		{
			return;
		}

		QString FilePath = MimeData->urls().first().toString();
		if (!QStringList(ISDefines::Gui::AVIABLE_IMAGE_EXTENSION).contains(QFileInfo(FilePath).suffix().toLower())) //Если происходит перетаскивание не изображения
		{
			return;
		}
		e->acceptProposedAction();
	}
}
//-----------------------------------------------------------------------------
void ISImageWidget::dropEvent(QDropEvent *e)
{
	const QMimeData* MimeData = e->mimeData();
	if (MimeData->hasUrls())
	{
		QString FilePath = MimeData->urls().first().toString(QUrl::PreferLocalFile);
		if (ReadFile(FilePath))
		{
			SetPixmap();
		}
	}
}
//-----------------------------------------------------------------------------
void ISImageWidget::dragMoveEvent(QDragMoveEvent *e)
{
	if (e->mimeData()->hasUrls())
	{
		e->acceptProposedAction();
	}
}
//-----------------------------------------------------------------------------
void ISImageWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
	QLabel::mouseDoubleClickEvent(e);
	if (e->button() == Qt::LeftButton)
	{
		ByteArray.isNull() ? Select() : OpenView();
	}
}
//-----------------------------------------------------------------------------
QByteArray ISImageWidget::GetImage() const
{
	return ByteArray;
}
//-----------------------------------------------------------------------------
void ISImageWidget::SetByteArray(const QByteArray &byte_array)
{
	ByteArray = byte_array;
	SetPixmap();
}
//-----------------------------------------------------------------------------
void ISImageWidget::SetPixmap()
{
	QPixmap Pixmap;
	if (Pixmap.loadFromData(ByteArray, "PNG"))
	{
		SetPixmap(Pixmap.scaled(size(), Qt::KeepAspectRatio));
	}
	else
	{
		ISMessageBox::ShowCritical(this, LANG("Message.Error.LoadPixmapFromBuffer"));
	}
}
//-----------------------------------------------------------------------------
void ISImageWidget::SetPixmap(const QPixmap &Pixmap)
{
	setPixmap(Pixmap.scaled(size(), Qt::KeepAspectRatio));
	emit ImageChanged();
}
//-----------------------------------------------------------------------------
void ISImageWidget::Clear()
{
	ISGui::SetWaitGlobalCursor(true);
	ByteArray.clear();
	setPixmap(BUFFER_PIXMAPS("ImageEditBackgroup"));
	ISGui::SetWaitGlobalCursor(false);
	emit ImageChanged();
}
//-----------------------------------------------------------------------------
void ISImageWidget::Select()
{
	QString Path = ISFileDialog::GetOpenFileNameImage(this);
	if (!Path.isEmpty())
	{
		if (ReadFile(Path))
		{
			SetPixmap();
		}
	}
}
//-----------------------------------------------------------------------------
void ISImageWidget::Cut()
{
	Copy();
	Clear();
}
//-----------------------------------------------------------------------------
void ISImageWidget::Copy()
{
	QApplication::clipboard()->setPixmap(ByteArrayToPixmap());
}
//-----------------------------------------------------------------------------
void ISImageWidget::Paste()
{
	QPixmap Pixmap = QApplication::clipboard()->pixmap();
	if (!Pixmap.isNull())
	{
		ByteArray = ISGui::PixmapToByteArray(Pixmap);
		SetPixmap(Pixmap);
	}
}
//-----------------------------------------------------------------------------
void ISImageWidget::PasteFromLink()
{
	QString Url = QApplication::clipboard()->text();
	if (!Url.isEmpty())
	{
		ISProcessForm ProcessForm(LANG("DownloadingImage"));
		ProcessForm.show();

		ISNetwork Network;
		if (Network.DownloadFile(Url, ByteArray))
		{
			ProcessForm.SetText(LANG("ImageOpening"));

			QPixmap Pixmap;
			if (Pixmap.loadFromData(ByteArray))
			{
				SetPixmap(Pixmap);
			}
			else
			{
				ProcessForm.hide();
				ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotOpenedImage"));
			}
		}
		else
		{
			ProcessForm.hide();
			ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotDownloadedImage"), Network.GetErrorString());
		}
	}
	else
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotExistLinkInBuffer"));
	}
}
//-----------------------------------------------------------------------------
void ISImageWidget::Save()
{
	QString Path = ISFileDialog::GetSaveFileNameImage(this);
	if (!Path.isEmpty())
	{
		QFile File(Path);
		if (File.open(QIODevice::WriteOnly))
		{
			File.write(ByteArray);
			File.close();
		}
		else
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.SaveImageFile"), File.errorString());
		}
	}
}
//-----------------------------------------------------------------------------
void ISImageWidget::OpenView()
{
	ISGui::SetWaitGlobalCursor(true);
	(new ISImageViewerForm(ISGui::ByteArrayToPixmap(ByteArray)))->show();
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
bool ISImageWidget::ReadFile(const QString &FilePath)
{
	QFile File(FilePath);
	bool Result = File.open(QIODevice::ReadOnly);
	if (Result)
	{
		ByteArray = File.readAll();
		File.close();
	}
	else
	{
		ISMessageBox::ShowCritical(this, LANG("Message.Error.OpenImageFile"), File.errorString());
	}
	return Result;
}
//-----------------------------------------------------------------------------
QPixmap ISImageWidget::ByteArrayToPixmap()
{
	QPixmap Pixmap;
	if (!Pixmap.loadFromData(ByteArray, "PNG"))
	{
		ISMessageBox::ShowCritical(this, LANG("Message.Error.LoadPixmapFromBuffer"));
	}
	return Pixmap;

}
//-----------------------------------------------------------------------------
