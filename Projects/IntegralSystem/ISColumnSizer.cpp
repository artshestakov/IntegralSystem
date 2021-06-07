#include "ISColumnSizer.h"
//-----------------------------------------------------------------------------
ISColumnSizer::ISColumnSizer()
{

}
//-----------------------------------------------------------------------------
ISColumnSizer::~ISColumnSizer()
{

}
//-----------------------------------------------------------------------------
ISColumnSizer& ISColumnSizer::Instance()
{
    static ISColumnSizer ColumnSizer;
    return ColumnSizer;
}
//-----------------------------------------------------------------------------
void ISColumnSizer::Initialize(const QVariantMap &VariantMap)
{
    for (const auto &TableItem : VariantMap.toStdMap())
    {
        ISStringToIntMapQt StringToIntMap;
        for (const auto &ColumnItem : TableItem.second.toMap().toStdMap())
        {
            StringToIntMap[ColumnItem.first] = ColumnItem.second.toInt();
        }
        Tables[TableItem.first] = StringToIntMap;
    }
}
//-----------------------------------------------------------------------------
QVariantMap ISColumnSizer::GetColumnSize() const
{
    QVariantMap VariantMap;
    for (const auto &TableItem : Tables)
    {
        QVariantMap TableMap;
        for (const auto &ColumnItem : TableItem.second)
        {
            TableMap[ColumnItem.first] = ColumnItem.second;
        }
        VariantMap[TableItem.first] = TableMap;
    }
    return VariantMap;
}
//-----------------------------------------------------------------------------
void ISColumnSizer::SetColumnSize(const QString &TableName, const QString &FieldName, int Size)
{
    //���� ����� ������� ��� ��� - ���������
    if (Tables.find(TableName) == Tables.end())
    {
        Tables[TableName] = ISStringToIntMapQt();
    }
    Tables[TableName][FieldName] = Size;
}
//-----------------------------------------------------------------------------
int ISColumnSizer::GetColumnSize(const QString &TableName, const QString &FieldName) const
{
    auto TableIt = Tables.find(TableName);
    if (TableIt != Tables.end()) //����� �������
    {
        auto ColumnIt = TableIt->second.find(FieldName);
        if (ColumnIt != TableIt->second.end())
        {
            return ColumnIt->second;
        }
    }
    return 0;
}
//-----------------------------------------------------------------------------
