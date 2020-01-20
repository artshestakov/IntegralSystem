#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISAttachFileBaseForm.h"
#include "ISPushButton.h"
#include "ISListWidget.h"
//-----------------------------------------------------------------------------
class ISAttachFilesForm : public ISAttachFileBaseForm
{
	Q_OBJECT

signals:
	void RemoveBeginItem();

public:
	ISAttachFilesForm(QWidget *parent = 0);
	virtual ~ISAttachFilesForm();

protected:
	void UpdateButtons();
	void SelectFiles();
	void Clean();
	void Delete();

	void StartDownload();
	void StopDownload();

private:
	ISListWidget *ListWidget;
	QLabel *LabelFiles;
	QProgressBar *ProgressFiles;
	QProgressBar *ProgressFile;

	ISPushButton *ButtonSelect;
	ISPushButton *ButtonClean;
	ISPushButton *ButtonStart;
	ISPushButton *ButtonStop;
	ISPushButton *ButtonClose;
};
//-----------------------------------------------------------------------------
