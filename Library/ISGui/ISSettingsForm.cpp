#include "StdAfx.h"
#include "ISSettingsForm.h"
#include "ISDefines.h"
#include "ISBuffer.h"
#include "ISAssert.h"
#include "ISSystem.h"
#include "ISMessageBox.h"
#include "ISLocalization.h"
#include "ISCore.h"
#include "ISSettings.h"
#include "ISMetaSettingsGroup.h"
#include "ISQuery.h"
#include "ISProgressForm.h"
#include "ISQLabel.h"
#include "ISControls.h"
#include "ISMemoryObjects.h"
#include "ISFileDialog.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
static QString QS_SETTINGS = PREPARE_QUERY("SELECT stgs_uid, stgs_defaultvalue, stgs_localname FROM _settings WHERE NOT stgs_isdeleted ORDER BY stgs_id");
//-----------------------------------------------------------------------------
static QString QS_SETTING_EXPORT = PREPARE_QUERY("SELECT usst_setting, usst_value FROM _usersettings WHERE NOT usst_isdeleted ORDER BY usst_id");
//-----------------------------------------------------------------------------
static QString QU_SETTING_IMPORT = PREPARE_QUERY("UPDATE _usersettings SET usst_value = :Value WHERE usst_setting = :SettingUID");
//-----------------------------------------------------------------------------
static QString QU_SETTINGS_DEFAULT = PREPARE_QUERY("UPDATE _usersettings SET usst_value = :SettingValue WHERE usst_setting = :SettingUID");
//-----------------------------------------------------------------------------
ISSettingsForm::ISSettingsForm(const QString &SettingGroupUID, QWidget *parent) : ISInterfaceDialogForm(parent)
{
	setWindowIcon(BUFFER_ICONS("Settings"));
	setWindowTitle(LOCALIZATION("Settings"));
	setMinimumSize(SIZE_MAIN_WINDOW_MINIMUM);

	Layout = new QHBoxLayout();
	Layout->setContentsMargins(LAYOUT_MARGINS_10_PX);
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
	LayoutBottom->setContentsMargins(LAYOUT_MARGINS_10_PX);
	GetMainLayout()->addLayout(LayoutBottom);

	ISPushButton *ButtonDefault = new ISPushButton(this);
	ButtonDefault->setText(LOCALIZATION("SettingsDefault"));
	ButtonDefault->setIcon(BUFFER_ICONS("SettingsDefault"));
	connect(ButtonDefault, &ISPushButton::clicked, this, &ISSettingsForm::DefaultSettings);
	LayoutBottom->addWidget(ButtonDefault);

	ISPushButton *ButtonExport = new ISPushButton(this);
	ButtonExport->setText(LOCALIZATION("Settings.Export"));
	ButtonExport->setIcon(BUFFER_ICONS("Arrow.Up"));
	connect(ButtonExport, &ISPushButton::clicked, this, &ISSettingsForm::Export);
	LayoutBottom->addWidget(ButtonExport);

	ISPushButton *ButtonImport = new ISPushButton(this);
	ButtonImport->setText(LOCALIZATION("Settings.Import"));
	ButtonImport->setIcon(BUFFER_ICONS("Arrow.Down"));
	connect(ButtonImport, &ISPushButton::clicked, this, &ISSettingsForm::Import);
	LayoutBottom->addWidget(ButtonImport);

	LayoutBottom->addStretch();

	ButtonDialog = new ISButtonDialog(this);
	ButtonDialog->SetApplyEnabled(false);
	ButtonDialog->layout()->setContentsMargins(LAYOUT_MARGINS_NULL);
	connect(ButtonDialog, &ISButtonDialog::Apply, this, &ISSettingsForm::Save);
	connect(ButtonDialog, &ISButtonDialog::Close, this, &ISSettingsForm::close);
	LayoutBottom->addWidget(ButtonDialog, 0, Qt::AlignRight);

	if (SettingGroupUID.length())
	{
		for (int i = 0; i < ListWidget->count(); i++)
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
	LabelCurrentGroup->setFont(FONT_TAHOMA_12_BOLD);
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
	for (int i = 0; i < ISSettings::GetInstance().GetSettingGroups().count(); i++)
	{
		ISMetaSettingsGroup *MetaGroup = ISSettings::GetInstance().GetSettingGroups().at(i);
		QListWidgetItem* ListWidgetItem = CreateItemGroup(MetaGroup);

		QFormLayout *FormLayout = new QFormLayout();
		
		ISScrollArea *ScrollArea = new ISScrollArea(TabWidget);
		ScrollArea->widget()->setLayout(FormLayout);
		TabWidget->addTab(ScrollArea, QString());

		for (int j = 0; j < MetaGroup->GetSettings().count(); j++)
		{
			ISMetaSetting *MetaSetting = MetaGroup->GetSettings().at(j);
			
			ISQLabel *LabelRow = new ISQLabel(ScrollArea);
			LabelRow->setText(MetaSetting->GetLocalName() + ":");

			if (MetaSetting->GetHint().length())
			{
				LabelRow->setToolTip(ISSystem::PrepareLongToolTip(MetaSetting->GetHint()));
				LabelRow->setCursor(CURSOR_WHATS_THIS);
				ISSystem::SetFontWidgetUnderline(LabelRow, true);
				connect(LabelRow, &ISQLabel::Clicked, this, &ISSettingsForm::LabelRowClicked);
			}

			ISFieldEditBase *FieldEditBase = ISGui::CreateColumnForField(ScrollArea, MetaSetting->GetType(), MetaSetting->GetWidgetEditName());

			QVariant Value = SETTING_VALUE(MetaSetting->GetUID());
			FieldEditBase->SetValue(Value);
			connect(FieldEditBase, &ISFieldEditBase::ValueChange, this, &ISSettingsForm::DataChanged);
			FormLayout->addRow(LabelRow, FieldEditBase);
			Fields.insert(MetaSetting->GetUID(), FieldEditBase);
		}
		
		Groups.insert(ListWidgetItem, MetaGroup);
	}

	if (ListWidget->count())
	{
		QListWidgetItem *BeginItem = ListWidget->item(0);
		ListWidget->setCurrentItem(BeginItem);
	}
}
//-----------------------------------------------------------------------------
void ISSettingsForm::Save()
{
	ISProgressForm ProgressForm(0, Fields.count(), this);
	ProgressForm.show();

	int Iterator = 0;

	for (const auto &MapItem : Fields.toStdMap())
	{
		Iterator++;
		ProgressForm.setValue(Iterator);
		ProgressForm.SetText(LOCALIZATION("SavedSetting").arg(Iterator).arg(Fields.count()));

		QString SettingUID = MapItem.first;
		ISFieldEditBase *FieldEditBase = MapItem.second;
		if (FieldEditBase)
		{
			ISSettings::GetInstance().SaveValue(SettingUID, FieldEditBase->GetValue());
		}
	}

	ProgressForm.hide();

	if (ISMessageBox::ShowQuestion(this, LOCALIZATION("Message.Question.SettingsSaved")))
	{
		Restart();
	}
	else
	{
		close();
	}
}
//-----------------------------------------------------------------------------
void ISSettingsForm::DefaultSettings()
{
	if (ISMessageBox::ShowQuestion(this, LOCALIZATION("Message.Question.ChangeSettingsToDefault"), LOCALIZATION("ThisActionCanNotUndone")))
	{
		ISQuery qSelectSettings(QS_SETTINGS);
		if (qSelectSettings.Execute())
		{
			ISProgressForm ProgressForm(0, qSelectSettings.GetCountResultRows(), this);
			ProgressForm.show();
			
			int Iterator = 0;

			while (qSelectSettings.Next())
			{
				Iterator++;

				QString SettingUID = qSelectSettings.ReadColumn("stgs_uid").toString();
				QVariant SettingValue = qSelectSettings.ReadColumn("stgs_defaultvalue");
				QString SettingLocalName = qSelectSettings.ReadColumn("stgs_localname").toString();

				ProgressForm.setValue(ProgressForm.value() + 1);
				ProgressForm.SetText(LOCALIZATION("SavedSetting").arg(Iterator).arg(qSelectSettings.GetCountResultRows()));

				ISQuery qUpdateDefault(QU_SETTINGS_DEFAULT);
				qUpdateDefault.BindValue(":SettingValue", SettingValue);
				qUpdateDefault.BindValue(":SettingUID", SettingUID);
				qUpdateDefault.Execute();
			}

			ProgressForm.hide();

			ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Warning.AppliocationWillBeRestart"));
			Restart();
		}
	}
}
//-----------------------------------------------------------------------------
void ISSettingsForm::Export()
{
	QString FilePath = ISFileDialog::GetSaveFileName(this, LOCALIZATION("File.Filter.SettingsIntegralSystem"));
	if (FilePath.length())
	{
		QFile FileSettings(FilePath);

		if (FileSettings.exists())
		{
			FileSettings.remove();
		}

		if (FileSettings.open(QIODevice::WriteOnly))
		{
			FileSettings.close();
		}

		QSettings Settings(FilePath, QSettings::IniFormat, this);
		
		ISQuery qSelect(QS_SETTING_EXPORT);
		if (qSelect.Execute())
		{
			while (qSelect.Next())
			{
				QString SettingUID = "{" + qSelect.ReadColumn("usst_setting").toString().toUpper() + "}";
				QString SettingValue = qSelect.ReadColumn("usst_value").toString();

				Settings.setValue("Settings/" + SettingUID, SettingValue);
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISSettingsForm::Import()
{
	QString FilePath = ISFileDialog::GetOpenFileName(this, QString(), LOCALIZATION("File.Filter.SettingsIntegralSystem"));
	if (FilePath.length())
	{
		QSettings Settings(FilePath, QSettings::IniFormat);
		QStringList AllKeys = Settings.allKeys();

		ISProgressForm ProgressForm(0, AllKeys.count(), this);
		ProgressForm.show();

		for (int i = 0; i < AllKeys.count(); i++)
		{
			ProgressForm.setValue(i);
			ProgressForm.SetText(LOCALIZATION("ImportSetting").arg(i).arg(Fields.count()));

			QString SettingKey = AllKeys.at(i);

			QString SettingUID = SettingKey.split("/").at(1);
			QString SettingValue = Settings.value(SettingKey).toString();

			ISSettings::GetInstance().SaveValue(SettingUID, SettingValue);
		}

		ProgressForm.close();

		if (ISMessageBox::ShowQuestion(this, LOCALIZATION("Message.Question.ImportedSettings")))
		{
			Restart();
		}
		else
		{
			close();
		}
	}
}
//-----------------------------------------------------------------------------
void ISSettingsForm::Restart()
{
	ISCore::ChangeUser();
}
//-----------------------------------------------------------------------------
QListWidgetItem* ISSettingsForm::CreateItemGroup(ISMetaSettingsGroup *MetaGroup)
{
	QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);
	ListWidgetItem->setText(MetaGroup->GetLocalName());
	ListWidgetItem->setToolTip(MetaGroup->GetHint());
	ListWidgetItem->setSizeHint(QSize(ListWidgetItem->sizeHint().width(), 30));
	ListWidgetItem->setData(Qt::UserRole, MetaGroup->GetUID());

	if (MetaGroup->GetSystem())
	{
		ListWidgetItem->setIcon(BUFFER_ICONS(MetaGroup->GetIconName()));
	}

	return ListWidgetItem;
}
//-----------------------------------------------------------------------------
void ISSettingsForm::ItemSelectionChanged()
{
	QListWidgetItem *ClickedItem = ListWidget->currentItem();

	for (int i = 0; i < ListWidget->count(); i++)
	{
		QListWidgetItem *ListWidgetItem = ListWidget->item(i);
		ListWidgetItem->setFont(FONT_TAHOMA_9);
	}

	ClickedItem->setFont(FONT_TAHOMA_9_BOLD);

	int IndexClicked = ListWidget->row(ClickedItem);
	TabWidget->setCurrentIndex(IndexClicked);

	LabelCurrentGroup->setText(ClickedItem->text());

	QString GroupHint = Groups.value(ClickedItem)->GetHint();
	if (GroupHint.length())
	{
		LabelCurrentGroupHint->setText(SYMBOL_CIRCLE + SYMBOL_SPACE + GroupHint);
		LabelCurrentGroupHint->setVisible(true);
	}
	else
	{
		LabelCurrentGroupHint->clear();
		LabelCurrentGroupHint->setVisible(false);
	}
}
//-----------------------------------------------------------------------------
void ISSettingsForm::DataChanged()
{
	ButtonDialog->SetApplyEnabled(true);
}
//-----------------------------------------------------------------------------
void ISSettingsForm::LabelRowClicked()
{
	ISQLabel *LabelRow = dynamic_cast<ISQLabel*>(sender());
	if (LabelRow)
	{
		ISMessageBox::ShowInformation(this, LabelRow->toolTip());
	}
}
//-----------------------------------------------------------------------------
