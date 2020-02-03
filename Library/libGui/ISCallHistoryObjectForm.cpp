#include "ISCallHistoryObjectForm.h"
//-----------------------------------------------------------------------------
ISCallHistoryObjectForm::ISCallHistoryObjectForm(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id) : ISObjectFormBase(form_type, meta_table, parent, object_id)
{
	setWindowModality(Qt::ApplicationModal);

	SetVisibleNavigationBar(false);
	SetVisibleFavorites(false);
	SetVisibleDelete(false);
	SetVisibleReRead(false);

	GetTabWidget()->tabBar()->setAutoHide(true);

	GetFieldWidget("Organization")->SetValue(object_id);
	GetFieldWidget("Note")->SetFocus();
}
//-----------------------------------------------------------------------------
ISCallHistoryObjectForm::~ISCallHistoryObjectForm()
{

}
//-----------------------------------------------------------------------------
