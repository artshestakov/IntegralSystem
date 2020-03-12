#include "ISEMailSelectEdit.h"
#include "ISMetaData.h"
//-----------------------------------------------------------------------------
ISEMailSelectEdit::ISEMailSelectEdit(QWidget *parent) : ISComboEdit(parent)
{
	SetEditable(false);

	PMetaTable *MetaTable = ISMetaData::GetInstanse().GetMetaTable("Organizations");
	for (int i = 0; i < MetaTable->Fields.size(); ++i)
	{
		PMetaField *MetaField = MetaTable->Fields[i];
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
