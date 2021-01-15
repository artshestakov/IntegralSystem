#include "ISViewModel.h"
//-----------------------------------------------------------------------------
ISViewModel::ISViewModel(QObject *parent) : QAbstractItemModel(parent)
{

}
//-----------------------------------------------------------------------------
ISViewModel::~ISViewModel()
{

}
//-----------------------------------------------------------------------------
void ISViewModel::Clear()
{
	beginResetModel();
	Fields.clear();
	Records.clear();
	endResetModel();
}
//-----------------------------------------------------------------------------
void ISViewModel::SetSource(const QStringList &fields, const QVariantList &records)
{
	Clear();
	beginResetModel();
	
	//Заполняем поля
	for (const QString &FieldName : fields)
	{
		Fields.emplace_back(FieldName);
	}

	//Заполняем строки
	unsigned int RecordCount = (unsigned int)records.size();
	Records.resize(RecordCount);
	for (unsigned int i = 0; i < RecordCount; ++i)
	{
		QStringList Values = records[i].toStringList();
		unsigned int ValuesSize = Values.size();

		QStringList Record;
		for (unsigned int j = 0; j < ValuesSize; ++j) //Обходим значения записи
		{
			Record.append(Values[j]);
		}
		Records[i] = Record;
	}
	endResetModel();
}
//-----------------------------------------------------------------------------
QVariant ISViewModel::data(const QModelIndex &ModelIndex, int Role) const
{
	if (!ModelIndex.isValid())
	{
		return QVariant();
	}

	QVariant Value;
	if (Role == Qt::DisplayRole)
	{
		Value = Records[ModelIndex.row()][ModelIndex.column()];
	}
	return Value;
}
//-----------------------------------------------------------------------------
QVariant ISViewModel::headerData(int Section, Qt::Orientation Orientation, int Role) const
{
	QVariant Value;
	if (Orientation == Qt::Horizontal) //Горизонтальный заголовок
	{
		if (Role == Qt::DisplayRole) //Отображение локального наименование столбца
		{
			Value = Fields[Section];
		}
		else if (Role == Qt::ToolTipRole) //Всплывающий текст заголовка поля
		{
			Value = Fields[Section];
		}
	}
	return Value;
}
//-----------------------------------------------------------------------------
int ISViewModel::rowCount(const QModelIndex &Parent) const
{
	Q_UNUSED(Parent);
	return (int)Records.size();
}
//-----------------------------------------------------------------------------
int ISViewModel::columnCount(const QModelIndex &Parent) const
{
	Q_UNUSED(Parent);
	return (int)Fields.size();
}
//-----------------------------------------------------------------------------
QModelIndex ISViewModel::index(int Row, int Column, const QModelIndex &Parent) const
{
	Q_UNUSED(Row);
	Q_UNUSED(Column);
	Q_UNUSED(Parent);
	return createIndex(Row, Column);
}
//-----------------------------------------------------------------------------
QModelIndex ISViewModel::parent(const QModelIndex &Index) const
{
	Q_UNUSED(Index);
	return QModelIndex();
}
//-----------------------------------------------------------------------------
