#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISInterfaceMetaForm.h"
#include "ISMedTechRatingListForm.h"
#include "ISAsteriskCallsListForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISMedTechRatingSubSystem : public ISInterfaceMetaForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISMedTechRatingSubSystem(QWidget *parent = 0);
	virtual ~ISMedTechRatingSubSystem();

	void LoadData() override;

protected:
	void SelectedRating();
	void ClearCDR();

private:
	ISMedTechRatingListForm *RatingListForm;
	ISAsteriskCallsListForm *AsteriskCallsListForm;
	bool CDRLoadData;
};
//-----------------------------------------------------------------------------
