#include "ISClientsSqlModel.h"
#include "ISDefinesGui.h"
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
			Value = qVariantFromValue(ISDefines::Gui::COLOR_RED);
		}
		else
		{
			Value = qVariantFromValue(ISDefines::Gui::COLOR_DARK_GREEN);
		}
	}

	return Value;
}
//-----------------------------------------------------------------------------
