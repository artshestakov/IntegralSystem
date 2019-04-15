#include "StdAfx.h"
#include "ISObjectModel.h"
//-----------------------------------------------------------------------------
ISObjectModel::ISObjectModel(QObject *parent) : QObject(parent)
{

}
//-----------------------------------------------------------------------------
ISObjectModel::~ISObjectModel()
{

}
//-----------------------------------------------------------------------------
void ISObjectModel::Append(const QString &FieldName)
{
	ISObjectModelItem *ModelItem = new ISObjectModelItem(this);
	Items.insert(FieldName, ModelItem);
}
//-----------------------------------------------------------------------------
QVariant ISObjectModel::GetOldValue(const QString &FieldName) const
{
	return Items.value(FieldName)->GetOldValue();
}
//-----------------------------------------------------------------------------
void ISObjectModel::SetOldValue(const QString &FieldName, const QVariant &Value)
{
	Items.value(FieldName)->SetOldValue(Value);
}
//-----------------------------------------------------------------------------
QVariant ISObjectModel::GetNewValue(const QString &FieldName) const
{
	return Items.value(FieldName)->GetNewValue();
}
//-----------------------------------------------------------------------------
void ISObjectModel::SetNewValue(const QString &FieldName, const QVariant &Value)
{
	Items.value(FieldName)->SetNewValue(Value);
}
//-----------------------------------------------------------------------------
