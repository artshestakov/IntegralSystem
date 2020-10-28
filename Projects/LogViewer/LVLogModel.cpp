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
	Q_UNUSED(Content);
	return true;
}
//-----------------------------------------------------------------------------
QVariant LVLogModel::data(const QModelIndex &ModelIndex, int Role) const
{
	Q_UNUSED(ModelIndex);
	Q_UNUSED(Role);
	return QVariant();
}
//-----------------------------------------------------------------------------
bool LVLogModel::setData(const QModelIndex &ModelIndex, const QVariant &Value, int Role)
{
	Q_UNUSED(ModelIndex);
	Q_UNUSED(Value);
	Q_UNUSED(Role);
	return true;
}
//-----------------------------------------------------------------------------
QVariant LVLogModel::headerData(int Section, Qt::Orientation Orientation, int Role) const
{
	Q_UNUSED(Section);
	Q_UNUSED(Orientation);
	Q_UNUSED(Role);
	return QVariant();
}
//-----------------------------------------------------------------------------
int LVLogModel::rowCount(const QModelIndex &Parent) const
{
	Q_UNUSED(Parent);
	return 0;
}
//-----------------------------------------------------------------------------
int LVLogModel::columnCount(const QModelIndex &Parent) const
{
	Q_UNUSED(Parent);
	return 0;
}
//-----------------------------------------------------------------------------
QModelIndex LVLogModel::index(int Row, int Column, const QModelIndex &Parent) const
{
	Q_UNUSED(Row);
	Q_UNUSED(Column);
	Q_UNUSED(Parent);
	return createIndex(Row, Column);
}
//-----------------------------------------------------------------------------
QModelIndex LVLogModel::parent(const QModelIndex &Index) const
{
	Q_UNUSED(Index);
	return QModelIndex();
}
//-----------------------------------------------------------------------------
