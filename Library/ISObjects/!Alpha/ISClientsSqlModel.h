#pragma once
//-----------------------------------------------------------------------------
#include "isobjects_global.h"
#include "ISSqlModelCore.h"
//-----------------------------------------------------------------------------
class ISOBJECTS_EXPORT ISClientsSqlModel : public ISSqlModelCore
{
	Q_OBJECT

public:
	Q_INVOKABLE ISClientsSqlModel(PMetaClassTable *meta_table, QObject *parent = 0);
	virtual ~ISClientsSqlModel();

	QVariant data(const QModelIndex &ModelIndex, int Role = Qt::DisplayRole) const;
};
//-----------------------------------------------------------------------------
