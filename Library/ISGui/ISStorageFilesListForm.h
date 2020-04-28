#pragma once
#ifndef _ISSTORAGEFILESLISTFORM_H_INCLUDED
#define _ISSTORAGEFILESLISTFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISStorageFilesListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISStorageFilesListForm(QWidget *parent = 0);
	virtual ~ISStorageFilesListForm();

protected:
	void Create() override;
	void CreateCopy() override;
	void Edit() override;

private:
	void SaveFile(); //��������� ����
	void FilterChanged(QAbstractButton *AbstractButton);
	
	bool IsMyFile();
	bool CheckPassword();

private:
	QRadioButton *RadioAllFiles;
	QRadioButton *RadioMyFiles;
	QRadioButton *RadioMyPrivateFiles;
};
//-----------------------------------------------------------------------------
#endif