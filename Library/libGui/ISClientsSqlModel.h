#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISSqlModelCore.h"
//-----------------------------------------------------------------------------
class ISClientsSqlModel : public ISSqlModelCore
{
	Q_OBJECT

public:
	Q_INVOKABLE ISClientsSqlModel(PMetaClassTable *meta_table, QObject *parent = 0);
	virtual ~ISClientsSqlModel();

	QVariant data(const QModelIndex &ModelIndex, int Role = Qt::DisplayRole) const;
};
//-----------------------------------------------------------------------------
