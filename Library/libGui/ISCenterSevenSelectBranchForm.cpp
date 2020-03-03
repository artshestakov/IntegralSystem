#include "ISCenterSevenSelectBranchForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISQuery.h"
#include "ISConstants.h"
#include "ISQueryText.h"
//-----------------------------------------------------------------------------
static QString QS_BRANCH = PREPARE_QUERY2("SELECT brch_id, brch_name, brch_address "
										 "FROM branches "
										 "WHERE NOT brch_isdeleted "
										 "ORDER BY brch_name");
//-----------------------------------------------------------------------------
ISCenterSevenSelectBranchForm::ISCenterSevenSelectBranchForm(QWidget *parent) : ISInterfaceDialogForm(parent)
{
	SelectedBranchID = 0;

	setWindowTitle(LANG("CenterSeven.SelectBranch"));
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);
	ForbidResize();

	QLabel *Label = new QLabel(this);
	Label->setText(LANG("CenterSeven.SelectYouCurrentBranch") + ':');
	GetMainLayout()->addWidget(Label);

	ISQuery qSelect(QS_BRANCH);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			int BranchID = qSelect.ReadColumn("brch_id").toInt();
			QString BranchName = qSelect.ReadColumn("brch_name").toString();
			QString BranchAddress = qSelect.ReadColumn("brch_address").toString();

			QCommandLinkButton *CommandLinkButton = new QCommandLinkButton(this);
			CommandLinkButton->setText(BranchName);
			CommandLinkButton->setDescription(BranchAddress);
			CommandLinkButton->setProperty("BranchID", BranchID);
			CommandLinkButton->setCursor(CURSOR_POINTING_HAND);
			connect(CommandLinkButton, &QCommandLinkButton::clicked, this, &ISCenterSevenSelectBranchForm::Select);
			GetMainLayout()->addWidget(CommandLinkButton);
		}
	}
}
//-----------------------------------------------------------------------------
ISCenterSevenSelectBranchForm::~ISCenterSevenSelectBranchForm()
{

}
//-----------------------------------------------------------------------------
int ISCenterSevenSelectBranchForm::GetSelectedBranchID() const
{
	return SelectedBranchID;
}
//-----------------------------------------------------------------------------
void ISCenterSevenSelectBranchForm::Select()
{
	SelectedBranchID = sender()->property("BranchID").toInt();
	SetResult(true);
	close();
}
//-----------------------------------------------------------------------------
