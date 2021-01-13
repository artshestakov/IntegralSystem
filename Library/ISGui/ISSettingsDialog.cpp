#include "ISSettingsDialog.h"
#include "ISDefinesGui.h"
#include "ISBuffer.h"
#include "ISAssert.h"
#include "ISMessageBox.h"
#include "ISLocalization.h"
#include "ISCore.h"
#include "ISSettings.h"
#include "ISTcpQuery.h"
#include "ISLabels.h"
#include "ISControls.h"
#include "ISFileDialog.h"
#include "ISGui.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISSettingsDialog::ISSettingsDialog(const QString &SettingGroupUID) : ISInterfaceDialogForm()
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
	connect(ListWidget, &QListWidget::itemSelectionChanged, this, &ISSettingsDialog::ItemSelectionChanged);
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
	connect(ButtonDefault, &ISPushButton::clicked, this, &ISSettingsDialog::DefaultSettings);
	LayoutBottom->addWidget(ButtonDefault);

	LayoutBottom->addStretch();

	ButtonDialog = new ISButtonDialog(this);
	ButtonDialog->SetApplyEnabled(false);
	ButtonDialog->layout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);
	connect(ButtonDialog, &ISButtonDialog::Apply, this, &ISSettingsDialog::Save);
	connect(ButtonDialog, &ISButtonDialog::Close, this, &ISSettingsDialog::close);
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
ISSettingsDialog::~ISSettingsDialog()
{

}
//-----------------------------------------------------------------------------
void ISSettingsDialog::CreateCentralPanel()
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
void ISSettingsDialog::CreateSettings()
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

			ISFieldEditBase *FieldEditBase = ISGui::CreateColumnForField(ScrollArea, MetaSetting->Type, MetaSetting->WidgetEditName);
			FieldEditBase->SetValue(SETTING_VALUE(MetaSetting->UID));
			FieldEditBase->SetModificationFlag(false);
			connect(FieldEditBase, &ISFieldEditBase::ValueChange, this, &ISSettingsDialog::DataChanged);
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
void ISSettingsDialog::Save()
{
	for (const auto &MapItem : Fields) //������� ��� ���� ��������
	{
		if (MapItem.second->GetModificationFlag()) //���� �������� ���� ���� �������� - ��������� ���
		{
			ISSettings::Instance().SetValue(MapItem.first, MapItem.second->GetValue());
		}
	}
	close();
}
//-----------------------------------------------------------------------------
void ISSettingsDialog::DefaultSettings()
{
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.ChangeSettingsToDefault"), LANG("ThisActionCanNotUndone")))
	{
		ISTcpQuery qUserSettingsReset(API_USER_SETTINGS_RESET);
		if (qUserSettingsReset.Execute())
		{
			QVariantMap ResultMap = qUserSettingsReset.GetAnswer()["Result"].toMap();
			for (const auto &MapItem : ResultMap.toStdMap())
			{
				ISSettings::Instance().SetValue(MapItem.first, MapItem.second);
			}
			ISMessageBox::ShowInformation(this, LANG("Message.Information.AppliocationWillBeRestart"));
			close();
		}
		else
		{
			ISMessageBox::ShowCritical(this, qUserSettingsReset.GetErrorString());
		}
	}
}
//-----------------------------------------------------------------------------
QListWidgetItem* ISSettingsDialog::CreateItemGroup(ISMetaSettingsGroup *MetaGroup)
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
void ISSettingsDialog::ItemSelectionChanged()
{
	ListWidget->SetFontItems(ISDefines::Gui::FONT_TAHOMA_9);
	QListWidgetItem *ClickedItem = ListWidget->currentItem();
	ClickedItem->setFont(ISDefines::Gui::FONT_TAHOMA_9_BOLD);
	TabWidget->setCurrentIndex(ListWidget->row(ClickedItem));
	LabelCurrentGroup->setText(ClickedItem->text());

	QString GroupHint = Groups[ClickedItem]->Hint;
	LabelCurrentGroupHint->setText(GroupHint.isEmpty() ? QString() : SYMBOL_STAR + SYMBOL_SPACE + GroupHint);
	LabelCurrentGroupHint->setVisible(!GroupHint.isEmpty());
}
//-----------------------------------------------------------------------------
void ISSettingsDialog::DataChanged()
{
	ButtonDialog->SetApplyEnabled(true);
}
//-----------------------------------------------------------------------------