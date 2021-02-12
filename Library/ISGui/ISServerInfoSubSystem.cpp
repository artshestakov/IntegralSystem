#include "ISServerInfoSubSystem.h"
#include "ISTcpQuery.h"
#include "ISConstants.h"
#include "ISDialogsCommon.h"
#include "ISLocalization.h"
#include "ISDefinesGui.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISServerInfoSubSystem::ISServerInfoSubSystem(QWidget *parent) : ISInterfaceMetaForm(parent)
{
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);
}
//-----------------------------------------------------------------------------
ISServerInfoSubSystem::~ISServerInfoSubSystem()
{

}
//-----------------------------------------------------------------------------
void ISServerInfoSubSystem::LoadData()
{
	ISTcpQuery qGetServerInfo(API_GET_SERVER_INFO);

	ISGui::SetWaitGlobalCursor(true);
	bool Result = qGetServerInfo.Execute();
	ISGui::SetWaitGlobalCursor(false);
	if (!Result)
	{
		ISMessageBox::ShowWarning(this, qGetServerInfo.GetErrorString());
		return;
	}

	QVariantMap AnswerMap = qGetServerInfo.GetAnswer();
	QVariantMap CaratMap = AnswerMap["Carat"].toMap(),
		DatabaseMap = AnswerMap["Database"].toMap();

	QGroupBox *GroupBoxCarat = new QGroupBox(LANG("ISServerInfoSubSystem.Carat.Title"), this);
	GetMainLayout()->addWidget(GroupBoxCarat);

	QFormLayout *FormLayoutCarat = new QFormLayout();
	FormLayoutCarat->addRow(LANG("ISServerInfoSubSystem.Carat.Version"), new QLabel(CaratMap["Version"].toString(), this));
	FormLayoutCarat->addRow(LANG("ISServerInfoSubSystem.Carat.StartDateTime"), new QLabel(CaratMap["StartedDateTime"].toString(), this));
	FormLayoutCarat->addRow(LANG("ISServerInfoSubSystem.Carat.Uptime"), new QLabel(CaratMap["Uptime"].toString(), this));
	FormLayoutCarat->addRow(LANG("ISServerInfoSubSystem.Carat.SizeLogs"), new QLabel(CaratMap["SizeLogs"].toString(), this));
	FormLayoutCarat->addRow(LANG("ISServerInfoSubSystem.Carat.CountClients"), new QLabel(CaratMap["CountClients"].toString(), this));
	GroupBoxCarat->setLayout(FormLayoutCarat);

	QGroupBox *GroupBoxDatabase = new QGroupBox(LANG("ISServerInfoSubSystem.Database.Title"), this);
	GetMainLayout()->addWidget(GroupBoxDatabase);

	QFormLayout *FormLayoutDatabase = new QFormLayout();
	FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.SizeFull"), new QLabel(DatabaseMap["SizeFull"].toString(), this));
	FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.SizeIndexes"), new QLabel(DatabaseMap["SizeIndexes"].toString(), this));
	FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.SizeData"), new QLabel(DatabaseMap["SizeData"].toString(), this));
	FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.Owner"), new QLabel(DatabaseMap["Owner"].toString(), this));
	FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.Encoding"), new QLabel(DatabaseMap["Encoding"].toString(), this));
	FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.Uptime"), new QLabel(DatabaseMap["Uptime"].toString(), this));
	FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.PID"), new QLabel(DatabaseMap["PID"].toString(), this));
	FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.Version"), new QLabel(DatabaseMap["Version"].toString(), this));
	FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.ClusterPath"), new QLabel(DatabaseMap["ClusterPath"].toString(), this));
	FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.SizeLogs"), new QLabel(DatabaseMap["SizeLogs"].toString(), this));
	FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.SizeXLogs"), new QLabel(DatabaseMap["SizeXLogs"].toString(), this));
	FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.CountTable"), new QLabel(DatabaseMap["CountTable"].toString(), this));
	FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.CountField"), new QLabel(DatabaseMap["CountField"].toString(), this));
	FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.CountSequence"), new QLabel(DatabaseMap["CountSequence"].toString(), this));
	FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.CountForeign"), new QLabel(DatabaseMap["CountForeign"].toString(), this));
	FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.RowsCount"), new QLabel(DatabaseMap["RowsCount"].toString(), this));
	FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.ProtocolCount"), new QLabel(DatabaseMap["ProtocolCount"].toString(), this));
	FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.MonitorCount"), new QLabel(DatabaseMap["MonitorCount"].toString(), this));
	FormLayoutDatabase->addRow(LANG("ISServerInfoSubSystem.Database.UsersCount"), new QLabel(DatabaseMap["UsersCount"].toString(), this));
	GroupBoxDatabase->setLayout(FormLayoutDatabase);
}
//-----------------------------------------------------------------------------
