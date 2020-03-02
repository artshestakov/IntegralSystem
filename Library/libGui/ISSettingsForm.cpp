#include "ISSettingsForm.h"
#include "ISDefinesGui.h"
#include "ISBuffer.h"
#include "ISAssert.h"
#include "ISMessageBox.h"
#include "ISLocalization.h"
#include "ISCore.h"
#include "ISSettings.h"
#include "ISQuery.h"
#include "ISProgressForm.h"
#include "ISLabels.h"
#include "ISControls.h"
#include "ISFileDialog.h"
#include "ISGui.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
#include "ISQueryText.h"
//-----------------------------------------------------------------------------
static QString QS_SETTINGS = PREPARE_QUERY("SELECT stgs_uid, stgs_defaultvalue, stgs_localname FROM _settings WHERE NOT stgs_isdeleted ORDER BY stgs_id");
//-----------------------------------------------------------------------------
static QString QS_SETTING_EXPORT = PREPARE_QUERY("SELECT usst_setting, usst_value FROM _usersettings WHERE NOT usst_isdeleted ORDER BY usst_id");
//-----------------------------------------------------------------------------
static QString QU_SETTINGS_DEFAULT = PREPARE_QUERY("UPDATE _usersettings SET usst_value = :SettingValue WHERE usst_setting = :SettingUID");
//-----------------------------------------------------------------------------
ISSettingsForm::ISSettingsForm(const QString &SettingGroupUID, QWidget *parent) : ISInterfaceDialogForm(parent)
{
	setWindowIcon(BUFFER_ICONS("Settings"));
	setWindowTitle(LANG("Settings"));
	setMinimumSize(ISDefines::Gui::SIZE_MAIN_WINDOW_MINIMUM);

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

	ISPushButton *ButtonExport = new ISPushButton(this);
	ButtonExport->setText(LANG("Settings.Export"));
	ButtonExport->setIcon(BUFFER_ICONS("Arrow.Up"));
	connect(ButtonExport, &ISPushButton::clicked, this, &ISSettingsForm::Export);
	LayoutBottom->addWidget(ButtonExport);

	ISPushButton *ButtonImport = new ISPushButton(this);
	ButtonImport->setText(LANG("Settings.Import"));
	ButtonImport->setIcon(BUFFER_ICONS("Arrow.Down"));
	connect(ButtonImport, &ISPushButton::clicked, this, &ISSettingsForm::Import);
	LayoutBottom->addWidget(ButtonImport);

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
	for (int i = 0; i < ISSettings::GetInstance().GetSettingGroups().count(); ++i)
	{
		ISMetaSettingsGroup *MetaGroup = ISSettings::GetInstance().GetSettingGroups()[i];
		QListWidgetItem* ListWidgetItem = CreateItemGroup(MetaGroup);

		QFormLayout *FormLayout = new QFormLayout();
		
		ISScrollArea *ScrollArea = new ISScrollArea(TabWidget);
		ScrollArea->widget()->setLayout(FormLayout);
		TabWidget->addTab(ScrollArea, QString());

		for (int j = 0; j < MetaGroup->Settings.count(); ++j)
		{
			ISMetaSetting *MetaSetting = MetaGroup->Settings[j];
			
			ISQLabel *LabelRow = new ISQLabel(ScrollArea);
			LabelRow->setText(MetaSetting->LocalName + ':');

			if (!MetaSetting->Hint.isEmpty())
			{
				LabelRow->setToolTip(ISGui::PrepareLongToolTip(MetaSetting->Hint));
				LabelRow->setCursor(CURSOR_WHATS_THIS);
				ISGui::SetFontWidgetUnderline(LabelRow, true);
				connect(LabelRow, &ISQLabel::Clicked, this, &ISSettingsForm::LabelRowClicked);
			}

			ISFieldEditBase *FieldEditBase = ISGui::CreateColumnForField(ScrollArea, MetaSetting->SettingType, MetaSetting->WidgetEditName);

			QVariant Value = SETTING_VALUE(MetaSetting->UID);
			FieldEditBase->SetValue(Value);
			connect(FieldEditBase, &ISFieldEditBase::ValueChange, this, &ISSettingsForm::DataChanged);
			FormLayout->addRow(LabelRow, FieldEditBase);
			Fields.insert(MetaSetting->UID, FieldEditBase);
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
		++Iterator;
		ProgressForm.setValue(Iterator);
		ProgressForm.SetText(LANG("SavedSetting").arg(Iterator).arg(Fields.count()));

		QString SettingUID = MapItem.first;
		ISFieldEditBase *FieldEditBase = MapItem.second;
		if (FieldEditBase)
		{
			ISSettings::GetInstance().SaveValue(SettingUID, FieldEditBase->GetValue());
		}
	}

	ProgressForm.hide();

	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.SettingsSaved")))
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
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.ChangeSettingsToDefault"), LANG("ThisActionCanNotUndone")))
	{
		ISQuery qSelectSettings(QS_SETTINGS);
		if (qSelectSettings.Execute())
		{
			ISProgressForm ProgressForm(0, qSelectSettings.GetCountResultRows(), this);
			ProgressForm.show();
			
			int Iterator = 0;
			while (qSelectSettings.Next())
			{
				++Iterator;
				QString SettingUID = qSelectSettings.ReadColumn("stgs_uid").toString();
				QVariant SettingValue = qSelectSettings.ReadColumn("stgs_defaultvalue");
				QString SettingLocalName = qSelectSettings.ReadColumn("stgs_localname").toString();

				ProgressForm.setValue(ProgressForm.value() + 1);
				ProgressForm.SetText(LANG("SavedSetting").arg(Iterator).arg(qSelectSettings.GetCountResultRows()));

				ISQuery qUpdateDefault(QU_SETTINGS_DEFAULT);
				qUpdateDefault.BindValue(":SettingValue", SettingValue);
				qUpdateDefault.BindValue(":SettingUID", SettingUID);
				qUpdateDefault.Execute();
			}

			ProgressForm.hide();

			ISMessageBox::ShowWarning(this, LANG("Message.Warning.AppliocationWillBeRestart"));
			Restart();
		}
	}
}
//-----------------------------------------------------------------------------
void ISSettingsForm::Export()
{
	QString FilePath = ISFileDialog::GetSaveFileName(this, LANG("File.Filter.SettingsIntegralSystem"));
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
				QString SettingUID = '{' + qSelect.ReadColumn("usst_setting").toString().toUpper() + '}';
				QString SettingValue = qSelect.ReadColumn("usst_value").toString();

				Settings.setValue("Settings/" + SettingUID, SettingValue);
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISSettingsForm::Import()
{
	QString FilePath = ISFileDialog::GetOpenFileName(this, QString(), LANG("File.Filter.SettingsIntegralSystem"));
	if (FilePath.length())
	{
		QSettings Settings(FilePath, QSettings::IniFormat);
		QStringList AllKeys = Settings.allKeys();

		ISProgressForm ProgressForm(0, AllKeys.count(), this);
		ProgressForm.show();

		for (int i = 0; i < AllKeys.count(); ++i)
		{
			ProgressForm.setValue(i);
			ProgressForm.SetText(LANG("ImportSetting").arg(i).arg(Fields.count()));

			QString SettingKey = AllKeys[i];

			QString SettingUID = SettingKey.split('/')[1];
			QString SettingValue = Settings.value(SettingKey).toString();

			ISSettings::GetInstance().SaveValue(SettingUID, SettingValue);
		}

		ProgressForm.close();

		if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.ImportedSettings")))
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
	ISGui::ChangeUser();
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
	QListWidgetItem *ClickedItem = ListWidget->currentItem();

	for (int i = 0; i < ListWidget->count(); ++i)
	{
		QListWidgetItem *ListWidgetItem = ListWidget->item(i);
		ListWidgetItem->setFont(ISDefines::Gui::FONT_TAHOMA_9);
	}

	ClickedItem->setFont(ISDefines::Gui::FONT_TAHOMA_9_BOLD);

	int IndexClicked = ListWidget->row(ClickedItem);
	TabWidget->setCurrentIndex(IndexClicked);

	LabelCurrentGroup->setText(ClickedItem->text());

	QString GroupHint = Groups.value(ClickedItem)->Hint;
	if (!GroupHint.isEmpty())
	{
		LabelCurrentGroupHint->setText(ISDefines::Core::SYMBOL_CIRCLE + SYMBOL_SPACE + GroupHint);
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
