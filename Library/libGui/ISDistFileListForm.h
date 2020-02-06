#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListBaseForm.h"

//-----------------------------------------------------------------------------
class ISDistFileListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	ISDistFileListForm(QWidget *parent = 0);
	virtual ~ISDistFileListForm();

protected:
	void dragEnterEvent(QDragEnterEvent *e);
	void dropEvent(QDropEvent *e);

	void Create() override;
	bool DeleteCascade() override;

	void InsertFile(const QString &FilePath);
	bool InsertData(int FileID, const QString &Data);
	
	void Actual();
	void Save();
};
//-----------------------------------------------------------------------------
