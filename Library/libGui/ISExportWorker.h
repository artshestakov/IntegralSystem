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
	void SetFields(const ISVectorString &fields);
	void SetHeader(bool header);
	void SetSelectedRows(const ISVectorInt &selected_rows);

	QString GetErrorString() const;

protected:
	QVariant PrepareValue(const QVariant &Value) const;

protected:
	QString ErrorString;
	QString LocalName;
	QString TableName;
	ISSqlModelCore *Model;
	ISVectorString Fields;
	bool Header;
	ISVectorInt SelectedRows;
	bool Canceled;
};
//-----------------------------------------------------------------------------
