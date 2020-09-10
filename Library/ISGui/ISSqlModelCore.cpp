#include "ISSqlModelCore.h"
#include "ISDefinesGui.h"
#include "ISBuffer.h"
#include "ISTrace.h"
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
void ISSqlModelCore::SetRecords(const std::vector<QSqlRecord> &records, const std::vector<QString> &fields)
{
	beginResetModel();
	Records = records;
	if (Fields.empty()) //Если список полей ещё не заполнен (первое открытие модели) - заполняем
	{
		for (const QString &FieldName : fields)
		{
			Fields.emplace_back(MetaTable->GetField(FieldName));
		}
	}
	endResetModel();
}
//-----------------------------------------------------------------------------
void ISSqlModelCore::Clear()
{
	if (!Records.empty())
	{
		beginResetModel();
		for (long long i = Records.size() - 1; i >= 0; --i)
		{
			Records.erase(Records.begin() + i);
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
		if (headerData(i, Qt::Horizontal, Qt::UserRole).toString() == FieldName)
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

	if (Role == Qt::TextColorRole) //Роль цвета текста в ячейках
	{
		if (Records[ModelIndex.row()].value("IsSystem").toBool()) //Если запись системная
		{
			return qVariantFromValue(ISDefines::Gui::COLOR_BLUE); //Пометить её синим цветом
		}
		else if (Records[ModelIndex.row()].value("IsDeleted").toBool()) //Если запись удаленная
		{
			return qVariantFromValue(ISDefines::Gui::COLOR_RED); //Пометить её красным цветом
		}
	}
	else if (Role == Qt::ToolTipRole && ShowToolTip) //Роль отображения подсказки для ячейки (ToolTip)
	{
		//Если тип поля булевый или набор байт - возвращаем пустую строку
		return FieldType == ISNamespace::FT_Bool || FieldType == ISNamespace::FT_ByteArray ? QString() : ModelIndex.data().toString();
	}
	else if (Role == Qt::TextAlignmentRole) //Роль положения текста в ячейке
	{
		return ISSqlModelHelper::ValueFromTextAlignment(FieldType, MetaField->Foreign);
	}
	else if (Role == Qt::DisplayRole)
	{
		//Еслли значение пустое - возвращаем невалидный QVariant, иначе - преобразовываем значение в соответствии с его типом
		QVariant Value = Records[ModelIndex.row()].value(ModelIndex.column());
		return Value.isNull() ? QVariant() : ISSqlModelHelper::ValueForType(Value, FieldType);
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
	if (Orientation == Qt::Horizontal) //Горизонтальный заголовок
	{
		if (Role == Qt::DisplayRole) //Отображение локального наименование столбца
		{
			Value = Fields[Section]->LocalListName;
		}
		else if (Role == Qt::DecorationRole) //Отображение иконки сортируемого столбца
		{
			if (Section == SortingColumn)
			{
				Value = SortingOrder == Qt::AscendingOrder ? IconSortingUp : IconSortingDown;
			}
		}
		else if (Role == Qt::UserRole) //Отображение наименования столбца
		{
			Value = Fields[Section]->Name;
		}
		else if (Role == Qt::ToolTipRole) //Всплывающий текст заголовка поля
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
	return (int)(Fields.size());
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
