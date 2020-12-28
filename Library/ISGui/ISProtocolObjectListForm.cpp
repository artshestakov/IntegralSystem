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
void ISProtocolObjectListForm::LoadDataAfterEvent()
{
	ISListBaseForm::LoadDataAfterEvent();
	HideField("ObjectID");
	HideField("TableLocalName");
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
