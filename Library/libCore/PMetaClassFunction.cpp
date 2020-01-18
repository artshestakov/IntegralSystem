#include "PMetaClassFunction.h"
//-----------------------------------------------------------------------------
PMetaClassFunction::PMetaClassFunction(QObject *parent) : PMetaClass(parent)
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
