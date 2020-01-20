#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISStorageFilesListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISStorageFilesListForm(QWidget *parent = 0);
	virtual ~ISStorageFilesListForm();

protected:
	void AddMore();
	void Create() override;
	void CreateCopy() override;

	void OpenFile(); //������� ����
	void SaveFile(); //��������� ����

	void FilterChanged();

private:
	QRadioButton *RadioAll;
	QRadioButton *RadioPrivate;
};
//-----------------------------------------------------------------------------
