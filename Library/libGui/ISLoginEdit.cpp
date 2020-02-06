#include "ISLoginEdit.h"
#include "ISQuery.h"
#include "ISLocalization.h"
#include "ISGui.h"
#include "ISQueryText.h"
//-----------------------------------------------------------------------------
static QString QS_LOGIN_EXIST = PREPARE_QUERY("SELECT COUNT(*) "
											  "FROM _users "
											  "WHERE usrs_login = :Login");
//-----------------------------------------------------------------------------
ISLoginEdit::ISLoginEdit(QWidget *parent) : ISLineEdit(parent)
{
	Valid = false;
	connect(this, &ISLoginEdit::DataChanged, this, &ISLoginEdit::LoginChanged);
}
//-----------------------------------------------------------------------------
ISLoginEdit::~ISLoginEdit()
{

}
//-----------------------------------------------------------------------------
bool ISLoginEdit::IsValid() const
{
	return Valid;
}
//-----------------------------------------------------------------------------
void ISLoginEdit::LoginChanged()
{
	ISGui::SetWaitGlobalCursor(true);
	disconnect(this, &ISLoginEdit::DataChanged, this, &ISLoginEdit::LoginChanged);

	ISQuery qSelectLogin(QS_LOGIN_EXIST);
	qSelectLogin.BindValue(":Login", GetValue());
	if (qSelectLogin.ExecuteFirst())
	{
		if (qSelectLogin.ReadColumn("count").toInt())
		{
			if (!QToolTip::isVisible())
			{
				QPoint Point = GetLineEdit()->mapToGlobal(QPoint());
				Point.setX(Point.x() - 2);
				Point.setY(Point.y() + 8);
				QToolTip::showText(Point, LANG("ThisLoginAlreadyExist"), GetLineEdit(), QRect(), 5000);
			}

			Valid = false;
		}
		else
		{
			if (QToolTip::isVisible())
			{
				QToolTip::hideText();
			}

			Valid = true;
		}
	}

	connect(this, &ISLoginEdit::DataChanged, this, &ISLoginEdit::LoginChanged);
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
