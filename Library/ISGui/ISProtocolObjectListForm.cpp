#include "ISProtocolObjectListForm.h"
#include "ISMetaData.h"
//-----------------------------------------------------------------------------
ISProtocolObjectListForm::ISProtocolObjectListForm(QWidget *parent) : ISProtocolSubSystem(parent)
{

}
//-----------------------------------------------------------------------------
ISProtocolObjectListForm::~ISProtocolObjectListForm()
{

}
//-----------------------------------------------------------------------------
bool ISProtocolObjectListForm::Update()
{
    bool Result = ISListBaseForm::Update();
    HideField("ObjectID");
    HideField("TableLocalName");
    return Result;
}
//-----------------------------------------------------------------------------
void ISProtocolObjectListForm::LoadData()
{
    //���������� ������� � ������ �������, ����� � ������� ����������� �������� ������ �� �������� ������� � ����������, � ������� ���������� ������� ������
    if (GetParentObjectID())
    {
        GetTcpQuery()->AddFilter("TableName", GetParentTableName());
        GetTcpQuery()->AddFilter("ObjectID", GetParentObjectID());
    }
    ISListBaseForm::LoadData();
}
//-----------------------------------------------------------------------------
