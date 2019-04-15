#include "StdAfx.h"
#include "ISInformResource.h"
//-----------------------------------------------------------------------------
#include "ISDesktopInformResource.h"
#include "ISOrganizationListForm.h"
#include "ISOrganizationObjectForm.h"
#include "ISOrganizationWorkListForm.h"
#include "ISOrganizationMyListForm.h"
#include "ISCallHistoryObjectForm.h"
#include "ISCallUserStatisticsForm.h"
#include "ISDispatchListForm.h"
#include "ISDispatchOrganizationListForm.h"
#include "ISEMailSelectEdit.h"
#include "ISPriceListObjectForm.h"
#include "ISIncomingCallInformResourceForm.h"
#include "ISDemoAccessesObjectForm.h"
#include "ISDemoAccessesListForm.h"
#include "ISDemoAccessesMyListForm.h"
#include "ISDemoAccessesMyCreatedListForm.h"
#include "ISScoreObjectForm.h"
#include "ISOrganizationScoreObjectForm.h"
//-----------------------------------------------------------------------------
ISInformResource::ISInformResource(QObject *parent) : ISObjectInterface(parent)
{

}
//-----------------------------------------------------------------------------
ISInformResource::~ISInformResource()
{

}
//-----------------------------------------------------------------------------
void ISInformResource::RegisterMetaTypes() const
{
    qRegisterMetaType<ISDesktopInformResource*>("ISInformResourceDesktop");
	qRegisterMetaType<ISOrganizationObjectForm*>("ISOrganizationObjectForm");
	qRegisterMetaType<ISOrganizationListForm*>("ISOrganizationListForm");
	qRegisterMetaType<ISOrganizationWorkListForm*>("ISOrganizationWorkListForm");
	qRegisterMetaType<ISOrganizationMyListForm*>("ISOrganizationMyListForm");
	qRegisterMetaType<ISCallHistoryObjectForm*>("ISCallHistoryObjectForm");
	qRegisterMetaType<ISCallUserStatisticsForm*>("ISCallUserStatisticsForm");
	qRegisterMetaType<ISDispatchListForm*>("ISDispatchListForm");
	qRegisterMetaType<ISDispatchOrganizationListForm*>("ISDispatchOrganizationListForm");
	qRegisterMetaType<ISEMailSelectEdit*>("ISEMailSelectEdit");

	qRegisterMetaType<ISPriceListObjectForm*>("ISPriceListObjectForm");
	qRegisterMetaType<ISIncomingCallInformResourceForm*>("ISIncomingCallInformResourceForm");
	qRegisterMetaType<ISDemoAccessesObjectForm*>("ISDemoAccessesObjectForm");
	qRegisterMetaType<ISDemoAccessesListForm*>("ISDemoAccessesListForm");
	qRegisterMetaType<ISDemoAccessesMyListForm*>("ISDemoAccessesMyListForm");
	qRegisterMetaType<ISDemoAccessesMyCreatedListForm*>("ISDemoAccessesMyCreatedListForm");
	qRegisterMetaType<ISScoreObjectForm*>("ISScoreObjectForm");
    qRegisterMetaType<ISOrganizationScoreObjectForm*>("ISOrganizationScoreObjectForm");
}
//-----------------------------------------------------------------------------
void ISInformResource::BeforeShowMainWindow() const
{

}
//-----------------------------------------------------------------------------
void ISInformResource::InitializePlugin() const
{

}
//-----------------------------------------------------------------------------
