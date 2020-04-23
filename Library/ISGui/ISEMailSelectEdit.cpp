#include "ISEMailSelectEdit.h"
#include "ISMetaData.h"
//-----------------------------------------------------------------------------
ISEMailSelectEdit::ISEMailSelectEdit(QWidget *parent) : ISComboEdit(parent)
{
	SetEditable(false);

	PMetaTable *MetaTable = ISMetaData::GetInstanse().GetMetaTable("Organizations");
	for (PMetaField *MetaField : MetaTable->Fields)
	{
		if (MetaField->Type == ISNamespace::FT_EMail)
		{
			AddItem(MetaField->LabelName, MetaField->Name);
		}
	}
}
//-----------------------------------------------------------------------------
ISEMailSelectEdit::~ISEMailSelectEdit()
{

}
//-----------------------------------------------------------------------------
