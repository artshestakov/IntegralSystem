#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISAttachFileBaseForm.h"
#include "ISButtons.h"
#include "ISListWidget.h"
//-----------------------------------------------------------------------------
class ISAttachFilesForm : public ISAttachFileBaseForm
{
	Q_OBJECT

signals:
	void RemoveBeginItem();

public:
	ISAttachFilesForm();
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
