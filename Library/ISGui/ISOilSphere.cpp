#include "ISOilSphere.h"
//-----------------------------------------------------------------------------
#include "ISOilSphereCounterpartyObjectForm.h"
#include "ISOilSphereImplementationObjectForm.h"
#include "ISOilSphereImplementationDetailObjectForm.h"
//-----------------------------------------------------------------------------
ISOilSphere::ISOilSphere() : ISObjectInterface()
{

}
//-----------------------------------------------------------------------------
ISOilSphere::~ISOilSphere()
{

}
//-----------------------------------------------------------------------------
void ISOilSphere::RegisterMetaTypes() const
{
	
}
//-----------------------------------------------------------------------------
void ISOilSphere::BeforeShowMainWindow() const
{

}
//-----------------------------------------------------------------------------
void ISOilSphere::InitializePlugin() const
{
	qRegisterMetaType<ISOilSphereCounterpartyObjectForm*>("ISOilSphereCounterpartyObjectForm");
	qRegisterMetaType<ISOilSphereImplementationObjectForm*>("ISOilSphereImplementationObjectForm");
	qRegisterMetaType<ISOilSphereImplementationDetailObjectForm*>("ISOilSphereImplementationDetailObjectForm");
}
//-----------------------------------------------------------------------------
