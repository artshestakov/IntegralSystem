#include "StdAfx.h"
#include "ISToolBarAccessTable.h"
#include "EXDefines.h"
#include "ISBuffer.h"
#include "ISQuery.h"
#include "ISUserRoleEntity.h"
//-----------------------------------------------------------------------------
static QString QS_GROUP_ACCESS_TABLE_TYPE = PREPARE_QUERY("SELECT gatt_uid, gatt_name, gatt_icon "
														  "FROM _groupaccesstabletype "
														  "WHERE NOT gatt_isdeleted "
														  "ORDER BY gatt_order");
//-----------------------------------------------------------------------------
ISToolBarAccessTable::ISToolBarAccessTable(int GroupID, const ISUuid &TableUID, const QString &TableName, QWidget *parent) : QToolBar(parent)
{
	setToolButtonStyle(Qt::ToolButtonIconOnly);
	setIconSize(SIZE_20_20);

	ISQuery qSelectAccess(QS_GROUP_ACCESS_TABLE_TYPE);
	if (qSelectAccess.Execute())
	{
		while (qSelectAccess.Next())
		{
			ISUuid AccessUID = qSelectAccess.ReadColumn("gatt_uid");
			QString Name = qSelectAccess.ReadColumn("gatt_name").toString();
			QString Icon = qSelectAccess.ReadColumn("gatt_icon").toString();

			QAction *Action = new QAction(this);
			Action->setToolTip(Name);
			Action->setIcon(BUFFER_ICONS(Icon));
			Action->setCheckable(true);
			Action->setProperty("TableUID", TableUID);
			Action->setProperty("AccessUID", AccessUID);
			Action->setProperty("TableName", TableName);
			addAction(Action);
			widgetForAction(Action)->setCursor(CURSOR_POINTING_HAND);

			if (ISUserRoleEntity::CheckExistTableAccess(GroupID, TableUID, AccessUID))
			{
				Action->setChecked(true);
			}
		}
	}
}
//-----------------------------------------------------------------------------
ISToolBarAccessTable::~ISToolBarAccessTable()
{

}
//-----------------------------------------------------------------------------
