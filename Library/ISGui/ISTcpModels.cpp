#include "ISTcpModels.h"
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
	
	//��������� ����
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

	//��������� ������
	unsigned int RecordCount = (unsigned int)records.size();
	Records.resize(RecordCount);
	for (unsigned int i = 0; i < RecordCount; ++i)
	{
		QVariantList Values = records[i].toList();
		unsigned int ValuesSize = Values.size();

		ISModelRecord Record;
		Record.ID = Values[0].toUInt();
		Record.Values.resize(ValuesSize);
		for (unsigned int j = 0; j < ValuesSize; ++j) //������� �������� ������
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
		for (size_t i = 0, c = rowCount(); i < c; ++i) //������� ��� ������
		{
			if (Records[i].ID == ObjectID) //����� ������ ��� ������
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
			if (Section == SortingColumnIndex && SortingColumnIndex != -1)
			{
				Value = SortingOrder == Qt::AscendingOrder ? SortingIconUp : SortingIconDown;
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
//-----------------------------------------------------------------------------
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

	//��������� ����
	for (const QString &FieldName : fields)
	{
		Fields.emplace_back(FieldName);
	}

	//��������� ������
	unsigned int RecordCount = (unsigned int)records.size();
	Records.resize(RecordCount);
	for (unsigned int i = 0; i < RecordCount; ++i)
	{
		QStringList Values = records[i].toStringList();
		unsigned int ValuesSize = Values.size();

		QStringList Record;
		for (unsigned int j = 0; j < ValuesSize; ++j) //������� �������� ������
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
	if (Orientation == Qt::Horizontal) //�������������� ���������
	{
		if (Role == Qt::DisplayRole) //����������� ���������� ������������ �������
		{
			Value = Fields[Section];
		}
		else if (Role == Qt::ToolTipRole) //����������� ����� ��������� ����
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
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISLogModel::ISLogModel(QObject *parent) : QAbstractItemModel(parent)
{

}
//-----------------------------------------------------------------------------
ISLogModel::~ISLogModel()
{

}
//-----------------------------------------------------------------------------
void ISLogModel::Clear()
{
	beginResetModel();
	Fields.clear();
	Records.clear();
	endResetModel();
}
//-----------------------------------------------------------------------------
void ISLogModel::SetSource(const QStringList &fields, const QVariantList &records)
{
	Clear();
	beginResetModel();

	//��������� ����
	for (const QString &FieldName : fields)
	{
		Fields.emplace_back(FieldName);
	}

	//��������� ������
	unsigned int RecordCount = (unsigned int)records.size();
	Records.resize(RecordCount);
	for (unsigned int i = 0; i < RecordCount; ++i) //������� ������
	{
		QVariantMap RecordMap = records[i].toMap();
		Records[i] = ISModelLogRecord{ RecordMap["Color"].toString(), RecordMap["Values"].toStringList() };
	}
	endResetModel();
}
//-----------------------------------------------------------------------------
unsigned int ISLogModel::GetCountDebug() const
{
	return GetCount(LOGGER_SEVERITY_DEBUG);
}
//-----------------------------------------------------------------------------
unsigned int ISLogModel::GetCountInfo() const
{
	return GetCount(LOGGER_SEVERITY_INFO);
}
//-----------------------------------------------------------------------------
unsigned int ISLogModel::GetCountWarning() const
{
	return GetCount(LOGGER_SEVERITY_WARNING);
}
//-----------------------------------------------------------------------------
unsigned int ISLogModel::GetCountError() const
{
	return GetCount(LOGGER_SEVERITY_ERROR);
}
//-----------------------------------------------------------------------------
unsigned int ISLogModel::GetCountCritical() const
{
	return GetCount(LOGGER_SEVERITY_CRITICAL);
}
//-----------------------------------------------------------------------------
unsigned int ISLogModel::GetCountTrace() const
{
	return GetCount(LOGGER_SEVERITY_TRACE);
}
//-----------------------------------------------------------------------------
unsigned int ISLogModel::GetCountAssert() const
{
	return GetCount(LOGGER_SEVERITY_ASSERT);
}
//-----------------------------------------------------------------------------
ISVectorUInt ISLogModel::Search(const QString &String)
{
	QString SearchValue = String.toLower();
	ISVectorUInt VectorUInt;
	for (unsigned int i = 0, c = Records.size(); i < c; ++i)
	{
		QString Message = Records[i].Values.back().toLower();
		if (Message.contains(SearchValue))
		{
			VectorUInt.emplace_back(i);
		}
	}
	return VectorUInt;
}
//-----------------------------------------------------------------------------
QVariant ISLogModel::data(const QModelIndex &ModelIndex, int Role) const
{
	if (!ModelIndex.isValid())
	{
		return QVariant();
	}

	QVariant Value;
	if (Role == Qt::DisplayRole)
	{
		Value = Records[ModelIndex.row()].Values[ModelIndex.column()];
	}
	else if (Role == Qt::TextColorRole)
	{
		Value = QColor(Records[ModelIndex.row()].Color);
	}
	return Value;
}
//-----------------------------------------------------------------------------
QVariant ISLogModel::headerData(int Section, Qt::Orientation Orientation, int Role) const
{
	QVariant Value;
	if (Orientation == Qt::Horizontal) //�������������� ���������
	{
		if (Role == Qt::DisplayRole) //����������� ���������� ������������ �������
		{
			Value = Fields[Section];
		}
	}
	return Value;
}
//-----------------------------------------------------------------------------
int ISLogModel::rowCount(const QModelIndex &Parent) const
{
	Q_UNUSED(Parent);
	return (int)Records.size();
}
//-----------------------------------------------------------------------------
int ISLogModel::columnCount(const QModelIndex &Parent) const
{
	Q_UNUSED(Parent);
	return (int)Fields.size();
}
//-----------------------------------------------------------------------------
QModelIndex ISLogModel::index(int Row, int Column, const QModelIndex &Parent) const
{
	Q_UNUSED(Row);
	Q_UNUSED(Column);
	Q_UNUSED(Parent);
	return createIndex(Row, Column);
}
//-----------------------------------------------------------------------------
QModelIndex ISLogModel::parent(const QModelIndex &Index) const
{
	Q_UNUSED(Index);
	return QModelIndex();
}
//-----------------------------------------------------------------------------
unsigned int ISLogModel::GetCount(const QString &Severity) const
{
	unsigned int Result = 0;
	for (size_t i = 0, c = Records.size(); i < c; ++i)
	{
		if (Records[i].Values.contains(Severity))
		{
			++Result;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
