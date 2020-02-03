#include "ISColumnSizeItem.h"
//-----------------------------------------------------------------------------
ISColumnSizeItem::ISColumnSizeItem()
	: ModificationFlag(false)
{
	
}
//-----------------------------------------------------------------------------
ISColumnSizeItem::~ISColumnSizeItem()
{

}
//-----------------------------------------------------------------------------
void ISColumnSizeItem::SetFieldSize(const QString &FieldName, int Size)
{
	Fields.insert(FieldName, Size);
}
//-----------------------------------------------------------------------------
int ISColumnSizeItem::GetFieldSize(const QString &FieldName)
{
	return Fields.value(FieldName);
}
//-----------------------------------------------------------------------------
void ISColumnSizeItem::SetModificationFlag(bool modification_flag)
{
	ModificationFlag = modification_flag;
}
//-----------------------------------------------------------------------------
bool ISColumnSizeItem::GetModificationFlag() const
{
	return ModificationFlag;
}
//-----------------------------------------------------------------------------
QMap<QString, int> ISColumnSizeItem::GetFields()
{
	return Fields;
}
//-----------------------------------------------------------------------------
