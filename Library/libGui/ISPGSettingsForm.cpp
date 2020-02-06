#include "ISPGSettingsForm.h"
#include "ISQuery.h"
#include "ISScrollArea.h"
#include "ISGui.h"
#include "ISQueryText.h"
//-----------------------------------------------------------------------------
static QString QS_CATEGORY = PREPARE_QUERY("SELECT DISTINCT category "
										   "FROM pg_settings "
										   "ORDER BY category");
//-----------------------------------------------------------------------------
static QString QS_SETTINGS = PREPARE_QUERY("SELECT name, setting "
										   "FROM pg_settings "
										   "WHERE category = :Category "
										   "ORDER BY name");
//-----------------------------------------------------------------------------
ISPGSettingsForm::ISPGSettingsForm(QWidget *parent) : ISInterfaceForm(parent)
{
	ISScrollArea *ScrollArea = new ISScrollArea(this);
	ScrollArea->widget()->setLayout(new QVBoxLayout());
	GetMainLayout()->addWidget(ScrollArea);

	ISQuery qSelectCategory(QS_CATEGORY);
	if (qSelectCategory.Execute())
	{
		while (qSelectCategory.Next())
		{
			QString Category = qSelectCategory.ReadColumn("category").toString();

			QFormLayout *FormLayout = new QFormLayout();
			QGroupBox *GroupBox = new QGroupBox(ScrollArea);
			GroupBox->setTitle(Category);
			GroupBox->setLayout(FormLayout);
			ScrollArea->widget()->layout()->addWidget(GroupBox);

			ISQuery qSelectSettings(QS_SETTINGS);
			qSelectSettings.BindValue(":Category", Category);
			if (qSelectSettings.Execute())
			{
				while (qSelectSettings.Next())
				{
					QString SettingName = qSelectSettings.ReadColumn("name").toString();
					QString SettingValue = qSelectSettings.ReadColumn("setting").toString();

					QLabel *LabelValue = new QLabel(GroupBox);
					LabelValue->setText(SettingValue);
					ISGui::SetFontWidgetBold(LabelValue, true);
					FormLayout->addRow(SettingName + ':', LabelValue);
				}
			}
		}
	}
}
//-----------------------------------------------------------------------------
ISPGSettingsForm::~ISPGSettingsForm()
{

}
//-----------------------------------------------------------------------------
