#include "ISComboStartedParagraph.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
static QString QS_PARAFGRAPH = PREPARE_QUERY("SELECT prhs_localname, prhs_uid "
											 "FROM _paragraphs "
											 "WHERE NOT prhs_isdeleted "
											 "ORDER BY prhs_orderid");
//-----------------------------------------------------------------------------
ISComboStartedParagraph::ISComboStartedParagraph(QWidget *parent) : ISComboSettingEdit(parent)
{
	ISQuery qSelect(QS_PARAFGRAPH);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			AddItem(qSelect.ReadColumn("prhs_localname").toString(), ISUuid(qSelect.ReadColumn("prhs_uid")));
		}
	}
}
//-----------------------------------------------------------------------------
ISComboStartedParagraph::~ISComboStartedParagraph()
{

}
//-----------------------------------------------------------------------------
