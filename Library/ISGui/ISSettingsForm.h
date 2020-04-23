#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
#include "ISStructs.h"
#include "ISFieldEditBase.h"
#include "ISButtonDialog.h"
#include "ISListWidget.h"
//-----------------------------------------------------------------------------
class ISSettingsForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISSettingsForm(const QString &SettingGroupUID);
	virtual ~ISSettingsForm();

protected:
	void CreateCentralPanel(); //Создание центральной панели
	void CreateSettings();
	void Save();
	void DefaultSettings(); //Установить настройки по умолчанию
	void Export();
	void Import();
	void Restart();
	QListWidgetItem* CreateItemGroup(ISMetaSettingsGroup *MetaGroup);
	void ItemSelectionChanged();
	void LabelRowClicked();
	void DataChanged();

private:
	QHBoxLayout *Layout;
	ISListWidget *ListWidget;
	QVBoxLayout *LayoutCentral;
	QLabel *LabelCurrentGroup;
	QLabel *LabelCurrentGroupHint;
	QTabWidget *TabWidget;
	ISButtonDialog *ButtonDialog;

	std::map<QListWidgetItem*, ISMetaSettingsGroup*> Groups;
	std::map<QString, ISFieldEditBase*> Fields;
};
//-----------------------------------------------------------------------------
