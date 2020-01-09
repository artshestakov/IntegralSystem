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
	void SystemClicked(); //������� �� �������� �������
	void SubSystemClicked(); //������� �� ����������

	void LoadSubSystemsBar(ISMetaSystem *MetaSystem); //���������� ���������
	void ClearSubSystemsBar(); //������� ���� ���������

	QString GetSystemToolTip(ISMetaSystem *MetaSystem) const; //�������� ����������� ��������� ��� �������
	QString GetSubSystemToolTip(ISMetaSubSystem *MetaSubSystem) const; //�������� ����������� ��������� ��� ��� ����������

	QIcon GetSystemIcon(ISMetaSystem *MetaSystem) const; //�������� ������ ��� �������
	QIcon GetSubSystemIcon(ISMetaSubSystem *MetaSystem) const; //�������� ������ ��� �������

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
