#include "StdAfx.h"
#include "ISEMailSelectEdit.h"
#include "ISMetaData.h"
//-----------------------------------------------------------------------------
ISEMailSelectEdit::ISEMailSelectEdit(QWidget *parent) : ISComboEdit(parent)
{
	SetEditable(false);

	PMetaClassTable *MetaTable = ISMetaData::GetInstanse().GetMetaTable("Organizations");
	for (int i = 0; i < MetaTable->GetFields().count(); ++i)
	{
		PMetaClassField *MetaField = MetaTable->GetFields().at(i);
		if (MetaField->GetType() == ISNamespace::FT_EMail)
		{
			AddItem(MetaField->GetLabelName(), MetaField->GetName());
		}
	}
}
//-----------------------------------------------------------------------------
ISEMailSelectEdit::~ISEMailSelectEdit()
{

}
//-----------------------------------------------------------------------------
