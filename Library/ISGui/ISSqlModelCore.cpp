#include "ISSqlModelCore.h"
#include "ISDefinesGui.h"
#include "ISBuffer.h"
#include "ISTrace.h"
#include "ISQueryModel.h"
#include "ISQuery.h"
#include "ISAssert.h"
#include "ISSqlModelHelper.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISSqlModelCore::ISSqlModelCore(PMetaTable *meta_table, QObject *parent)
	: QAbstractItemModel(parent),
	MetaTable(meta_table),
	SortingColumn(0),
	SortingOrder(Qt::AscendingOrder),
	IsSystemIndex(-1),
	IsDeletedIndex(-1),
	ShowToolTip(false),
	IconSortingUp(BUFFER_ICONS("Table.Sorting.Up")),
	IconSortingDown(BUFFER_ICONS("Table.Sorting.Down"))
{
	
}
//-----------------------------------------------------------------------------
ISSqlModelCore::~ISSqlModelCore()
{
	Clear();
}
//-----------------------------------------------------------------------------
void ISSqlModelCore::FillColumns()
{
	ISQueryModel QueryModel(MetaTable, ISNamespace::QMT_List);
	QueryModel.SetClassFilter(MetaTable->Alias + SYMBOL_POINT + MetaTable->Alias + "_id = 0");

	ISQuery qSelectColumns(QueryModel.GetQueryText());
	if (qSelectColumns.Execute())
	{
		QSqlRecord RecordColumn = qSelectColumns.GetRecord();

		for (int i = 0; i < RecordColumn.count(); ++i)
		{
			AppendField(MetaTable->GetField(RecordColumn.fieldName(i)));
		}
	}
}
//-----------------------------------------------------------------------------
void ISSqlModelCore::SetRecords(const std::vector<QSqlRecord> &records)
{
	beginResetModel();
	Records = records;
	endResetModel();
}
//-----------------------------------------------------------------------------
void ISSqlModelCore::Clear()
{
	if (!Records.empty())
	{
		beginResetModel();
		int Step = 0;
		while (!Records.empty())
		{
			Records.erase(--Records.end());
			if (Step == 500) //����� ��� �������� ������� �������
			{
				QThread::currentThread()->msleep(10);
				Step = 0;
			}
			else
			{
				++Step;
			}
		}
		endResetModel();
	}
}
//-----------------------------------------------------------------------------
QSqlRecord ISSqlModelCore::GetRecord(int RowIndex) const
{
	return Records.at(RowIndex);
}
//-----------------------------------------------------------------------------
void ISSqlModelCore::RemoveRecord(int Index)
{
	beginResetModel();
	Records.erase(Records.begin() + Index);
	endResetModel();
}
//-----------------------------------------------------------------------------
void ISSqlModelCore::RemoveColumn(PMetaField *MetaField)
{
	beginResetModel();
	size_t Index = ISAlgorithm::VectorIndexOf(Fields, MetaField);
	Fields.erase(Fields.begin() + Index);
	for (size_t i = 0; i < Records.size(); ++i)
	{
		QSqlRecord SqlRecord = Records[i];
		SqlRecord.remove((int)Index);
		Records[i] = SqlRecord;
	}
	endResetModel();
}
//-----------------------------------------------------------------------------
int ISSqlModelCore::GetFieldIndex(const QString &FieldName) const
{
	for (size_t i = 0; i < Fields.size(); ++i)
	{
		if (Fields[i]->Name == FieldName)
		{
			return (int)i;
		}
	}
	return -1;
}
//-----------------------------------------------------------------------------
QString ISSqlModelCore::GetFieldLocalName(const QString &FieldName) const
{
	QString LocalName;
	for (int i = 0; i < columnCount(); ++i)
	{
		QString CurrentField = headerData(i, Qt::Horizontal, Qt::UserRole).toString();
		if (CurrentField == FieldName)
		{
			LocalName = headerData(i, Qt::Horizontal).toString();
			break;
		}
	}
	IS_ASSERT(LocalName.length(), "Not found local name from field: " + FieldName);
	return LocalName;
}
//-----------------------------------------------------------------------------
PMetaField* ISSqlModelCore::GetField(int Index)
{
	return Fields[Index];
}
//-----------------------------------------------------------------------------
int ISSqlModelCore::GetSortColumn() const
{
	return SortingColumn;
}
//-----------------------------------------------------------------------------
Qt::SortOrder ISSqlModelCore::GetSortOrder() const
{
	return SortingOrder;
}
//-----------------------------------------------------------------------------
QVariant ISSqlModelCore::data(const QModelIndex &ModelIndex, int Role) const
{
	if (!ModelIndex.isValid())
	{
		return QVariant();
	}
	
	PMetaField *MetaField = MetaTable->GetField(headerData(ModelIndex.column(), Qt::Horizontal, Qt::UserRole).toString());
	ISNamespace::FieldType FieldType = MetaField->Type;

	if (Role == Qt::TextColorRole) //���� ����� ������ � �������
	{
		if (GetIsSystem(ModelIndex.row())) //���� ������ ���������
		{
			return qVariantFromValue(ISDefines::Gui::COLOR_BLUE); //�������� � ����� ������
		}
		else if (GetIsDeleted(ModelIndex.row())) //���� ������ ���������
		{
			return qVariantFromValue(ISDefines::Gui::COLOR_RED); //�������� � ������� ������
		}
	}
	else if (Role == Qt::ToolTipRole && ShowToolTip) //���� ����������� ��������� ��� ������ (ToolTip)
	{
		if (FieldType == ISNamespace::FT_Bool || FieldType == ISNamespace::FT_ByteArray)
		{
			return QString();
		}
		return ModelIndex.data().toString();
	}
	else if (Role == Qt::TextAlignmentRole) //���� ��������� ������ � ������
	{
		return ISSqlModelHelper::ValueFromTextAlignment(FieldType, MetaField->Foreign);
	}
	else if (Role == Qt::DisplayRole)
	{
		QVariant Value = Records[ModelIndex.row()].value(ModelIndex.column());
		if (Value.isNull())
		{
			return QVariant();
		}

		return ISSqlModelHelper::ValueForType(Value, FieldType);
	}

	return QVariant();
}
//-----------------------------------------------------------------------------
bool ISSqlModelCore::setData(const QModelIndex &ModelIndex, const QVariant &Value, int Role)
{
    Q_UNUSED(Role);
	QSqlRecord SqlRecord = Records[ModelIndex.row()];
	SqlRecord.setValue(ModelIndex.column(), Value);
	Records[ModelIndex.row()] = SqlRecord;
	return true;
}
//-----------------------------------------------------------------------------
QVariant ISSqlModelCore::headerData(int Section, Qt::Orientation Orientation, int Role) const
{
	QVariant Value;
	if (Orientation == Qt::Horizontal) //�������������� ���������
	{
		if (Role == Qt::DisplayRole) //����������� ���������� ������������ �������
		{
			Value = Fields[Section]->LocalListName;
		}
		else if (Role == Qt::DecorationRole) //����������� ������ ������������ �������
		{
			if (Section == SortingColumn)
			{
				Value = SortingOrder == Qt::AscendingOrder ? IconSortingUp : IconSortingDown;
			}
		}
		else if (Role == Qt::UserRole) //����������� ������������ �������
		{
			Value = Fields[Section]->Name;
		}
		else if (Role == Qt::ToolTipRole) //����������� ����� ��������� ����
		{
			Value = Fields[Section]->LocalListName;
		}
	}
	else
	{
		if (Role == Qt::DisplayRole)
		{
			return Section + 1;
		}
	}
	return Value;
}
//-----------------------------------------------------------------------------
int ISSqlModelCore::rowCount(const QModelIndex &Parent) const
{
    Q_UNUSED(Parent);
	return (int)Records.size();
}
//-----------------------------------------------------------------------------
int ISSqlModelCore::columnCount(const QModelIndex &Parent) const
{
    Q_UNUSED(Parent);
	return (int)Fields.size();
}
//-----------------------------------------------------------------------------
QModelIndex ISSqlModelCore::index(int Row, int Column, const QModelIndex &Parent) const
{
    Q_UNUSED(Parent);
	return createIndex(Row, Column);
}
//-----------------------------------------------------------------------------
QModelIndex ISSqlModelCore::parent(const QModelIndex &Index) const
{
    Q_UNUSED(Index)
	return QModelIndex();
}
//-----------------------------------------------------------------------------
void ISSqlModelCore::SetIsDeletedIndex(int IndexColumn)
{
	IsDeletedIndex = IndexColumn;
}
//-----------------------------------------------------------------------------
void ISSqlModelCore::SetIsSystemIndex(int IndexColumn)
{
	IsSystemIndex = IndexColumn;
}
//-----------------------------------------------------------------------------
void ISSqlModelCore::SetSorting(const QString &FieldName, Qt::SortOrder Order)
{
	SetSorting(GetFieldIndex(FieldName), Order);
}
//-----------------------------------------------------------------------------
void ISSqlModelCore::SetSorting(int IndexColumn, Qt::SortOrder Order)
{
	SortingColumn = IndexColumn;
	SortingOrder = Order;
}
//-----------------------------------------------------------------------------
void ISSqlModelCore::SetShowToolTip(bool show_tool_tip)
{
	ShowToolTip = show_tool_tip;
}
//-----------------------------------------------------------------------------
PMetaTable* ISSqlModelCore::GetMetaTable()
{
	return MetaTable;
}
//-----------------------------------------------------------------------------
void ISSqlModelCore::AppendField(PMetaField *MetaField)
{
	Fields.emplace_back(MetaField);
}
//-----------------------------------------------------------------------------
bool ISSqlModelCore::GetIsSystem(int RowIndex) const
{
	return Records[RowIndex].value("IsSystem").toBool();
}
//-----------------------------------------------------------------------------
bool ISSqlModelCore::GetIsDeleted(int RowIndex) const
{
	return Records[RowIndex].value("IsDeleted").toBool();
}
//-----------------------------------------------------------------------------
