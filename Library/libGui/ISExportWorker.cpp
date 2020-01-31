#include "StdAfx.h"
#include "ISExportWorker.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISExportWorker::ISExportWorker(QObject *parent) : QObject(parent)
{
	ErrorString = LANG("Export.Error.NoError");
	Model = nullptr;
	Header = false;
	Canceled = false;
}
//-----------------------------------------------------------------------------
ISExportWorker::~ISExportWorker()
{

}
//-----------------------------------------------------------------------------
void ISExportWorker::Cancel()
{
	Canceled = true;
}
//-----------------------------------------------------------------------------
void ISExportWorker::SetLocalName(const QString &local_name)
{
	LocalName = local_name;
}
//-----------------------------------------------------------------------------
void ISExportWorker::SetTableName(const QString &table_name)
{
	TableName = table_name;
}
//-----------------------------------------------------------------------------
void ISExportWorker::SetModel(ISSqlModelCore *model)
{
	Model = model;
}
//-----------------------------------------------------------------------------
void ISExportWorker::SetFields(const QList<QString> &fields)
{
	Fields = fields;
}
//-----------------------------------------------------------------------------
void ISExportWorker::SetHeader(bool header)
{
	Header = header;
}
//-----------------------------------------------------------------------------
void ISExportWorker::SetSelectedRows(const QVectorInt &selected_rows)
{
	SelectedRows = selected_rows;
}
//-----------------------------------------------------------------------------
QString ISExportWorker::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
QVariant ISExportWorker::PrepareValue(const QVariant &Value) const
{
	QVariant Result;
	if (Value.type() == QVariant::Bool)
	{
		if (Value.toBool())
		{
			Result = LANG("Yes");
		}
		else
		{
			Result = LANG("No");
		}
	}
	else
	{
		Result = Value;
	}

	return Result;
}
//-----------------------------------------------------------------------------
