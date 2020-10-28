#include "LVLogModel.h"
//-----------------------------------------------------------------------------
LVLogModel::LVLogModel(QObject *parent) : QAbstractItemModel(parent)
{

}
//-----------------------------------------------------------------------------
LVLogModel::~LVLogModel()
{

}
//-----------------------------------------------------------------------------
bool LVLogModel::SetContent(const QString &Content)
{
	return true;
}
//-----------------------------------------------------------------------------
QVariant LVLogModel::data(const QModelIndex &ModelIndex, int Role) const
{
	return QVariant();
}
//-----------------------------------------------------------------------------
bool LVLogModel::setData(const QModelIndex &ModelIndex, const QVariant &Value, int Role)
{
	return true;
}
//-----------------------------------------------------------------------------
QVariant LVLogModel::headerData(int Section, Qt::Orientation Orientation, int Role) const
{
	return QVariant();
}
//-----------------------------------------------------------------------------
int LVLogModel::rowCount(const QModelIndex &Parent) const
{
	return 0;
}
//-----------------------------------------------------------------------------
int LVLogModel::columnCount(const QModelIndex &Parent) const
{
	return 0;
}
//-----------------------------------------------------------------------------
QModelIndex LVLogModel::index(int Row, int Column, const QModelIndex &Parent) const
{
	return createIndex(Row, Column);
}
//-----------------------------------------------------------------------------
QModelIndex LVLogModel::parent(const QModelIndex &Index) const
{
	return QModelIndex();
}
//-----------------------------------------------------------------------------
