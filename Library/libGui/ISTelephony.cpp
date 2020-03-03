#include "ISTelephony.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
static QString QS_TELEPHONY = PREPARE_QUERY("SELECT COUNT(*) "
                                            "FROM _asteriskpattern "
                                            "WHERE NOT aspt_isdeleted "
                                            "AND aspt_user = currentuserid()");
//-----------------------------------------------------------------------------
ISTelephony::ISTelephony() : QObject()
{

}
//-----------------------------------------------------------------------------
ISTelephony::~ISTelephony()
{

}
//-----------------------------------------------------------------------------
bool ISTelephony::CheckSetUp()
{
    ISQuery qSelect(QS_TELEPHONY);
    if (qSelect.ExecuteFirst())
    {
		if (qSelect.ReadColumn("count").toInt())
		{
				return true;
		}
    }

    return false;
}
//-----------------------------------------------------------------------------
