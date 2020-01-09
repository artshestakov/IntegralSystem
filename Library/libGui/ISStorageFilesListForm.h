#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISStorageFilesListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISStorageFilesListForm(QWidget *parent = 0);
	virtual ~ISStorageFilesListForm();

protected:
	void AddMore();
	void Create() override;
	void CreateCopy() override;

	void OpenFile(); //Открыть файл
	void SaveFile(); //Сохранить файл

	void FilterChanged();

private:
	QRadioButton *RadioAll;
	QRadioButton *RadioPrivate;
};
//-----------------------------------------------------------------------------
