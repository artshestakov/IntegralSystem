#pragma once
#ifndef _ISSTORAGEFILESLISTFORM_H_INCLUDED
#define _ISSTORAGEFILESLISTFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISStorageFilesSubSystem : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISStorageFilesSubSystem(QWidget *parent = 0);
	virtual ~ISStorageFilesSubSystem();

protected:
	void Create() override;
	void CreateCopy() override;

private:
	void SaveFile(); //Сохранить файл
};
//-----------------------------------------------------------------------------
#endif
