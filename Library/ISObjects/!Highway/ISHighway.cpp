#include "StdAfx.h"
#include "ISHighway.h"
//-----------------------------------------------------------------------------
#include "ISHighwayIncomingCallForm.h"
#include "ISHighwayOrganizationMyListForm.h"
#include "ISHighwayOrganizationListForm.h"
#include "ISHighwayOrganizationObjectForm.h"
#include "ISHighwayBillsListForm.h"
//-----------------------------------------------------------------------------
ISHighway::ISHighway(QObject *parent) : ISObjectInterface(parent)
{

}
//-----------------------------------------------------------------------------
ISHighway::~ISHighway()
{

}
//-----------------------------------------------------------------------------
void ISHighway::RegisterMetaTypes() const
{
	qRegisterMetaType<ISHighwayIncomingCallForm*>("ISHighwayIncomingCallForm");
	qRegisterMetaType<ISHighwayOrganizationMyListForm*>("ISHighwayOrganizationMyListForm");
	qRegisterMetaType<ISHighwayOrganizationListForm*>("ISHighwayOrganizationListForm");
	qRegisterMetaType<ISHighwayOrganizationObjectForm*>("ISHighwayOrganizationObjectForm");
	qRegisterMetaType<ISHighwayBillsListForm*>("ISHighwayBillsListForm");
}
//-----------------------------------------------------------------------------
void ISHighway::BeforeShowMainWindow() const
{

}
//-----------------------------------------------------------------------------
void ISHighway::InitializePlugin() const
{

}
//-----------------------------------------------------------------------------
