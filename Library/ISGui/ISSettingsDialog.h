#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
#include "ISStructs.h"
#include "ISFieldEditBase.h"
#include "ISListWidget.h"
//-----------------------------------------------------------------------------
class ISSettingsDialog : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISSettingsDialog(const QString &SettingGroupUID);
	virtual ~ISSettingsDialog();

protected:
	void CreateCentralPanel(); //Создание центральной панели
	void CreateSettings();
	void Save();
	void DefaultSettings(); //Установить настройки по умолчанию
	QListWidgetItem* CreateItemGroup(ISMetaSettingsGroup *MetaGroup);
	void ItemSelectionChanged();
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
	std::map<ISUuid, ISFieldEditBase*> Fields;
};
//-----------------------------------------------------------------------------
