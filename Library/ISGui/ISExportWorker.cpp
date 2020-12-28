#include "ISExportWorker.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISExportWorker::ISExportWorker(PMetaTable *meta_table, ISTcpModel *tcp_model, QObject *parent)
	: QObject(parent),
	ErrorString(LANG("Export.Error.NoError")),
	MetaTable(meta_table),
	TcpModel(tcp_model),
	Header(false),
	Canceled(false)
{
	
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
void ISExportWorker::SetFields(const ISVectorUInt &fields)
{
	Fields = fields;
}
//-----------------------------------------------------------------------------
void ISExportWorker::SetHeader(bool header)
{
	Header = header;
}
//-----------------------------------------------------------------------------
void ISExportWorker::SetSelectedRows(const ISVectorInt &selected_rows)
{
	SelectedRows = selected_rows;
}
//-----------------------------------------------------------------------------
QString ISExportWorker::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
QVariant ISExportWorker::PrepareValue(ISNamespace::FieldType Type, const QVariant &Value) const
{
	QVariant Result;
	if (!Value.isNull())
	{
		if (Type == ISNamespace::FT_Bool)
		{
			Result = Value.toBool() ? LANG("Yes") : LANG("No");
		}
		else
		{
			Result = Value;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
