#include "ISObjectModel.h"
//-----------------------------------------------------------------------------
ISObjectModel::ISObjectModel()
{

}
//-----------------------------------------------------------------------------
ISObjectModel::~ISObjectModel()
{
	while (!Items.isEmpty())
	{
		delete Items.take(Items.keys().back());
	}
}
//-----------------------------------------------------------------------------
void ISObjectModel::Append(const QString &FieldName)
{
	ISObjectModelItem *ModelItem = new ISObjectModelItem();
	Items.insert(FieldName, ModelItem);
}
//-----------------------------------------------------------------------------
QVariant ISObjectModel::GetOldValue(const QString &FieldName) const
{
	return Items.value(FieldName)->OldValue;
}
//-----------------------------------------------------------------------------
void ISObjectModel::SetOldValue(const QString &FieldName, const QVariant &Value)
{
	Items.value(FieldName)->OldValue = Value;
}
//-----------------------------------------------------------------------------
QVariant ISObjectModel::GetNewValue(const QString &FieldName) const
{
	return Items.value(FieldName)->NewValue;
}
//-----------------------------------------------------------------------------
void ISObjectModel::SetNewValue(const QString &FieldName, const QVariant &Value)
{
	Items.value(FieldName)->NewValue = Value;
}
//-----------------------------------------------------------------------------
