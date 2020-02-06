#include "ISSqlModelCore.h"
#include "ISDefinesGui.h"
#include "ISBuffer.h"
#include "ISTrace.h"
#include "ISQueryModel.h"
#include "ISQuery.h"
#include "ISAssert.h"
#include "ISSystem.h"
#include "ISSqlModelHelper.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISSqlModelCore::ISSqlModelCore(PMetaClassTable *meta_table, QObject *parent) : QAbstractItemModel(parent)
{
	MetaTable = meta_table;
	CurrentSortingColumn = -1;
	CurrentColumnSortOrder = Qt::AscendingOrder;
	IsSystemIndex = -1;
	IsDeletedIndex = -1;
	ShowToolTip = false;
	
	IconSortingUp = BUFFER_ICONS("Arrow.Up");
	IconSortingDown = BUFFER_ICONS("Arrow.Down");
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
	QueryModel.SetClassFilter(MetaTable->GetAlias() + SYMBOL_POINT + MetaTable->GetAlias() + "_id = 0");

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
void ISSqlModelCore::SetRecords(const QList<QSqlRecord> &records)
{
	beginResetModel();
	Records = records;
	endResetModel();
}
//-----------------------------------------------------------------------------
void ISSqlModelCore::Clear()
{
	if (Records.count())
	{
		beginResetModel();
		
		int Step = 0;
		while (!Records.isEmpty())
		{
			Records.removeFirst();

			if (Step == 1000) //Пауза при удалении участка записей
			{
				QThread::currentThread()->msleep(1);
				Step = 0;
			}
			else
			{
				Step++;
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
	Records.removeAt(Index);
	endResetModel();
}
//-----------------------------------------------------------------------------
void ISSqlModelCore::RemoveColumn(PMetaClassField *MetaField)
{
	beginResetModel();
	
	int Index = Fields.indexOf(MetaField);
	Fields.remove(Index);

	for (int i = 0; i < Records.count(); ++i)
	{
		QSqlRecord SqlRecord = Records.at(i);
		SqlRecord.remove(Index);
		Records.replace(i, SqlRecord);
	}

	endResetModel();
}
//-----------------------------------------------------------------------------
int ISSqlModelCore::GetFieldIndex(const QString &FieldName) const
{
	for (int i = 0; i < Fields.count(); ++i)
	{
		if (Fields.at(i)->GetName() == FieldName)
		{
			return i;
		}
	}

	IS_ASSERT(false, QString("Not found field index from field name: %1.").arg(FieldName));
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
PMetaClassField* ISSqlModelCore::GetField(int Index)
{
	return Fields.at(Index);
}
//-----------------------------------------------------------------------------
QVariant ISSqlModelCore::data(const QModelIndex &ModelIndex, int Role) const
{
	if (!ModelIndex.isValid())
	{
		return QVariant();
	}
	
	PMetaClassField *MetaField = MetaTable->GetField(headerData(ModelIndex.column(), Qt::Horizontal, Qt::UserRole).toString());
	ISNamespace::FieldType FieldType = MetaField->GetType();

	if (Role == Qt::TextColorRole) //Роль цвета текста в ячейках
	{
		if (GetIsSystem(ModelIndex.row())) //Если запись системная
		{
			return qVariantFromValue(DEFINES_GUI.COLOR_BLUE); //Пометить её синим цветом
		}
		else if (GetIsDeleted(ModelIndex.row())) //Если запись удаленная
		{
			return qVariantFromValue(DEFINES_GUI.COLOR_RED); //Пометить её красным цветом
		}
	}
	else if (Role == Qt::ToolTipRole) //Роль отображения подсказки для ячейки (ToolTip)
	{
		QVariant TempValue = ModelIndex.data();
		if (!TempValue.isNull())
		{
			return ISSqlModelHelper::ValueForToolTip(ShowToolTip, TempValue, FieldType);
		}
	}
	else if (Role == Qt::TextAlignmentRole) //Роль положения текста в ячейке
	{
		return ISSqlModelHelper::ValueFromTextAlignment(FieldType, MetaField->GetForeign());
	}
	else if (Role == Qt::DisplayRole)
	{
		QVariant Value = Records.at(ModelIndex.row()).value(ModelIndex.column());
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
	QSqlRecord SqlRecord = Records.at(ModelIndex.row());
	SqlRecord.setValue(ModelIndex.column(), Value);
	Records.replace(ModelIndex.row(), SqlRecord);
	return true;
}
//-----------------------------------------------------------------------------
QVariant ISSqlModelCore::headerData(int Section, Qt::Orientation Orientation, int Role) const
{
	QVariant Value;
	if (Orientation == Qt::Horizontal) //Горизонтальный заголовок
	{
		if (Role == Qt::DisplayRole) //Отображение локального наименование столбца
		{
			Value = Fields.at(Section)->GetLocalListName();
		}
		else if (Role == Qt::DecorationRole) //Отображение иконки сортируемого столбца
		{
			Value = GetSortingIcon(Section);
		}
		else if (Role == Qt::UserRole) //Отображение наименования столбца
		{
			Value = Fields.at(Section)->GetName();
		}
		else if (Role == Qt::ToolTipRole) //Всплывающий текст заголовка поля
		{
			Value = Fields.at(Section)->GetLocalListName();
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
	return Records.count();
}
//-----------------------------------------------------------------------------
int ISSqlModelCore::columnCount(const QModelIndex &Parent) const
{
    Q_UNUSED(Parent);
	return Fields.count();
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
void ISSqlModelCore::SetCurrentSorting(int IndexColumn, Qt::SortOrder Order)
{
	CurrentSortingColumn = IndexColumn;
	CurrentColumnSortOrder = Order;
}
//-----------------------------------------------------------------------------
void ISSqlModelCore::SetShowToolTip(bool show_tooltip)
{
	ShowToolTip = show_tooltip;
}
//-----------------------------------------------------------------------------
PMetaClassTable* ISSqlModelCore::GetMetaTable()
{
	return MetaTable;
}
//-----------------------------------------------------------------------------
void ISSqlModelCore::AppendField(PMetaClassField *MetaField)
{
	Fields.append(MetaField);
}
//-----------------------------------------------------------------------------
bool ISSqlModelCore::GetIsSystem(int RowIndex) const
{
	return Records.at(RowIndex).value("IsSystem").toBool();
}
//-----------------------------------------------------------------------------
bool ISSqlModelCore::GetIsDeleted(int RowIndex) const
{
	return Records.at(RowIndex).value("IsDeleted").toBool();
}
//-----------------------------------------------------------------------------
QIcon ISSqlModelCore::GetSortingIcon(int Section) const
{
	QIcon SortingIcon;
	if (Section == CurrentSortingColumn)
	{
		if (CurrentColumnSortOrder == Qt::AscendingOrder)
		{
			SortingIcon = IconSortingUp;
		}
		else if (CurrentColumnSortOrder == Qt::DescendingOrder)
		{
			SortingIcon = IconSortingDown;
		}
	}
	return SortingIcon;
}
//-----------------------------------------------------------------------------
