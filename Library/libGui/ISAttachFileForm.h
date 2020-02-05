#pragma once
#ifndef _ISATTACHFILEFORM_H_INCLUDED
#define _ISATTACHFILEFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISAttachFileBaseForm.h"
#include "ISPathEditFile.h"
#include "ISButtonDialog.h"
//-----------------------------------------------------------------------------
class ISAttachFileForm : public ISAttachFileBaseForm
{
	Q_OBJECT

signals:
	void Loaded(int FileID);

public:
	ISAttachFileForm(const QString &FilePath = QString(), QWidget *parent = 0);
	virtual ~ISAttachFileForm();

	void SetEnabledPath(bool Enabled);
	void SetFilePath(const QString &FilePath);

protected:
	void StartDownload();
	void EditFileChanged(const QVariant &value);

private:
	ISPathEditFile *PathFileEdit;
	ISLineEdit *NoteEdit;
	QProgressBar *ProgressBar;
	ISButtonDialog *ButtonDialog;
};
//-----------------------------------------------------------------------------
#endif
