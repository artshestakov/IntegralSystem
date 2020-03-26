#include "ISOilSphere.h"
//-----------------------------------------------------------------------------
#include "ISOilSphereCounterpartyObjectForm.h"
#include "ISOilSphereImplementationObjectForm.h"
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
}
//-----------------------------------------------------------------------------
