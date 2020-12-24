#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class ISExportWorker : public QObject
{
	Q_OBJECT

signals:
	void ExportedRow();
	void Message(const QString &Message);

public:
	ISExportWorker(PMetaTable *meta_table, QObject *parent = 0);
	virtual ~ISExportWorker();

	virtual bool Prepare() = 0; //Подготовка экспорта
	virtual bool Export() = 0; //Экспорт

	void Cancel(); //Остановка

	void SetModel(QObject *SqlModel);
	void SetFields(const ISVectorString &fields);
	void SetHeader(bool header);
	void SetSelectedRows(const ISVectorInt &selected_rows);

	QString GetErrorString() const;

protected:
	QVariant PrepareValue(ISNamespace::FieldType Type, const QVariant &Value) const;

protected:
	QString ErrorString;
	PMetaTable *MetaTable;
	QObject *Model;
	ISVectorString Fields;
	bool Header;
	ISVectorInt SelectedRows;
	bool Canceled;
};
//-----------------------------------------------------------------------------
