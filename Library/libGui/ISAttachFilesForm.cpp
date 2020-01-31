#include "StdAfx.h"
#include "ISAttachFilesForm.h"
#include "ISDefines.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISControls.h"
#include "ISFileDialog.h"
#include "ISAttachFileItem.h"
#include "ISGui.h"
#include "ISStorageFileLoader.h"
//-----------------------------------------------------------------------------
ISAttachFilesForm::ISAttachFilesForm(QWidget *parent) : ISAttachFileBaseForm(parent)
{
	setWindowTitle(LANG("AddingFiles"));
	resize(SIZE_MAIN_WINDOW_MINIMUM);
	GetMainLayout()->setContentsMargins(LAYOUT_MARGINS_10_PX);

	QHBoxLayout *Layout = new QHBoxLayout();
	GetMainLayout()->addLayout(Layout);

	ButtonSelect = new ISPushButton(this);
	ButtonSelect->setText(LANG("ListForm.Select") + "...");
	connect(ButtonSelect, &ISPushButton::clicked, this, &ISAttachFilesForm::SelectFiles);
	Layout->addWidget(ButtonSelect);

	ButtonClean = new ISPushButton(this);
	ButtonClean->setEnabled(false);
	ButtonClean->setText(LANG("Clear"));
	connect(ButtonClean, &ISPushButton::clicked, this, &ISAttachFilesForm::Clean);
	Layout->addWidget(ButtonClean);

	Layout->addStretch();

	ListWidget = new ISListWidget(this);
	GetMainLayout()->addWidget(ListWidget);

	LabelFiles = new QLabel(this);
	LabelFiles->setText(LANG("FilesCount") + ": 0");
	GetMainLayout()->addWidget(LabelFiles, 0, Qt::AlignLeft);

	GetMainLayout()->addWidget(ISControls::CreateHorizontalLine(this));

	ProgressFiles = new QProgressBar(this);
	ProgressFiles->setValue(0);
	GetMainLayout()->addWidget(ProgressFiles);

	ProgressFile = new QProgressBar(this);
	ProgressFile->setValue(0);
	GetMainLayout()->addWidget(ProgressFile);

	GetMainLayout()->addWidget(ISControls::CreateHorizontalLine(this));

	QHBoxLayout *LayoutBottom = new QHBoxLayout();
	LayoutBottom->addStretch();
	GetMainLayout()->addLayout(LayoutBottom);

	ButtonStart = new ISPushButton(this);
	ButtonStart->setEnabled(false);
	ButtonStart->setText(LANG("Download"));
	connect(ButtonStart, &ISPushButton::clicked, this, &ISAttachFilesForm::StartDownload);
	LayoutBottom->addWidget(ButtonStart);

	ButtonStop = new ISPushButton(this);
	ButtonStop->setEnabled(false);
	ButtonStop->setText(LANG("Stop"));
	connect(ButtonStop, &ISPushButton::clicked, this, &ISAttachFilesForm::StopDownload);
	LayoutBottom->addWidget(ButtonStop);

	ButtonClose = new ISPushButton(this);
	ButtonClose->setText(LANG("Close"));
	ButtonClose->setIcon(BUFFER_ICONS("Close"));
	connect(ButtonClose, &ISPushButton::clicked, this, &ISAttachFilesForm::close);
	LayoutBottom->addWidget(ButtonClose);

	connect(this, &ISAttachFilesForm::RemoveBeginItem, ListWidget, &ISListWidget::RemoveBeginItem);
}
//-----------------------------------------------------------------------------
ISAttachFilesForm::~ISAttachFilesForm()
{

}
//-----------------------------------------------------------------------------
void ISAttachFilesForm::UpdateButtons()
{
	int Count = ListWidget->count();
	if (Count)
	{
		ButtonClean->setEnabled(true);
		ButtonStart->setEnabled(true);

		ProgressFiles->setMaximum(Count);
	}
	else
	{
		ButtonClean->setEnabled(false);
		ButtonStart->setEnabled(false);
	}

	LabelFiles->setText(LANG("FilesCount") + ": " + QString::number(Count));
}
//-----------------------------------------------------------------------------
void ISAttachFilesForm::SelectFiles()
{
	QStringList StringList = ISFileDialog::GetOpenFilesName(this);

	ISGui::SetWaitGlobalCursor(true);

	for (int i = 0; i < StringList.count(); ++i)
	{
		ISAttachFileItem *FileItem = new ISAttachFileItem(this);
		FileItem->SetFilePath(StringList.at(i));
		connect(FileItem, &ISAttachFileItem::Delete, this, &ISAttachFilesForm::Delete);

		QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);
		ListWidgetItem->setSizeHint(FileItem->sizeHint());
		ListWidget->setItemWidget(ListWidgetItem, FileItem);

		FileItem->SetItemIndex(ListWidget->row(ListWidgetItem));

		ISGui::ProcessEvents();
	}

	UpdateButtons();

	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISAttachFilesForm::Clean()
{
	ISGui::SetWaitGlobalCursor(true);
	ListWidget->Clear();
	UpdateButtons();
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISAttachFilesForm::Delete()
{
	ISAttachFileItem *FileItem = dynamic_cast<ISAttachFileItem*>(sender());
	
	QListWidgetItem *ListWidgetItem = ListWidget->item(FileItem->GetItemIndex());
	ListWidget->setItemWidget(ListWidgetItem, nullptr);

	delete FileItem;
	FileItem = nullptr;

	delete ListWidgetItem;
	ListWidgetItem = nullptr;

	UpdateButtons();
}
//-----------------------------------------------------------------------------
void ISAttachFilesForm::StartDownload()
{
	ButtonStart->setEnabled(false);
	ButtonStop->setEnabled(true);
	ButtonClose->setEnabled(false);

	while (ListWidget->count())
	{
		QListWidgetItem *ListWidgetItem = ListWidget->item(0);
		ISAttachFileItem *FileItem = dynamic_cast<ISAttachFileItem*>(ListWidget->itemWidget(ListWidgetItem));
		QString FilePath = FileItem->GetFilePath();
		FileItem->SetFilePath(FileItem->GetFilePath() + SYMBOL_SPACE + LANG("Loaded") + "...");
		FileItem->SetFont(FONT_APPLICATION_BOLD);
		ISGui::ProcessEvents();

		ISStorageFileLoader StorageFileLoader(FilePath, QString(), this);
		connect(&StorageFileLoader, &ISStorageFileLoader::ChangeProgressFileMaximum, ProgressFile, &QProgressBar::setMaximum);
		connect(&StorageFileLoader, &ISStorageFileLoader::LoadingFile, [=]	{ ProgressFile->setValue(ProgressFile->value() + 1); });
		StorageFileLoader.Load();

		emit RemoveBeginItem();

		ProgressFiles->setValue(ProgressFiles->value() + 1);
		ISGui::ProcessEvents();
	}

	ButtonStop->setEnabled(false);
	ButtonClose->setEnabled(true);
	
	ProgressFile->setValue(0);
	ProgressFiles->setValue(0);
	UpdateButtons();

	emit UpdateList();
}
//-----------------------------------------------------------------------------
void ISAttachFilesForm::StopDownload()
{

}
//-----------------------------------------------------------------------------
