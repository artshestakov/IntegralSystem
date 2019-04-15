#pragma once
//-----------------------------------------------------------------------------
#include "isobjects_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISOBJECTS_EXPORT ISMedTechRatingListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISMedTechRatingListForm(QWidget *parent = 0);
	virtual ~ISMedTechRatingListForm();

protected:
	void FilterClicked();
};
//-----------------------------------------------------------------------------
