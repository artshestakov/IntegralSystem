#include "ISProtocolObjectListForm.h"
#include "ISMetaData.h"
//-----------------------------------------------------------------------------
ISProtocolObjectListForm::ISProtocolObjectListForm(QWidget *parent) : ISProtocolListForm(parent)
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
	//Добавление условия в фильтр таблицы, чтобы в таблице отображался протокол только по текущему объекту и подсистеме, в которой содержится текущий объект
	if (GetParentObjectID())
	{
		QString ClassFilter = GetQueryModel()->GetClassFilter();
		ClassFilter += " prtc_tablename = '" + GetParentTableName() + '\'';
		ClassFilter += " AND prtc_objectid = " + QString::number(GetParentObjectID());
		GetQueryModel()->SetClassFilter(ClassFilter);
	}

	ISListBaseForm::LoadData();
}
//-----------------------------------------------------------------------------
