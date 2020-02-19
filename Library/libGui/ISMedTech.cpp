#include "ISMedTech.h"
//-----------------------------------------------------------------------------
#include "ISMedTechRatingSubSystem.h"
#include "ISMedTechRatingListForm.h"
//-----------------------------------------------------------------------------
ISMedTech::ISMedTech() : ISObjectInterface()
{

}
//-----------------------------------------------------------------------------
ISMedTech::~ISMedTech()
{

}
//-----------------------------------------------------------------------------
void ISMedTech::RegisterMetaTypes() const
{
	qRegisterMetaType<ISMedTechRatingSubSystem*>("ISMedTechRatingSubSystem");
	qRegisterMetaType<ISMedTechRatingListForm*>("ISMedTechRatingListForm");
}
//-----------------------------------------------------------------------------
void ISMedTech::BeforeShowMainWindow() const
{

}
//-----------------------------------------------------------------------------
void ISMedTech::InitializePlugin() const
{

}
//-----------------------------------------------------------------------------
