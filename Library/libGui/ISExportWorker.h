#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISSqlModelCore.h"
//-----------------------------------------------------------------------------
class ISExportWorker : public QObject
{
	Q_OBJECT

signals:
	void ExportedRow();
	void Message(const QString &Message);

public:
	ISExportWorker(QObject *parent = 0);
	virtual ~ISExportWorker();

	virtual bool Prepare() = 0; //Подготовка экспорта
	virtual bool Export() = 0; //Экспорт

	void Cancel(); //Остановка

	void SetLocalName(const QString &local_name);
	void SetTableName(const QString &table_name);
	void SetModel(ISSqlModelCore *model);
	void SetFields(const QList<QString> &fields);
	void SetHeader(bool header);
	void SetSelectedRows(const QVectorInt &selected_rows);

	QString GetErrorString() const;

protected:
	QVariant PrepareValue(const QVariant &Value) const;

protected:
	QString ErrorString;
	QString LocalName;
	QString TableName;
	ISSqlModelCore *Model;
	QList<QString> Fields;
	bool Header;
	QVector<int> SelectedRows;
	bool Canceled;
};
//-----------------------------------------------------------------------------
