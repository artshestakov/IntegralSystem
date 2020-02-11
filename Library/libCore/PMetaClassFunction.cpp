#include "PMetaClassFunction.h"
//-----------------------------------------------------------------------------
PMetaClassFunction::PMetaClassFunction() : PMetaClass("Function")
{

}
//-----------------------------------------------------------------------------
PMetaClassFunction::~PMetaClassFunction()
{

}
//-----------------------------------------------------------------------------
void PMetaClassFunction::SetText(const QString &text)
{
	Text = text;
}
//-----------------------------------------------------------------------------
QString PMetaClassFunction::GetText() const
{
	return Text;
}
//-----------------------------------------------------------------------------
