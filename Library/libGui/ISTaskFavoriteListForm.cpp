#include "ISTaskFavoriteListForm.h"
#include "ISQueryText.h"
//-----------------------------------------------------------------------------
static QString QS_FAVORITES = PREPARE_QUERY("SELECT fvts_objectid "
											"FROM _favorites "
											"WHERE NOT fvts_isdeleted "
											"AND fvts_user = currentuserid() "
											"AND fvts_tablename = '_Task'");
//-----------------------------------------------------------------------------
ISTaskFavoriteListForm::ISTaskFavoriteListForm(QWidget *parent) : ISTaskBaseListForm(parent)
{
	GetQueryModel()->SetClassFilter("task_id IN(SELECT fvts_objectid FROM _favorites WHERE NOT fvts_isdeleted AND fvts_user = currentuserid() AND fvts_tablename = '_Task' ORDER BY fvts_id)");
}
//-----------------------------------------------------------------------------
ISTaskFavoriteListForm::~ISTaskFavoriteListForm()
{

}
//-----------------------------------------------------------------------------
void ISTaskFavoriteListForm::Update()
{
	/*QString StringIN;
	int TaskFavorites = 0;

	ISQuery qSelect(QS_FAVORITES);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			StringIN += qSelect.ReadColumn("fvts_objectid").toString() + ", ";
			TaskFavorites++;
		}
	}

	if (TaskFavorites)
	{
		ISSystem::RemoveLastSymbolFromString(StringIN, 2);
		GetQueryModel()->SetClassFilter("task_id IN(" + StringIN + ')');
	}
	*/
	ISTaskBaseListForm::Update();
}
//-----------------------------------------------------------------------------
