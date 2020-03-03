#include "ISDispatchListForm.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMessageBox.h"
#include "ISSmtpClient.h"
#include "ISMimeMessage.h"
#include "ISMimeHtml.h"
#include "ISQuery.h"
#include "ISProgressForm.h"
#include "ISQueryText.h"
//-----------------------------------------------------------------------------
static QString QS_DISPATCH_ORGANIZATIONS = PREPARE_QUERY2("SELECT dorg_id, orgz_id, orgz_name, dspc_email "
														 "FROM dispatchorganizations "
														 "LEFT JOIN dispatch ON dspc_id = :DispatchID "
														 "LEFT JOIN organizations ON dorg_organization = orgz_id "
														 "WHERE NOT dorg_isdeleted "
														 "AND dorg_dispatch = :DispatchID "
														 "ORDER BY dorg_id");
//-----------------------------------------------------------------------------
static QString QU_DISPATCH_ORGANIZATION = PREPARE_QUERY2("UPDATE dispatchorganizations SET "
														"dorg_sended = :Sended "
														"WHERE dorg_id = :DispatchOrganizationID");
//-----------------------------------------------------------------------------
ISDispatchListForm::ISDispatchListForm(QWidget *parent) : ISListBaseForm("Dispatch", parent)
{
	QAction *ActionSend = new QAction(this);
	ActionSend->setText(LANG("Dispatch"));
	ActionSend->setToolTip(LANG("Dispatch"));
	ActionSend->setIcon(BUFFER_ICONS("Chat.SendMessage"));
	connect(ActionSend, &QAction::triggered, this, &ISDispatchListForm::Send);
	AddAction(ActionSend);
}
//-----------------------------------------------------------------------------
ISDispatchListForm::~ISDispatchListForm()
{

}
//-----------------------------------------------------------------------------
void ISDispatchListForm::Send()
{
	QString Host = GetCurrentRecordValue("EMainServer").toString();
	int Port = GetCurrentRecordValue("Port").toInt();
	bool SSL = GetCurrentRecordValue("SSL").toBool();
	QString SenderLogin = GetCurrentRecordValue("Sender").toString(); //Отправитель
	QString SenderPassword = GetCurrentRecordValue("Password").toString(); //Пароль отправителя
	QString Subject = GetCurrentRecordValue("Subject").toString();
	QString Message = GetCurrentRecordValue("Text").toString();

	ISEmailAddress *Sender = new ISEmailAddress(SenderLogin); //Отправитель
	ISEmailAddress *Recipient = nullptr; //Получатель

	ISQuery qSelect(QS_DISPATCH_ORGANIZATIONS);
	qSelect.BindValue(":DispatchID", GetObjectID());
	if (qSelect.Execute())
	{
		int CountOrganizations = qSelect.GetCountResultRows();
		if (CountOrganizations)
		{
			ISProgressForm ProgressForm(0, CountOrganizations, this);
			ProgressForm.show();
				
			while (qSelect.Next()) //Обход организаций
			{
				int DispatchOrganizationID = qSelect.ReadColumn("dorg_id").toInt();
				int OrganizationID = qSelect.ReadColumn("orgz_id").toInt();
				QString OrganizationName = qSelect.ReadColumn("orgz_name").toString();
				QString EMailRecipient = "orgz_" + qSelect.ReadColumn("dspc_email").toString();

				ISQuery qSelectMail(QString("SELECT %1 FROM organizations WHERE orgz_id = :OrganizationID").arg(EMailRecipient));
				qSelectMail.BindValue(":OrganizationID", OrganizationID);
				if (qSelectMail.ExecuteFirst())
				{
					EMailRecipient = qSelectMail.ReadColumn(EMailRecipient).toString();
				}

				ProgressForm.SetText(LANG("Dispatching").arg(OrganizationName).arg(CountOrganizations));

				if (Recipient)
				{
					delete Recipient;
					Recipient = nullptr;
				}

				Recipient = new ISEmailAddress(EMailRecipient);
				ISSmtpClient SmtpClient(Host, Port, SSL ? ISSmtpClient::SslConnection : ISSmtpClient::TcpConnection);

				ISMimeMessage MimeMessage;
				MimeMessage.setSender(Sender);
				MimeMessage.setSubject(Subject);
				MimeMessage.addRecipient(Recipient);

				ISMimeHtml MimeHtml;
				MimeHtml.setHtml(Message);
				MimeMessage.addPart(&MimeHtml);

				if (SmtpClient.connectToHost()) //Если соединение с сервером прошло успешно
				{
					if (SmtpClient.login(SenderLogin, SenderPassword)) //Если авторизация прошла успешно
					{
						ISQuery qUpdate(QU_DISPATCH_ORGANIZATION);
						qUpdate.BindValue(":DispatchOrganizationID", DispatchOrganizationID);

						if (SmtpClient.sendMail(MimeMessage)) //Если сообщение отправлено
						{
							qUpdate.BindValue(":Sended", true);
						}
						else //Сообщение не отправлено
						{
							qUpdate.BindValue(":Sended", true);
							ISMessageBox::ShowWarning(this, LANG("Message.Warning.ErrorSendingMessageDispatch"));
						}

						qUpdate.Execute();
					}
					else
					{
						ISMessageBox::ShowWarning(this, LANG("Message.Warning.ErrorLoginAndPasswordDispatch"));
					}
				}
				else
				{
					ISMessageBox::ShowWarning(this, LANG("Message.Warning.ErrorConnectionHostDispath"));
				}

				SmtpClient.quit();
				--CountOrganizations;
			}
		}
	}

	delete Sender;
}
//-----------------------------------------------------------------------------
