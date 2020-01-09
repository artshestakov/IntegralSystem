#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISInterfaceDialogForm.h"
#include "ISButtonDialog.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISUpdateDownloadForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISUpdateDownloadForm(int file_id, const QString &file_name, const QString &version, QWidget *parent = 0);
	virtual ~ISUpdateDownloadForm();

protected:
	void EnterClicked() override;
	void SetStatusText(const QString &Text);
	void Start();
	void Close();

private:
	QLabel *LabelStatus;
	QProgressBar *ProgressBar;
	ISButtonDialog *ButtonDialog;

	int FileID;
	QString FileName;
};
//-----------------------------------------------------------------------------
