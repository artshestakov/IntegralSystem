#pragma once
#ifndef _LVFILEMODEL_H_INCLUDED
#define _LVFILEMODEL_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class LVLogModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	LVLogModel(QObject *parent);
	virtual ~LVLogModel();

	bool SetContent(const QString &Content);

	virtual QVariant data(const QModelIndex &ModelIndex, int Role = Qt::DisplayRole) const override;
	virtual bool setData(const QModelIndex &ModelIndex, const QVariant &Value, int Role = Qt::EditRole) override;
	virtual QVariant headerData(int Section, Qt::Orientation Orientation, int Role = Qt::DisplayRole) const override;
	virtual int rowCount(const QModelIndex &Parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex &Parent = QModelIndex()) const override;
	virtual QModelIndex index(int Row, int Column, const QModelIndex &Parent = QModelIndex()) const override;
	virtual QModelIndex parent(const QModelIndex &Index) const override;
};
//-----------------------------------------------------------------------------
#endif
