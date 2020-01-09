#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISInterfaceDialogForm.h"
#include "ISMetaSettingsGroup.h"
#include "ISFieldEditBase.h"
#include "ISButtonDialog.h"
#include "ISScrollArea.h"
#include "ISListWidget.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISSettingsForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISSettingsForm(const QString &SettingGroupUID, QWidget *parent = 0);
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

	QMap<QListWidgetItem*, ISMetaSettingsGroup*> Groups;
	QMap<QString, ISFieldEditBase*> Fields;
};
//-----------------------------------------------------------------------------
