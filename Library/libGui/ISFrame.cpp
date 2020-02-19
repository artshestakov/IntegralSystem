#include "ISFrame.h"
//-----------------------------------------------------------------------------
#include "ISFrameClientsListForm.h"
#include "ISCurrentAffairsForm.h"
//-----------------------------------------------------------------------------
ISFrame::ISFrame() : ISObjectInterface()
{

}
//-----------------------------------------------------------------------------
ISFrame::~ISFrame()
{

}
//-----------------------------------------------------------------------------
void ISFrame::RegisterMetaTypes() const
{
	qRegisterMetaType<ISFrameClientsListForm*>("ISFrameClientsListForm");
	qRegisterMetaType<ISCurrentAffairsForm*>("ISCurrentAffairsForm");
}
//-----------------------------------------------------------------------------
void ISFrame::BeforeShowMainWindow() const
{

}
//-----------------------------------------------------------------------------
void ISFrame::InitializePlugin() const
{

}
//-----------------------------------------------------------------------------
