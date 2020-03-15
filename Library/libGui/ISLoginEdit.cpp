#include "ISLoginEdit.h"
#include "ISQuery.h"
#include "ISLocalization.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
static QString QS_LOGIN_EXIST = PREPARE_QUERY("SELECT COUNT(*) "
											  "FROM _users "
											  "WHERE usrs_login = :Login");
//-----------------------------------------------------------------------------
ISLoginEdit::ISLoginEdit(QWidget *parent)
	: ISLineEdit(parent),
	Valid(false)
{
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
		Valid = qSelectLogin.ReadColumn("count").toBool();
		if (Valid)
		{
			if (!QToolTip::isVisible())
			{
				QPoint Point = GetLineEdit()->mapToGlobal(QPoint());
				Point.setX(Point.x() - 2);
				Point.setY(Point.y() + 8);
				QToolTip::showText(Point, LANG("ThisLoginAlreadyExist"), GetLineEdit(), QRect(), 5000);
			}
		}
		else
		{
			if (QToolTip::isVisible())
			{
				QToolTip::hideText();
			}
		}
	}

	connect(this, &ISLoginEdit::DataChanged, this, &ISLoginEdit::LoginChanged);
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
