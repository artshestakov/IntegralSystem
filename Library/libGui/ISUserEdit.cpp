#include "ISUserEdit.h"
#include "ISDefinesGui.h"
#include "ISQuery.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMetaUser.h"
#include "ISConstants.h"
#include "ISQueryText.h"
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
	SetToolTip(LANG("ClickFromShowUserList"));

	ISQuery qSelect(QS_USERS);
	if (qSelect.Execute())
	{
		if (qSelect.GetCountResultRows())
		{
			AddItem(QIcon(BUFFER_ICONS("Arrow.Down").pixmap(DEFINES_GUI.SIZE_16_16)), LANG("SelectUser"), QVariant());
			while (qSelect.Next())
			{
				QString UserFullName = qSelect.ReadColumn("userfullname").toString();
				int UserID = qSelect.ReadColumn("usrs_id").toInt();

				if (UserID == ISMetaUser::GetInstance().GetData()->ID)
				{
					UserFullName += " (" + LANG("You") + ')';
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
