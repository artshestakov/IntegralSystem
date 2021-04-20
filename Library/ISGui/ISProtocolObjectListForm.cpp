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
    //Добавление условия в фильтр таблицы, чтобы в таблице отображался протокол только по текущему объекту и подсистеме, в которой содержится текущий объект
    if (GetParentObjectID())
    {
        GetTcpQuery()->AddFilter("TableName", GetParentTableName());
        GetTcpQuery()->AddFilter("ObjectID", GetParentObjectID());
    }
    ISListBaseForm::LoadData();
}
//-----------------------------------------------------------------------------
