#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISTaskBaseListForm.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISTaskFavoriteListForm : public ISTaskBaseListForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTaskFavoriteListForm(QWidget *parent = 0);
	virtual ~ISTaskFavoriteListForm();

	void Update() override;

protected:
	int GetCountFavorites();
};
//-----------------------------------------------------------------------------
