#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISMetaSystem.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISSystemsPanel : public QWidget
{
	Q_OBJECT

signals:
	void ClickedSubSystem(const QString &SubSystemUID, const QIcon &IconSubSystem);

public:
	ISSystemsPanel(QWidget *parent = 0);
	virtual ~ISSystemsPanel();

	void AddSystem(ISMetaSystem *MetaSystem);

protected:
	void SystemClicked(); //Нажатие на действие системы
	void SubSystemClicked(); //Нажатие на подсистему

	void LoadSubSystemsBar(ISMetaSystem *MetaSystem); //Заполнение подсистем
	void ClearSubSystemsBar(); //Очистка бара подсистем

	QString GetSystemToolTip(ISMetaSystem *MetaSystem) const; //Получить всплывающую подсказку для системы
	QString GetSubSystemToolTip(ISMetaSubSystem *MetaSubSystem) const; //Получить всплывающую подсказку для для подсистемы

	QIcon GetSystemIcon(ISMetaSystem *MetaSystem) const; //Получить иконку для системы
	QIcon GetSubSystemIcon(ISMetaSubSystem *MetaSystem) const; //Получить иконку для системы

private:
	QVBoxLayout *Layout;

	QToolBar *SystemsBar;
	QFrame *LineSystems;

	QToolBar *SubSystemBar;
	QFrame *LineSubSystems;

	QActionGroup *ActionSystemGroup;
	QActionGroup *ActionSubSystemGroup;
	QMap<QAction*, ISMetaSystem*> Systems;

	QString CurrentSystemUID;
	QString CurrentSubSystemUID;
};
//-----------------------------------------------------------------------------
