#include "StdAfx.h"
#include "ISClientsSqlModel.h"
#include "ISDefines.h"
//-----------------------------------------------------------------------------
ISClientsSqlModel::ISClientsSqlModel(PMetaClassTable *meta_table, QObject *parent) : ISSqlModelCore(meta_table, parent)
{

}
//-----------------------------------------------------------------------------
ISClientsSqlModel::~ISClientsSqlModel()
{

}
//-----------------------------------------------------------------------------
QVariant ISClientsSqlModel::data(const QModelIndex &ModelIndex, int Role) const
{
	QVariant Value = ISSqlModelCore::data(ModelIndex, Role);

	if (Role == Qt::TextColorRole)
	{
		int CountInWork = GetRecord(ModelIndex.row()).value("CountInWork").toInt();
		if (CountInWork)
		{
			Value = qVariantFromValue(COLOR_RED);
		}
		else
		{
			Value = qVariantFromValue(COLOR_DARK_GREEN);
		}
	}

	return Value;
}
//-----------------------------------------------------------------------------
