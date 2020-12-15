#pragma once
#ifndef _ISTCPMODEL_H_INCLUDED
#define _ISTCPMODEL_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISTcpModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	ISTcpModel(QObject *parent);
	virtual ~ISTcpModel();

	void SetData(const QVariantList &fields, const QVariantList &records);

	QVariant data(const QModelIndex &ModelIndex, int Role = Qt::DisplayRole) const override;
	bool setData(const QModelIndex &ModelIndex, const QVariant &Value, int Role = Qt::EditRole) override;
	QVariant headerData(int Section, Qt::Orientation Orientation, int Role = Qt::DisplayRole) const override;
	int rowCount(const QModelIndex &Parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &Parent = QModelIndex()) const override;
	QModelIndex index(int Row, int Column, const QModelIndex &Parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &Index) const override;

private:
	std::vector<ISFieldModel> Fields;
	std::vector<ISVectorVariant> Records;
};
//-----------------------------------------------------------------------------
#endif
