#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISAttachFileBaseForm.h"
#include "ISPathEditFile.h"
#include "ISButtonDialog.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISAttachFileForm : public ISAttachFileBaseForm
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
