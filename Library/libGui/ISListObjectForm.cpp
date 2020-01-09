#include "StdAfx.h"
#include "ISListObjectForm.h"
//-----------------------------------------------------------------------------
ISListObjectForm::ISListObjectForm(const QString &TableName, int ParentObjectID, QWidget *parent) : ISListBaseForm(TableName, parent)
{
	SetParentObjectID(ParentObjectID);
	GetQueryModel()->SetParentObjectIDClassFilter(ParentObjectID);
}
//-----------------------------------------------------------------------------
ISListObjectForm::~ISListObjectForm()
{

}
//-----------------------------------------------------------------------------
