#include "ISHighway.h"
//-----------------------------------------------------------------------------
#include "ISHighwayIncomingCallForm.h"
#include "ISHighwayOrganizationMyListForm.h"
#include "ISHighwayOrganizationListForm.h"
#include "ISHighwayOrganizationObjectForm.h"
//-----------------------------------------------------------------------------
ISHighway::ISHighway() : ISObjectInterface()
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
