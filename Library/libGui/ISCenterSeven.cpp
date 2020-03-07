#include "ISCenterSeven.h"
#include "ISCenterSevenSelectBranchForm.h"
#include "ISProperty.h"
//-----------------------------------------------------------------------------
#include "ISCenterSevenIncomingCallForm.h"
#include "ISCenterSevenDesktop.h"
#include "ISCenterSevenAdmissionObjectForm.h"
#include "ISCenterSevenPatientsObjectForm.h"
#include "ISCenterSevenPatientsListForm.h"
#include "ISCenterSevenWaitAdmissionListForm.h"
//-----------------------------------------------------------------------------
ISCenterSeven::ISCenterSeven() : ISObjectInterface()
{

}
//-----------------------------------------------------------------------------
ISCenterSeven::~ISCenterSeven()
{

}
//-----------------------------------------------------------------------------
void ISCenterSeven::RegisterMetaTypes() const
{
	qRegisterMetaType<ISCenterSevenIncomingCallForm*>("ISCenterSevenIncomingCallForm");
	qRegisterMetaType<ISCenterSevenDesktop*>("ISCenterSevenDesktop");
	qRegisterMetaType<ISCenterSevenAdmissionObjectForm*>("ISCenterSevenAdmissionObjectForm");
	qRegisterMetaType<ISCenterSevenPatientsObjectForm*>("ISCenterSevenPatientsObjectForm");
	qRegisterMetaType<ISCenterSevenPatientsListForm*>("ISCenterSevenPatientsListForm");
	qRegisterMetaType<ISCenterSevenWaitAdmissionListForm*>("ISCenterSevenWaitAdmissionListForm");
}
//-----------------------------------------------------------------------------
void ISCenterSeven::BeforeShowMainWindow() const
{
	ISCenterSevenSelectBranchForm CenterSevenSelectBranchForm;
	if (CenterSevenSelectBranchForm.Exec())
	{
		ISProperty::Instance().SetValue("BranchID", CenterSevenSelectBranchForm.GetSelectedBranchID());
	}
}
//-----------------------------------------------------------------------------
void ISCenterSeven::InitializePlugin() const
{
	
}
//-----------------------------------------------------------------------------
