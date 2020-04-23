#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
#include "ISTextEdit.h"
#include "ISListWidget.h"
//-----------------------------------------------------------------------------
class ISNoteForm : public ISInterfaceForm
{
	Q_OBJECT

public:
	ISNoteForm(QWidget *parent = 0);
	virtual ~ISNoteForm();

protected:
	void ItemSelectionChanged();
	void TextChanged();

	void Load();
	void Reload();
	void Create();
	void Edit();
	void Delete();
	void SaveClicked();
	bool SaveText(int NoteID, const QString &Text);

	QListWidgetItem* AddItem(const QString &Name = QString(), const QString &Text = QString());
	void SelectItem(int NoteID);
	void SetFontBoldItem(QListWidgetItem *Item, bool Bold);
	void SetNoteText(const QString &Text);
	void SetModificationFlag(bool modification_flag);

	void EscapeClicked() override;

private:
	QAction *ActionCreate;
	QAction *ActionEdit;
	QAction *ActionDelete;
	QAction *ActionSave;

	ISListWidget *ListWidget;
	ISTextEdit *TextEdit;

	bool ModificationFlag;
	QListWidgetItem *PreviousItem;
	QMap<QListWidgetItem*, int> Items;
};
//-----------------------------------------------------------------------------
