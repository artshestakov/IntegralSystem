#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISMedTechRatingListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISMedTechRatingListForm(QWidget *parent = 0);
	virtual ~ISMedTechRatingListForm();

protected:
	void FilterClicked();
};
//-----------------------------------------------------------------------------
