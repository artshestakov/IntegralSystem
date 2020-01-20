#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class ISCenterSevenStatusAdmissionForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISCenterSevenStatusAdmissionForm(const ISUuid &CurrentStatusUID, const QString &Comment, QWidget *parent = 0);
	virtual ~ISCenterSevenStatusAdmissionForm();

	ISUuid GetStatusUID() const;
	QString GetComment() const;

protected:
	void EnterClicked() override;
	void StatusChange();

private:
	QButtonGroup *ButtonGroup;
	ISLineEdit *EditComment;
};
//-----------------------------------------------------------------------------
