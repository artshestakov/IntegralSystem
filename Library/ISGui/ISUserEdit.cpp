#include "StdAfx.h"
#include "ISUserEdit.h"
#include "EXDefines.h"
#include "ISQuery.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMetaUser.h"
//-----------------------------------------------------------------------------
static QString QS_USERS = PREPARE_QUERY("SELECT usrs_id, userfullname(usrs_id) "
										"FROM _users "
										"WHERE NOT usrs_isdeleted "
										"ORDER BY userfullname");
//-----------------------------------------------------------------------------
ISUserEdit::ISUserEdit(QWidget *parent) : ISComboEdit(parent)
{
	SetEditable(false);
	SetCursor(CURSOR_POINTING_HAND);
	SetToolTip(LOCALIZATION("ClickFromShowUserList"));

	ISQuery qSelect(QS_USERS);
	if (qSelect.Execute())
	{
		if (qSelect.GetCountResultRows())
		{
			AddItem(QIcon(BUFFER_ICONS("Arrow.Down").pixmap(SIZE_16_16)), LOCALIZATION("SelectUser"), QVariant());
			while (qSelect.Next())
			{
				QString UserFullName = qSelect.ReadColumn("userfullname").toString();
				int UserID = qSelect.ReadColumn("usrs_id").toInt();

				if (UserID == CURRENT_USER_ID)
				{
					UserFullName += " (" + LOCALIZATION("You") + ")";
				}

				AddItem(UserFullName, UserID);
			}
		}
	}
}
//-----------------------------------------------------------------------------
ISUserEdit::~ISUserEdit()
{

}
//-----------------------------------------------------------------------------
