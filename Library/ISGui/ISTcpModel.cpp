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
	endResetModel();
	SortingColumnIndex = -1;
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

		ISFieldModel FieldModel;
		FieldModel.Name = FieldMap["Name"].toString();
		FieldModel.LocalName = FieldMap["LocalName"].toString();
		FieldModel.Type = static_cast<ISNamespace::FieldType>(FieldMap["Type"].toInt());
		FieldModel.IsForeign = FieldMap["IsForeign"].toBool();
		Fields.emplace_back(FieldModel);
	}

	//Заполняем строки
	int RecordCount = records.size();
	Records.resize(RecordCount);
	for (int i = 0; i < RecordCount; ++i)
	{
		Records[i] = QVector<QVariant>::fromList(records[i].toList()).toStdVector();
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
QVariant ISTcpModel::data(const QModelIndex &ModelIndex, int Role) const
{
	if (!ModelIndex.isValid())
	{
		return QVariant();
	}

	QVariant Value;
	if (Role == Qt::DisplayRole)
	{
		Value = Records[ModelIndex.row()][ModelIndex.column()].toString();
	}
	else if (Role == Qt::TextAlignmentRole)
	{
		ISFieldModel FieldModel = Fields[ModelIndex.column()];
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
	else if (Role == Qt::TextColorRole)
	{
		if (Records[ModelIndex.row()][ModelIndex.column()].toBool())
		{
			Value = qVariantFromValue(ISDefines::Gui::COLOR_BLUE);
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
