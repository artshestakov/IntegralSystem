#include "ISTcpModel.h"
#include "ISBuffer.h"
#include "ISDefinesGui.h"
//-----------------------------------------------------------------------------
ISTcpModel::ISTcpModel(QObject *parent)
	: QAbstractItemModel(parent),
	SortingColumnIndex(-1),
	SortingOrder(Qt::AscendingOrder),
	SortingIconUp(BUFFER_ICONS("Table.Sorting.Up")),
	SortingIconDown(BUFFER_ICONS("Table.Sorting.Down"))
{

}
//-----------------------------------------------------------------------------
ISTcpModel::~ISTcpModel()
{

}
//-----------------------------------------------------------------------------
void ISTcpModel::Clear()
{
	beginResetModel();
	Fields.clear();
	Records.clear();
	SortingColumnIndex = -1;
	endResetModel();
}
//-----------------------------------------------------------------------------
void ISTcpModel::SetSource(const QVariantList &fields, const QVariantList &records)
{
	Clear();
	beginResetModel();
	
	//Заполняем поля
	for (const QVariant &Field : fields)
	{
		QVariantMap FieldMap = Field.toMap();

		ISModelField FieldModel;
		FieldModel.Index = FieldMap["Index"].toUInt();
		FieldModel.Name = FieldMap["Name"].toString();
		FieldModel.LocalName = FieldMap["LocalName"].toString();
		FieldModel.Type = static_cast<ISNamespace::FieldType>(FieldMap["Type"].toInt());
		FieldModel.IsForeign = FieldMap["IsForeign"].toBool();
		FieldModel.IsSystem = FieldMap["IsSystem"].toBool();
		Fields.emplace_back(FieldModel);
	}

	//Заполняем строки
	size_t RecordCount = (size_t)records.size();
	Records.resize(RecordCount);
	for (size_t i = 0; i < RecordCount; ++i)
	{
		QVariantList Values = records[i].toList();
		size_t ValuesSize = Values.size();

		ISModelRecord Record;
		Record.ID = Values[0].toUInt();
		Record.Values.resize(ValuesSize);
		for (size_t j = 0; j < ValuesSize; ++j) //Обходим значения записи
		{
			Record.Values[j] = Values[j];
		}
		Records[i] = Record;
	}
	endResetModel();
}
//-----------------------------------------------------------------------------
void ISTcpModel::SetSorting(const QString &sorting_field, Qt::SortOrder sorting_order)
{
	SortingColumnIndex = GetFieldIndex(sorting_field);
	SortingOrder = sorting_order;
}
//-----------------------------------------------------------------------------
void ISTcpModel::RemoveRecordIndex(unsigned int RowIndex)
{
	if (!Records.empty())
	{
		beginResetModel();
		Records.erase(Records.begin() + RowIndex);
		endResetModel();
	}
}
//-----------------------------------------------------------------------------
void ISTcpModel::RemoveRecordID(unsigned int ObjectID)
{
	if (!Records.empty())
	{
		beginResetModel();
		for (size_t i = 0, c = rowCount(); i < c; ++i) //Обходим все записи
		{
			if (Records[i].ID == ObjectID) //Нашли нужную нам запись
			{
				Records.erase(Records.begin() + i);
				break;
			}
		}
		endResetModel();
	}
}
//-----------------------------------------------------------------------------
ISModelRecord& ISTcpModel::GetRecord(int Index)
{
	return Records[Index];
}
//-----------------------------------------------------------------------------
QVariant ISTcpModel::GetRecordValue(int Index, const QString &FieldName)
{
	return GetRecord(Index).Values[GetFieldIndex(FieldName)];
}
//-----------------------------------------------------------------------------
ISVectorUInt ISTcpModel::GetIDs() const
{
	size_t RowCount = (int)rowCount();
	ISVectorUInt VectorID;
	VectorID.resize(RowCount);
	for (size_t i = 0; i < RowCount; ++i)
	{
		VectorID[i] = Records[i].ID;
	}
	return VectorID;
}
//-----------------------------------------------------------------------------
int ISTcpModel::GetFieldIndex(const QString &FieldName) const
{
	for (size_t i = 0, c = Fields.size(); i < c; ++i)
	{
		if (Fields[i].Name == FieldName)
		{
			return (int)i;
		}
	}
	return -1;
}
//-----------------------------------------------------------------------------
ISModelField ISTcpModel::GetField(const QString &FieldName)
{
	return GetField(GetFieldIndex(FieldName));
}
//-----------------------------------------------------------------------------
ISModelField ISTcpModel::GetField(unsigned int Index)
{
	return Fields[Index];
}
//-----------------------------------------------------------------------------
QVariant ISTcpModel::data(const QModelIndex &ModelIndex, int Role) const
{
	if (!ModelIndex.isValid())
	{
		return QVariant();
	}

	QVariant Value;
	if (Role == Qt::TextColorRole)
	{
		if (Records[ModelIndex.row()].Values[GetFieldIndex("IsSystem")].toBool())
		{
			return qVariantFromValue(ISDefines::Gui::COLOR_BLUE);
		}
	}
	else if (Role == Qt::DisplayRole)
	{
		Value = Records[ModelIndex.row()].Values[ModelIndex.column()].toString();
	}
	else if (Role == Qt::TextAlignmentRole)
	{
		ISModelField FieldModel = Fields[ModelIndex.column()];
		ISNamespace::FieldType Type = FieldModel.Type;
		if (Type == ISNamespace::FT_Date ||
			Type == ISNamespace::FT_Time ||
			Type == ISNamespace::FT_DateTime ||
			Type == ISNamespace::FT_Birthday ||
			Type == ISNamespace::FT_Seconds ||
			Type == ISNamespace::FT_Year ||
			Type == ISNamespace::FT_Phone) //Расположение по центру
		{
			Value = Qt::AlignCenter;
		}
		else if ((Type == ISNamespace::FT_Int ||
			Type == ISNamespace::FT_Double ||
			Type == ISNamespace::FT_Money) &&
			!FieldModel.IsForeign) //Для чисел расположение по центру справа (если не внешний ключ)
		{
			Value = Qt::AlignRight + Qt::AlignVCenter;
		}
	}
	return Value;
}
//-----------------------------------------------------------------------------
bool ISTcpModel::setData(const QModelIndex &ModelIndex, const QVariant &Value, int Role)
{
	Q_UNUSED(ModelIndex);
	Q_UNUSED(Value);
	Q_UNUSED(Role);
	return true;
}
//-----------------------------------------------------------------------------
QVariant ISTcpModel::headerData(int Section, Qt::Orientation Orientation, int Role) const
{
	QVariant Value;
	if (Orientation == Qt::Horizontal) //Горизонтальный заголовок
	{
		if (Role == Qt::DisplayRole) //Отображение локального наименование столбца
		{
			Value = Fields[Section].LocalName;
		}
		else if (Role == Qt::DecorationRole) //Отображение иконки сортируемого столбца
		{
			if (Section == SortingColumnIndex && SortingColumnIndex != -1)
			{
				Value = SortingOrder == Qt::AscendingOrder ? SortingIconUp : SortingIconDown;
			}
		}
		else if (Role == Qt::UserRole) //Отображение наименования столбца
		{
			Value = Fields[Section].Name;
		}
		else if (Role == Qt::ToolTipRole) //Всплывающий текст заголовка поля
		{
			Value = Fields[Section].LocalName;
		}
	}
	else
	{
		if (Role == Qt::DisplayRole)
		{
			Value = Section + 1;
		}
	}
	return Value;
}
//-----------------------------------------------------------------------------
int ISTcpModel::rowCount(const QModelIndex &Parent) const
{
	Q_UNUSED(Parent);
	return (int)Records.size();
}
//-----------------------------------------------------------------------------
int ISTcpModel::columnCount(const QModelIndex &Parent) const
{
	Q_UNUSED(Parent);
	return (int)Fields.size();
}
//-----------------------------------------------------------------------------
QModelIndex ISTcpModel::index(int Row, int Column, const QModelIndex &Parent) const
{
	Q_UNUSED(Row);
	Q_UNUSED(Column);
	Q_UNUSED(Parent);
	return createIndex(Row, Column);
}
//-----------------------------------------------------------------------------
QModelIndex ISTcpModel::parent(const QModelIndex &Index) const
{
	Q_UNUSED(Index);
	return QModelIndex();
}
//-----------------------------------------------------------------------------
