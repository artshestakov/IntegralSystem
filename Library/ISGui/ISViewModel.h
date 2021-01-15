#pragma once
#ifndef _ISVIEWMODEL_H_INCLUDED
#define _ISVIEWMODEL_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISViewModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	ISViewModel(QObject *parent);
	virtual ~ISViewModel();

	void Clear();
	void SetSource(const QStringList &fields, const QVariantList &records);

	QVariant data(const QModelIndex &ModelIndex, int Role = Qt::DisplayRole) const override;
	QVariant headerData(int Section, Qt::Orientation Orientation, int Role = Qt::DisplayRole) const override;
	int rowCount(const QModelIndex &Parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &Parent = QModelIndex()) const override;
	QModelIndex index(int Row, int Column, const QModelIndex &Parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &Index) const override;

private:
	std::vector<QString> Fields;
	std::vector<QStringList> Records;
};
//-----------------------------------------------------------------------------
#endif
