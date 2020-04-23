#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTaskBaseListForm.h"
//-----------------------------------------------------------------------------
class ISTaskFavoriteListForm : public ISTaskBaseListForm
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
