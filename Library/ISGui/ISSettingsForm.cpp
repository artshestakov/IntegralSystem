#include "ISSettingsForm.h"
#include "ISDefinesGui.h"
#include "ISBuffer.h"
#include "ISAssert.h"
#include "ISMessageBox.h"
#include "ISLocalization.h"
#include "ISCore.h"
#include "ISSettings.h"
#include "ISQuery.h"
#include "ISLabels.h"
#include "ISControls.h"
#include "ISFileDialog.h"
#include "ISGui.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
static QString QS_SETTINGS = PREPARE_QUERY("SELECT stgs_uid, stgs_defaultvalue FROM _settings WHERE NOT stgs_isdeleted ORDER BY stgs_id");
//-----------------------------------------------------------------------------
static QString QU_SETTINGS_DEFAULT = PREPARE_QUERY("UPDATE _usersettings SET usst_value = :SettingValue WHERE usst_setting = :SettingUID");
//-----------------------------------------------------------------------------
ISSettingsForm::ISSettingsForm(const QString &SettingGroupUID) : ISInterfaceDialogForm()
{
	setWindowIcon(BUFFER_ICONS("Settings"));
	setWindowTitle(LANG("Settings"));
	setMinimumSize(900, 600);

	Layout = new QHBoxLayout();
	Layout->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);
	GetMainLayout()->addLayout(Layout);

	ListWidget = new ISListWidget(this);
	ListWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
	ListWidget->setCursor(CURSOR_POINTING_HAND);
	ListWidget->setTabKeyNavigation(false);
	ListWidget->setTabKeyNavigation(false);
	connect(ListWidget, &QListWidget::itemSelectionChanged, this, &ISSettingsForm::ItemSelectionChanged);
	Layout->addWidget(ListWidget);

	CreateCentralPanel();
	CreateSettings();

	GetMainLayout()->addWidget(ISControls::CreateHorizontalLine(this));

	QHBoxLayout *LayoutBottom = new QHBoxLayout();
	LayoutBottom->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);
	GetMainLayout()->addLayout(LayoutBottom);

	ISPushButton *ButtonDefault = new ISPushButton(this);
	ButtonDefault->setText(LANG("SettingsDefault"));
	ButtonDefault->setIcon(BUFFER_ICONS("SettingsDefault"));
	connect(ButtonDefault, &ISPushButton::clicked, this, &ISSettingsForm::DefaultSettings);
	LayoutBottom->addWidget(ButtonDefault);

	LayoutBottom->addStretch();

	ButtonDialog = new ISButtonDialog(this);
	ButtonDialog->SetApplyEnabled(false);
	ButtonDialog->layout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);
	connect(ButtonDialog, &ISButtonDialog::Apply, this, &ISSettingsForm::Save);
	connect(ButtonDialog, &ISButtonDialog::Close, this, &ISSettingsForm::close);
	LayoutBottom->addWidget(ButtonDialog, 0, Qt::AlignRight);

	if (!SettingGroupUID.isEmpty())
	{
		for (int i = 0; i < ListWidget->count(); ++i)
		{
			if (ListWidget->item(i)->data(Qt::UserRole).toString() == SettingGroupUID)
			{
				ListWidget->setCurrentRow(i);
				break;
			}
		}
	}
}
//-----------------------------------------------------------------------------
ISSettingsForm::~ISSettingsForm()
{

}
//-----------------------------------------------------------------------------
void ISSettingsForm::CreateCentralPanel()
{
	LayoutCentral = new QVBoxLayout();
	Layout->addLayout(LayoutCentral);

	QVBoxLayout *LayoutTitle = new QVBoxLayout();

	QWidget *WidgetTitle = new QWidget(this);
	WidgetTitle->setLayout(LayoutTitle);
	LayoutCentral->addWidget(WidgetTitle);

	LabelCurrentGroup = new QLabel(WidgetTitle);
	LabelCurrentGroup->setFont(ISDefines::Gui::FONT_TAHOMA_12_BOLD);
	LayoutTitle->addWidget(LabelCurrentGroup, 0, Qt::AlignLeft);

	LabelCurrentGroupHint = new QLabel(WidgetTitle);
	LabelCurrentGroupHint->setVisible(false);
	LabelCurrentGroupHint->setCursor(CURSOR_WHATS_THIS);
	LayoutTitle->addWidget(LabelCurrentGroupHint, 0, Qt::AlignLeft);

	LayoutCentral->addWidget(ISControls::CreateHorizontalLine(this));

	TabWidget = new QTabWidget(this);
	TabWidget->setDocumentMode(true);
	TabWidget->tabBar()->setVisible(false);
	LayoutCentral->addWidget(TabWidget);
}
//-----------------------------------------------------------------------------
void ISSettingsForm::CreateSettings()
{
	for (ISMetaSettingsGroup *MetaGroup : ISSettings::Instance().GetSettingGroups())
	{
		QListWidgetItem* ListWidgetItem = CreateItemGroup(MetaGroup);

		QFormLayout *FormLayout = new QFormLayout();
		
		ISScrollArea *ScrollArea = new ISScrollArea(TabWidget);
		ScrollArea->widget()->setLayout(FormLayout);
		TabWidget->addTab(ScrollArea, QString());

		for (ISMetaSetting *MetaSetting : MetaGroup->Settings)
		{
			QLabel *LabelRow = new QLabel(MetaSetting->LocalName + ':', ScrollArea);
			if (!MetaSetting->Hint.isEmpty())
			{
				LabelRow->setToolTip(ISGui::PrepareLongToolTip(MetaSetting->Hint));
				LabelRow->setCursor(CURSOR_WHATS_THIS);
			}

			ISFieldEditBase *FieldEditBase = ISGui::CreateColumnForField(ScrollArea, MetaSetting->SettingType, MetaSetting->WidgetEditName);

			FieldEditBase->SetValue(SETTING_VALUE(MetaSetting->UID));
			FieldEditBase->SetModificationFlag(false);
			connect(FieldEditBase, &ISFieldEditBase::ValueChange, this, &ISSettingsForm::DataChanged);
			FormLayout->addRow(LabelRow, FieldEditBase);
			Fields.emplace(MetaSetting->UID, FieldEditBase);
		}
		Groups.emplace(ListWidgetItem, MetaGroup);
	}

	if (ListWidget->count())
	{
		ListWidget->setCurrentItem(ListWidget->BeginItem());
	}
}
//-----------------------------------------------------------------------------
void ISSettingsForm::Save()
{
	for (const auto &MapItem : Fields) //Обходим все поля настроек
	{
		if (MapItem.second->GetModificationFlag()) //Если значение поля было изменено - сохраняем его
		{
			ISSettings::Instance().SetValue(MapItem.first, MapItem.second->GetValue());
		}
	}
	close();
}
//-----------------------------------------------------------------------------
void ISSettingsForm::DefaultSettings()
{
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.ChangeSettingsToDefault"), LANG("ThisActionCanNotUndone")))
	{
		ISQuery qSelectSettings(QS_SETTINGS);
		if (qSelectSettings.Execute())
		{
			while (qSelectSettings.Next())
			{
				ISQuery qUpdateDefault(QU_SETTINGS_DEFAULT);
				qUpdateDefault.BindValue(":SettingValue", qSelectSettings.ReadColumn("stgs_defaultvalue"));
				qUpdateDefault.BindValue(":SettingUID", qSelectSettings.ReadColumn("stgs_uid"));
				qUpdateDefault.Execute();
			}
			ISMessageBox::ShowInformation(this, LANG("Message.Information.AppliocationWillBeRestart"));
			close();
		}
	}
}
//-----------------------------------------------------------------------------
QListWidgetItem* ISSettingsForm::CreateItemGroup(ISMetaSettingsGroup *MetaGroup)
{
	QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);
	ListWidgetItem->setText(MetaGroup->LocalName);
	ListWidgetItem->setToolTip(MetaGroup->Hint);
	ListWidgetItem->setSizeHint(QSize(ListWidgetItem->sizeHint().width(), 30));
	ListWidgetItem->setData(Qt::UserRole, MetaGroup->UID);
	if (MetaGroup->System)
	{
		ListWidgetItem->setIcon(BUFFER_ICONS(MetaGroup->IconName));
	}
	return ListWidgetItem;
}
//-----------------------------------------------------------------------------
void ISSettingsForm::ItemSelectionChanged()
{
	ListWidget->SetFontItems(ISDefines::Gui::FONT_TAHOMA_9);
	QListWidgetItem *ClickedItem = ListWidget->currentItem();
	ClickedItem->setFont(ISDefines::Gui::FONT_TAHOMA_9_BOLD);
	TabWidget->setCurrentIndex(ListWidget->row(ClickedItem));
	LabelCurrentGroup->setText(ClickedItem->text());

	QString GroupHint = Groups[ClickedItem]->Hint;
	LabelCurrentGroupHint->setText(GroupHint.isEmpty() ? QString() : ISDefines::Core::SYMBOL_CIRCLE + SYMBOL_SPACE + GroupHint);
	LabelCurrentGroupHint->setVisible(!GroupHint.isEmpty());
}
//-----------------------------------------------------------------------------
void ISSettingsForm::DataChanged()
{
	ButtonDialog->SetApplyEnabled(true);
}
//-----------------------------------------------------------------------------
