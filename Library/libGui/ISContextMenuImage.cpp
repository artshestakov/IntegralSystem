#include "StdAfx.h"
#include "ISContextMenuImage.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISControls.h"
//-----------------------------------------------------------------------------
ISContextMenuImage::ISContextMenuImage(QWidget *parent, bool PixmapIsNull) : QMenu(parent)
{
	setToolTipsVisible(true);

	//�������
	QAction *ActionSelect = new QAction(this);
	ActionSelect->setText(LOCALIZATION("ContextMenu.Image.SelectFile"));
	ActionSelect->setToolTip(LOCALIZATION("ContextMenu.Image.SelectFile.ToolTip"));
	ActionSelect->setIcon(BUFFER_ICONS("Select"));
	connect(ActionSelect, &QAction::triggered, this, &ISContextMenuImage::Select);
	addAction(ActionSelect);

	addAction(ISControls::CreateSeparator(this));

	//��������
	QAction *ActionCut = new QAction(this);
	ActionCut->setEnabled(!PixmapIsNull);
	ActionCut->setText(LOCALIZATION("ContextMenu.Image.Cut"));
	ActionCut->setToolTip(LOCALIZATION("ContextMenu.Image.Cut.ToolTip"));
	ActionCut->setIcon(BUFFER_ICONS("Cut"));
	ActionCut->setShortcut(QKeySequence::Cut);
	connect(ActionCut, &QAction::triggered, this, &ISContextMenuImage::Cut);
	addAction(ActionCut);

	//����������
	QAction *ActionCopy = new QAction(this);
	ActionCopy->setEnabled(!PixmapIsNull);
	ActionCopy->setText(LOCALIZATION("ContextMenu.Image.Copy"));
	ActionCopy->setToolTip(LOCALIZATION("ContextMenu.Image.Copy.ToolTip"));
	ActionCopy->setIcon(BUFFER_ICONS("Copy"));
	ActionCopy->setShortcut(QKeySequence::Copy);
	connect(ActionCopy, &QAction::triggered, this, &ISContextMenuImage::Copy);
	addAction(ActionCopy);

	//��������
	QAction *ActionPaste = new QAction(this);
	ActionPaste->setEnabled(!QApplication::clipboard()->pixmap().isNull());
	ActionPaste->setText(LOCALIZATION("ContextMenu.Image.Paste"));
	ActionPaste->setToolTip(LOCALIZATION("ContextMenu.Image.Paste.ToolTip"));
	ActionPaste->setIcon(BUFFER_ICONS("Paste"));
	ActionPaste->setShortcut(QKeySequence::Paste);
	connect(ActionPaste, &QAction::triggered, this, &ISContextMenuImage::Paste);
	addAction(ActionPaste);

	//�������� �� ������
	QAction *ActionPasteFromLink = new QAction(this);
	ActionPasteFromLink->setText(LOCALIZATION("ContextMenu.Image.PasteFromLink"));
	ActionPasteFromLink->setIcon(BUFFER_ICONS("PasteFromLink"));
	connect(ActionPasteFromLink, &QAction::triggered, this, &ISContextMenuImage::PasteFromLink);
	addAction(ActionPasteFromLink);

	addAction(ISControls::CreateSeparator(this));

	//���������
	QAction *ActionSave = ISControls::CreateActionSave(this);
	ActionSave->setEnabled(!PixmapIsNull);
	connect(ActionSave, &QAction::triggered, this, &ISContextMenuImage::Save);
	addAction(ActionSave);

	//��������
	QAction *ActionOpenView = new QAction(this);
	ActionOpenView->setEnabled(!PixmapIsNull);
	ActionOpenView->setText(LOCALIZATION("ViewImage"));
	ActionOpenView->setToolTip(LOCALIZATION("ViewImage"));
	ActionOpenView->setIcon(BUFFER_ICONS("Image"));
	connect(ActionOpenView, &QAction::triggered, this, &ISContextMenuImage::OpenView);
	addAction(ActionOpenView);
}
//-----------------------------------------------------------------------------
ISContextMenuImage::~ISContextMenuImage()
{

}
//-----------------------------------------------------------------------------
