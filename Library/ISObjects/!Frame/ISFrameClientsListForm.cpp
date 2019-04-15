#include "StdAfx.h"
#include "ISFrameClientsListForm.h"
//-----------------------------------------------------------------------------
ISFrameClientsListForm::ISFrameClientsListForm(QWidget *parent) : ISListBaseForm("Clients", parent)
{
	GetSqlModel()->RemoveColumn(GetMetaTable()->GetField("Call"));
	GetSqlModel()->RemoveColumn(GetMetaTable()->GetField("Meeting"));
	GetSqlModel()->RemoveColumn(GetMetaTable()->GetField("CommercialSentence"));
	GetSqlModel()->RemoveColumn(GetMetaTable()->GetField("Treaty"));
	GetSqlModel()->RemoveColumn(GetMetaTable()->GetField("InWork"));
}
//-----------------------------------------------------------------------------
ISFrameClientsListForm::~ISFrameClientsListForm()
{

}
//-----------------------------------------------------------------------------
