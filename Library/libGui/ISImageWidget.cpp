#include "ISImageWidget.h"
#include "ISDefinesGui.h"
#include "ISContextMenuImage.h"
#include "ISStyleSheet.h"
#include "ISFileDialog.h"
#include "ISSystem.h"
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
	SetBackgroundImage(BUFFER_ICONS("ImageEditBackgroup").pixmap(DEFINES_GUI.SIZE_64_64));
}
//-----------------------------------------------------------------------------
ISImageWidget::~ISImageWidget()
{

}
//-----------------------------------------------------------------------------
void ISImageWidget::contextMenuEvent(QContextMenuEvent *e)
{
	ISGui::SetWaitGlobalCursor(true);
	ISContextMenuImage ContextMenu(this, CurrentPixmap.isNull());
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

		QString FilePath = MimeData->urls()[0].toString();
		if (!QStringList(DEFINES_GUI.AVIABLE_IMAGE_EXTENSION).contains(QFileInfo(FilePath).suffix().toLower())) //Если происходит перетаскивание не изображения
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
		QString FilePath = MimeData->urls()[0].toString(QUrl::PreferLocalFile);
		SetPixmap(FilePath);
	}
}
//-----------------------------------------------------------------------------
void ISImageWidget::dragMoveEvent(QDragMoveEvent *e)
{
	const QMimeData *MimeData = e->mimeData();
	if (MimeData->hasUrls())
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
		if (CurrentPixmap.isNull())
		{
			Select();
		}
		else
		{
			OpenView();
		}
	}
}
//-----------------------------------------------------------------------------
void ISImageWidget::SetBackgroundImage(const QPixmap &Pixmap)
{
	setPixmap(Pixmap);
}
//-----------------------------------------------------------------------------
QPixmap ISImageWidget::GetPixmap() const
{
	return CurrentPixmap;
}
//-----------------------------------------------------------------------------
void ISImageWidget::SetPixmap(const QPixmap &Pixmap)
{
	ISGui::SetWaitGlobalCursor(true);

	Clear();
	CurrentPixmap = Pixmap;
	QPixmap CompletePixmap = Pixmap.scaled(DEFINES_GUI.SIZE_200_200, Qt::KeepAspectRatio);
	setPixmap(CompletePixmap);
	emit ImageChanged();

	if (!Pixmap.isNull())
	{
		setFocus();
	}

	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISImageWidget::SetPixmap(const QString &Path)
{
	QPixmap Pixmap;
	if (Pixmap.load(Path))
	{
		SetPixmap(Pixmap);
	}
}
//-----------------------------------------------------------------------------
void ISImageWidget::Clear()
{
	ISGui::SetWaitGlobalCursor(true);
	CurrentPixmap = QPixmap();
	setPixmap(QPixmap());
	emit ImageChanged();
	SetBackgroundImage(BUFFER_ICONS("ImageEditBackgroup").pixmap(DEFINES_GUI.SIZE_64_64));
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISImageWidget::Select()
{
	QString Path = ISFileDialog::GetOpenFileNameImage(this);
	if (Path.length())
	{
		SetPixmap(Path);
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
	QApplication::clipboard()->setPixmap(CurrentPixmap);
}
//-----------------------------------------------------------------------------
void ISImageWidget::Paste()
{
	ISGui::SetWaitGlobalCursor(true);
	SetPixmap(QApplication::clipboard()->pixmap());
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISImageWidget::PasteFromLink()
{
	QString Url = QApplication::clipboard()->text();
	if (Url.length())
	{
		if (ISGui::IsStringUrl(Url))
		{
			ISProcessForm ProcessForm;
			ProcessForm.show();
			ProcessForm.SetText(LANG("DownloadingImage"));

			QByteArray ByteArray;
			QString ErrorString;
			if (ISNetwork().DownloadFile(Url, ByteArray, ErrorString))
			{
				ProcessForm.SetText(LANG("ImageOpening"));

				QPixmap Pixmap;
				if (Pixmap.loadFromData(ByteArray))
				{
					SetPixmap(Pixmap);
				}
				else
				{
					ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotOpenedImage"));
				}
			}
			else
			{
				ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotDownloadedImage"));
			}
		}
		else
		{
			ISMessageBox::ShowWarning(this, LANG("Message.Warning.LinkInBufferInvalid"));
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
	if (Path.length())
	{
        CurrentPixmap.save(Path);
	}
}
//-----------------------------------------------------------------------------
void ISImageWidget::OpenView()
{
	ISGui::SetWaitGlobalCursor(true);
	ISImageViewerForm *ImageViewForm = new ISImageViewerForm(CurrentPixmap);
	ImageViewForm->show();
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
