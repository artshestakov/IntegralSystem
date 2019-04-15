#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISInterfaceForm.h"
#include "ISLineEdit.h"
#include "ISPhoneBaseEdit.h"
#include "ISEMailEdit.h"
#include "ISTextEdit.h"
#include "ISPathEditFile.h"
#include "ISButtonDialog.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISFeedbackForm : public ISInterfaceForm
{
	Q_OBJECT

public:
	ISFeedbackForm(QWidget *parent = 0);
	virtual ~ISFeedbackForm();

protected:
	void EscapeClicked() override;
	void CreateTypePanel();
	bool CheckReview();
	void SendReview();
	QString CreateMessage() const;

private:
	QButtonGroup *ButtonGroup;
	ISLineEdit *EditName;
	ISPhoneBaseEdit *EditPhone;
	ISEMailEdit *EditMail;
	ISLineEdit *EditTitle;
	ISTextEdit *EditDescription;
	ISPathEditFile *EditPathFile;
	ISButtonDialog *ButtonDialog;
};
//-----------------------------------------------------------------------------
