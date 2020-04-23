#include "ISSirona.h"
//-----------------------------------------------------------------------------
#include "ISSironaAppointmentListForm.h"
#include "ISSironaResearchTypeObjectForm.h"
#include "ISSironaReportForm.h"
#include "ISSironaResearchListForm.h"
#include "ISSironaResearchObjectForm.h"
#include "ISSironaCostListForm.h"
#include "ISSironaCostObjectForm.h"
//-----------------------------------------------------------------------------
ISSirona::ISSirona() : ISObjectInterface()
{

}
//-----------------------------------------------------------------------------
ISSirona::~ISSirona()
{

}
//-----------------------------------------------------------------------------
void ISSirona::RegisterMetaTypes() const
{
	qRegisterMetaType<ISSironaAppointmentListForm*>("ISSironaAppointmentListForm");
	qRegisterMetaType<ISSironaResearchListForm*>("ISSironaResearchListForm");
	qRegisterMetaType<ISSironaResearchObjectForm*>("ISSironaResearchObjectForm");
	qRegisterMetaType<ISSironaResearchTypeObjectForm*>("ISSironaResearchTypeObjectForm");
	qRegisterMetaType<ISSironaReportForm*>("ISSironaReportForm");
	qRegisterMetaType<ISSironaCostListForm*>("ISSironaCostListForm");
	qRegisterMetaType<ISSironaCostObjectForm*>("ISSironaCostObjectForm");
}
//-----------------------------------------------------------------------------
void ISSirona::BeforeShowMainWindow() const
{

}
//-----------------------------------------------------------------------------
void ISSirona::InitializePlugin() const
{

}
//-----------------------------------------------------------------------------
