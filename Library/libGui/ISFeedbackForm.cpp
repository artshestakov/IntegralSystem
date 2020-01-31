#include "StdAfx.h"
#include "ISFeedbackForm.h"
#include "ISDefines.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISControls.h"
#include "ISQuery.h"
#include "ISSmtpClient.h"
#include "ISMimeHtml.h"
#include "ISMimeAttachment.h"
#include "ISMessageBox.h"
#include "ISSystem.h"
#include "ISMetaUser.h"
#include "ISVersion.h"
//-----------------------------------------------------------------------------
static QString QS_FEEDBACK_TYPE = PREPARE_QUERY("SELECT fbtp_name, fbtp_icon "
												"FROM _feedbacktype "
												"WHERE NOT fbtp_isdeleted "
												"ORDER BY fbtp_order");
//-----------------------------------------------------------------------------
ISFeedbackForm::ISFeedbackForm(QWidget *parent) : ISInterfaceForm(parent)
{
	setWindowTitle(LANG("Feedback"));
	setWindowIcon(BUFFER_ICONS("FeedbackForm"));

	GetMainLayout()->setContentsMargins(LAYOUT_MARGINS_10_PX);

	QLabel *LabelTitle = new QLabel(this);
	LabelTitle->setFont(FONT_TAHOMA_15);
	LabelTitle->setText(LANG("Feedback"));
	GetMainLayout()->addWidget(LabelTitle);

	QLabel *LabelInfo = new QLabel(this);
	LabelInfo->setText(LANG("FeedbackFormInfo"));
	GetMainLayout()->addWidget(LabelInfo);

	GetMainLayout()->addWidget(ISControls::CreateHorizontalLine(this));

	CreateTypePanel();

	QFormLayout *FormLayout = new QFormLayout();
	GetMainLayout()->addLayout(FormLayout);

	EditName = new ISLineEdit(this);
	FormLayout->addRow(LANG("Feedback.YourName") + ":", EditName);

	EditPhone = new ISPhoneBaseEdit(this);
	EditPhone->SetVisibleCall(false);
	FormLayout->addRow(LANG("Feedback.YourPhone") + ":", EditPhone);

	EditMail = new ISEMailEdit(this);
	FormLayout->addRow(LANG("Feedback.YourEMail") + ":", EditMail);

	EditTitle = new ISLineEdit(this);
	EditTitle->SetPlaceholderText(LANG("Feedback.Title.PlaceholderText"));
	FormLayout->addRow(LANG("Feedback.Title") + ":", EditTitle);

	EditDescription = new ISTextEdit(this);
	EditDescription->SetPlaceholderText(LANG("Feedback.ReviewText"));
	FormLayout->addRow(LANG("Feedback.Review") + ":", EditDescription);

	EditPathFile = new ISPathEditFile(this);
	FormLayout->addRow(LANG("FilePath") + ":", EditPathFile);

	ButtonDialog = new ISButtonDialog(this, LANG("Send"));
	ButtonDialog->SetApplyEnabled(false);
	connect(ButtonDialog, &ISButtonDialog::Apply, this, &ISFeedbackForm::SendReview);
	connect(ButtonDialog, &ISButtonDialog::Close, this, &ISFeedbackForm::close);
	GetMainLayout()->addWidget(ButtonDialog);
}
//-----------------------------------------------------------------------------
ISFeedbackForm::~ISFeedbackForm()
{

}
//-----------------------------------------------------------------------------
void ISFeedbackForm::EscapeClicked()
{
	close();
}
//-----------------------------------------------------------------------------
void ISFeedbackForm::CreateTypePanel()
{
	QHBoxLayout *Layout = new QHBoxLayout();
	GetMainLayout()->addLayout(Layout);

	ButtonGroup = new QButtonGroup(this);
	connect(ButtonGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), [=](int)
	{
		ButtonDialog->SetApplyEnabled(true);
	});

	ISQuery qSelect(QS_FEEDBACK_TYPE);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			QString Name = qSelect.ReadColumn("fbtp_name").toString();
			QString Icon = qSelect.ReadColumn("fbtp_icon").toString();

			ISPushButton *ButtonType = new ISPushButton(this);
			ButtonType->setCheckable(true);
			ButtonType->setText(Name);
			ButtonType->setIcon(BUFFER_ICONS(Icon));
			ButtonType->setCursor(CURSOR_POINTING_HAND);
			Layout->addWidget(ButtonType);

			ButtonGroup->addButton(ButtonType);
		}
	}
}
//-----------------------------------------------------------------------------
bool ISFeedbackForm::CheckReview()
{
	if (!EditName->GetValue().toString().length())
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Error.Field.NullValue").arg(LANG("Feedback.YourName")));
		EditName->BlinkRed();
		return false;
	}

	if (!EditPhone->IsValid())
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.ValueFieldEditInvalid").arg(LANG("Feedback.YourPhone")));
		EditPhone->BlinkRed();
		return false;
	}

	if (!EditMail->IsValid())
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.ValueFieldEditInvalid").arg(LANG("Feedback.YourEMail")));
		EditMail->BlinkRed();
		return false;
	}

	return true;
}
//-----------------------------------------------------------------------------
void ISFeedbackForm::SendReview()
{
	if (CheckReview())
	{
		if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.FeedbackSend")))
		{
			ISSmtpClient SmtpClient(FEEDBACK_SERVER, FEEDBACK_PORT, ISSmtpClient::SslConnection);

			ISEmailAddress EMailSender(FEEDBACK_SENDER, LANG("Feedback.Name"));
			ISEmailAddress EMailRecipient(FEEDBACK_RECIPIENT);

			ISMimeMessage MimeMessage;
			MimeMessage.setSender(&EMailSender);
			MimeMessage.setSubject(EditTitle->GetValue().toString()); //Тема сообщения
			MimeMessage.addRecipient(&EMailRecipient);

			ISMimeHtml MimeHtml;
			MimeHtml.setHtml(CreateMessage());
			MimeMessage.addPart(&MimeHtml);

			QString FilePath = EditPathFile->GetValue().toString();
			if (FilePath.length())
			{
				QFile FileAttachment(FilePath);
				if (FileAttachment.exists())
				{
					ISMimeAttachment MimeAttachment(&FileAttachment);
					MimeMessage.addPart(&MimeAttachment);
				}
				else
				{
					ISMessageBox::ShowWarning(this, LANG("Message.Error.NotFoundFile").arg(FilePath));
					EditPathFile->BlinkRed();
					return;
				}
			}

			if (!SmtpClient.connectToHost())
			{
				ISMessageBox::ShowWarning(this, LANG("Message.Warning.Feedback.ConnectToHost"));
				return;
			}

			if (!SmtpClient.login(FEEDBACK_SENDER, FEEDBACK_SENDER_PASSWORD))
			{
				ISMessageBox::ShowWarning(this, LANG("Message.Warning.Feedback.Authentification"));
				return;
			}

			if (!SmtpClient.sendMail(MimeMessage))
			{
				ISMessageBox::ShowWarning(this, LANG("Message.Warning.Feedback.Send"));
				return;
			}

			SmtpClient.quit();

			ISMessageBox::ShowInformation(this, LANG("Message.Information.FeedbackSended"));
			close();
		}
	}
}
//-----------------------------------------------------------------------------
QString ISFeedbackForm::CreateMessage() const
{
	QString Message;

	Message += QString(LANG("Feedback.Type") + ": " + ButtonGroup->checkedButton()->text() + "\n");
	Message += QString(LANG("Feedback.SpecifiedName") + ": " + EditName->GetValue().toString() + "\n");
	Message += QString(LANG("Feedback.SpecifiedPhone") + ": " + EditPhone->GetValue().toString() + "\n");
	Message += QString(LANG("Feedback.SpecifiedMail") + ": " + EditMail->GetValue().toString() + "\n");

	if (EditTitle->GetValue().toString().length())
	{
		Message += QString(LANG("Feedback.Title") + ": " + EditTitle->GetValue().toString() + "\n");
	}

	if (EditDescription->GetValue().toString().length())
	{
		Message += QString(LANG("Feedback.Review") + ": " + EditDescription->GetValue().toString() + "\n");
	}

	if (EditPathFile->GetValue().toString().length())
	{
		Message += QString(LANG("FilePath") + ": " + EditPathFile->GetValue().toString() + "\n");
	}

	Message += QString(LANG("Feedback.Version") + ": " + ISVersion::GetInstance().GetVersion() + "\n");
	Message += QString(LANG("Feedback.VersionDate") + ": " + ISVersion::GetInstance().GetDate().toString(DATE_FORMAT_V2) + "\n");
	Message += QString(LANG("Feedback.ApplicationDirPath") + ": " + APPLICATION_DIR_PATH + "\n");
	Message += QString(LANG("Feedback.User") + ": " + ISMetaUser::GetInstance().GetData()->FullName + "\n");
	Message += QString(LANG("Feedback.Login") + ": " + ISMetaUser::GetInstance().GetData()->Login + "\n");

	ISSystem::RemoveLastSymbolFromString(Message);
	return Message;
}
//-----------------------------------------------------------------------------
