#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISObjectFormBase.h"
//-----------------------------------------------------------------------------
class ISScoreObjectForm : public ISObjectFormBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISScoreObjectForm(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id = 0);
	virtual ~ISScoreObjectForm();

protected:
	void CompetitorChanged();

private:
	ISFieldEditBase *EditCompetitor;
	ISFieldEditBase *EditAuditors;
	ISFieldEditBase *EditJournals;
	ISFieldEditBase *EditSeminars;
	ISFieldEditBase *EditElectronicReporting;
};
//-----------------------------------------------------------------------------
