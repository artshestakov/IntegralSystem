#include "ISTcpModel.h"
//-----------------------------------------------------------------------------
ISTcpModel::ISTcpModel(QObject *parent) : QAbstractItemModel(parent)
{

}
//-----------------------------------------------------------------------------
ISTcpModel::~ISTcpModel()
{

}
//-----------------------------------------------------------------------------
void ISTcpModel::SetData(const QVariantList &fields, const QVariantList &records)
{
	//Заполняем поля
	for (const QVariant &Field : fields)
	{
		QVariantMap FieldMap = Field.toMap();

		PMetaField MetaField;
		MetaField.Name = FieldMap["Name"].toString();
		MetaField.LocalListName = FieldMap["LocalName"].toString();
		Fields.emplace_back(MetaField);
	}

	//Заполняем строки
	int RecordCount = records.size();
	Records.resize(RecordCount);
	for (int i = 0; i < RecordCount; ++i)
	{
		QVariantList Values = records[i].toList();
		Records[i] = QVector<QVariant>::fromList(Values).toStdVector();
	}
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
	return Value;
}
//-----------------------------------------------------------------------------
bool ISTcpModel::setData(const QModelIndex &ModelIndex, const QVariant &Value, int Role)
{
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
			Value = Fields[Section].LocalListName;
		}
		else if (Role == Qt::DecorationRole) //Отображение иконки сортируемого столбца
		{
			//if (Section == SortingColumn)
			{
				//Value = SortingOrder == Qt::AscendingOrder ? IconSortingUp : IconSortingDown;
			}
		}
		else if (Role == Qt::UserRole) //Отображение наименования столбца
		{
			Value = Fields[Section].Name;
		}
		else if (Role == Qt::ToolTipRole) //Всплывающий текст заголовка поля
		{
			Value = Fields[Section].LocalListName;
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
int ISTcpModel::rowCount(const QModelIndex &Parent) const
{
	return (int)Records.size();
}
//-----------------------------------------------------------------------------
int ISTcpModel::columnCount(const QModelIndex &Parent) const
{
	return (int)Fields.size();
}
//-----------------------------------------------------------------------------
QModelIndex ISTcpModel::index(int Row, int Column, const QModelIndex &Parent) const
{
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
