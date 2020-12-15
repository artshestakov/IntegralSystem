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
	//��������� ����
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

	//��������� ������
	int RecordCount = records.size();
	Records.resize(RecordCount);
	for (int i = 0; i < RecordCount; ++i)
	{
		Records[i] = QVector<QVariant>::fromList(records[i].toList()).toStdVector();
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
			Type == ISNamespace::FT_Phone) //������������ �� ������
		{
			Value = Qt::AlignCenter;
		}
		else if ((Type == ISNamespace::FT_Int ||
			Type == ISNamespace::FT_Double ||
			Type == ISNamespace::FT_Money) &&
			!FieldModel.IsForeign) //��� ����� ������������ �� ������ ������ (���� �� ������� ����)
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
	if (Orientation == Qt::Horizontal) //�������������� ���������
	{
		if (Role == Qt::DisplayRole) //����������� ���������� ������������ �������
		{
			Value = Fields[Section].LocalName;
		}
		else if (Role == Qt::DecorationRole) //����������� ������ ������������ �������
		{
			//if (Section == SortingColumn)
			{
				//Value = SortingOrder == Qt::AscendingOrder ? IconSortingUp : IconSortingDown;
			}
		}
		else if (Role == Qt::UserRole) //����������� ������������ �������
		{
			Value = Fields[Section].Name;
		}
		else if (Role == Qt::ToolTipRole) //����������� ����� ��������� ����
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
