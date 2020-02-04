#include "ISSqlModelView.h"
#include "EXDefines.h"
#include "ISAssert.h"
#include "ISTrace.h"
#include "ISBuffer.h"
#include "ISSqlModelHelper.h"
//-----------------------------------------------------------------------------
ISSqlModelView::ISSqlModelView(PMetaClassTable *meta_table, QObject *parent) : QAbstractItemModel(parent)
{
	MetaTable = meta_table;
	CurrentSortingColumn = 0;
	CurrentColumnSortOrder = Qt::AscendingOrder;
	
	IconSortingUp = BUFFER_ICONS("Arrow.Up");
	IconSortingDown = BUFFER_ICONS("Arrow.Down");
}
//-----------------------------------------------------------------------------
ISSqlModelView::~ISSqlModelView()
{

}
//-----------------------------------------------------------------------------
QSqlRecord ISSqlModelView::GetRecord(int RowIndex) const
{
	return Records.at(RowIndex);
}
//-----------------------------------------------------------------------------
int ISSqlModelView::GetFieldIndex(const QString &FieldName) const
{
	for (int i = 0; i < MetaTable->GetFields().count(); ++i)
	{
		if (MetaTable->GetFields().at(i)->GetName() == FieldName)
		{
			return i;
		}
	}

	IS_ASSERT(false, QString("Not found field index from field name: %1.").arg(FieldName));
	return -1;
}
//-----------------------------------------------------------------------------
void ISSqlModelView::Clear()
{
	IS_TRACE();

	beginResetModel();

	int Step = 0;
	while (!Records.isEmpty())
	{
		Records.removeFirst();

		if (Step == 1000) //����� ��� �������� ������� �������
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
//-----------------------------------------------------------------------------
void ISSqlModelView::SetRecords(const QList<QSqlRecord> &records)
{
	beginResetModel();
	Records = records;
	endResetModel();
}
//-----------------------------------------------------------------------------
QVariant ISSqlModelView::data(const QModelIndex &ModelIndex, int Role) const
{
	if (!ModelIndex.isValid())
	{
		return QVariant();
	}

	PMetaClassField *MetaField = MetaTable->GetField(headerData(ModelIndex.column(), Qt::Horizontal, Qt::UserRole).toString());
	ISNamespace::FieldType FieldType = MetaField->GetType();

	if (Role == Qt::DisplayRole)
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
QVariant ISSqlModelView::headerData(int Section, Qt::Orientation Orientation, int Role) const
{
	QVariant Value;

	if (Orientation == Qt::Horizontal) //�������������� ���������
	{
		if (Role == Qt::DisplayRole) //����������� ���������� ������������ �������
		{
			Value = MetaTable->GetFields().at(Section)->GetLocalListName();
		}
		else if (Role == Qt::DecorationRole) //����������� ������ ������������ �������
		{
			Value = GetSortingIcon(Section);
		}
		else if (Role == Qt::UserRole) //����������� ������������ �������
		{
			Value = MetaTable->GetFields().at(Section)->GetName();
		}
		else if (Role == Qt::ToolTipRole) //����������� ����� ��������� ����
		{
			MetaTable->GetFields().at(Section)->GetLocalListName();
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
int ISSqlModelView::rowCount(const QModelIndex &Parent) const
{
    Q_UNUSED(Parent);
	return Records.count();
}
//-----------------------------------------------------------------------------
int ISSqlModelView::columnCount(const QModelIndex &Parent) const
{
    Q_UNUSED(Parent);
	return MetaTable->GetFields().count();
}
//-----------------------------------------------------------------------------
QModelIndex ISSqlModelView::index(int Row, int Column, const QModelIndex &Parent) const
{
    Q_UNUSED(Parent);
	return createIndex(Row, Column);
}
//-----------------------------------------------------------------------------
QModelIndex ISSqlModelView::parent(const QModelIndex &Index) const
{
    Q_UNUSED(Index);
	return QModelIndex();
}
//-----------------------------------------------------------------------------
void ISSqlModelView::SetCurrentSorting(int IndexColumn, Qt::SortOrder Order)
{
	CurrentSortingColumn = IndexColumn;
	CurrentColumnSortOrder = Order;
}
//-----------------------------------------------------------------------------
QIcon ISSqlModelView::GetSortingIcon(int Section) const
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
