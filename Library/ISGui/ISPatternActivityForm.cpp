#include "ISPatternActivityForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISStyleSheet.h"
#include "ISQuery.h"
#include "ISDatabase.h"
#include "ISMetaUser.h"
#include "ISFlowLayout.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
static QString QS_PATTERN = PREPARE_QUERY("SELECT aspt_uid, aspt_pattern "
										  "FROM _asteriskpattern "
										  "WHERE NOT aspt_isdeleted "
										  "ORDER BY aspt_pattern");
//-----------------------------------------------------------------------------
static QString QS_PATTERN_STATUS = PREPARE_QUERY("SELECT aspt_uid, aspt_pattern, aest_icon, aest_backgroundcolor "
												 "FROM _asteriskpattern "
												 "LEFT JOIN _asteriskextenstatus ON aest_id = aspt_currentstatus "
												 "WHERE NOT aspt_isdeleted "
												 "AND aspt_currentstatus IS NOT NULL");
//-----------------------------------------------------------------------------
ISPatternActivityForm::ISPatternActivityForm(QWidget *parent)
	: ISInterfaceForm(parent),
	FutureWatcher(new QFutureWatcher<void>(this))
{
	connect(this, &ISPatternActivityForm::PatternData, this, &ISPatternActivityForm::SetPatternData);

	LayoutCentral = new QVBoxLayout();
	GetMainLayout()->addLayout(LayoutCentral);

	CreateMainPanel();
	CreateSignPanel();

	Timer = new QTimer(this);
	Timer->setInterval(1000);
	connect(Timer, &QTimer::timeout, this, &ISPatternActivityForm::Timeout);

	Database = QSqlDatabase::database("PatternActivity");
	if (!Database.isValid())
	{
		//???
		//Database = QSqlDatabase::cloneDatabase(ISDatabase::Instance().GetDefaultDB(), CONNECTON_PATTERN_ACITIVITY);
	}
}
//-----------------------------------------------------------------------------
ISPatternActivityForm::~ISPatternActivityForm()
{
	
}
//-----------------------------------------------------------------------------
void ISPatternActivityForm::CreateMainPanel()
{
	QWidget *Widget = new QWidget(this);
	Widget->setLayout(new ISFlowLayout());
	LayoutCentral->addWidget(Widget);

	ISQuery qSelect(QS_PATTERN);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			ISPatternWidget *PatternWidget = new ISPatternWidget(Widget);
			PatternWidget->SetPattern(qSelect.ReadColumn("aspt_pattern").toString());
			Widget->layout()->addWidget(PatternWidget);

			Map.insert(qSelect.ReadColumn("aspt_uid"), PatternWidget);
		}
	}

	if (!Map.count())
	{
		QLabel *Label = new QLabel(Widget);
		Label->setText(LANG("TelephonyForm.Activity.EmptyPatterns"));
		Label->setFont(ISDefines::Gui::FONT_TAHOMA_12);
		Widget->layout()->addWidget(Label);
	}
}
//-----------------------------------------------------------------------------
void ISPatternActivityForm::CreateSignPanel()
{
	QHBoxLayout *LayoutGroupBox = new QHBoxLayout();

	QGroupBox *GroupBox = new QGroupBox(this);
	GroupBox->setTitle(LANG("TelephonyForm.Activity.Sign"));
	GroupBox->setLayout(LayoutGroupBox);
	GroupBox->setSizePolicy(GroupBox->sizePolicy().horizontalPolicy(), QSizePolicy::Maximum);
	GetMainLayout()->addWidget(GroupBox);

	QWidget *WidgetFree = new QWidget(GroupBox);
	WidgetFree->setFixedSize(ISDefines::Gui::SIZE_22_22);
	WidgetFree->setStyleSheet(STYLE_SHEET("Pattern.Color.Free"));
	LayoutGroupBox->addWidget(WidgetFree);

	QLabel *LabelFree = new QLabel(GroupBox);
	LabelFree->setText("- " + LANG("TelephonyForm.Activity.Sign.Free"));
	LabelFree->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
	LayoutGroupBox->addWidget(LabelFree);

	QWidget *WidgetBusy = new QWidget(GroupBox);
	WidgetBusy->setFixedSize(ISDefines::Gui::SIZE_22_22);
	WidgetBusy->setStyleSheet(STYLE_SHEET("Pattern.Color.Busy"));
	LayoutGroupBox->addWidget(WidgetBusy);

	QLabel *LabelBusy = new QLabel(GroupBox);
	LabelBusy->setText("- " + LANG("TelephonyForm.Activity.Sign.Busy"));
	LabelBusy->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
	LayoutGroupBox->addWidget(LabelBusy);

	QWidget *WidgetNotRegistered = new QWidget(GroupBox);
	WidgetNotRegistered->setFixedSize(ISDefines::Gui::SIZE_22_22);
	WidgetNotRegistered->setStyleSheet(STYLE_SHEET("Pattern.Color.NotRegistered"));
	LayoutGroupBox->addWidget(WidgetNotRegistered);

	QLabel *LabelNotRegistered = new QLabel(GroupBox);
	LabelNotRegistered->setText("- " + LANG("TelephonyForm.Activity.Sign.NotRegistered"));
	LabelNotRegistered->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
	LayoutGroupBox->addWidget(LabelNotRegistered);

	LayoutGroupBox->addStretch();
}
//-----------------------------------------------------------------------------
void ISPatternActivityForm::Timeout()
{
	if (!FutureWatcher->isRunning())
	{
		QFuture<void> Future = QtConcurrent::run(this, &ISPatternActivityForm::CheckStatus);
		FutureWatcher->setFuture(Future);
	}
}
//-----------------------------------------------------------------------------
void ISPatternActivityForm::CheckStatus()
{
	if (Database.open(ISMetaUser::Instance().UserData->Login, ISMetaUser::Instance().UserData->Password))
	{
		{
			ISQuery qSelect(Database, QS_PATTERN_STATUS);
			if (qSelect.Execute())
			{
				while (qSelect.Next())
				{
					ISUuid UID = qSelect.ReadColumn("aspt_uid");
					QString Pattern = qSelect.ReadColumn("aspt_pattern").toString();
					QString IconName = qSelect.ReadColumn("aest_icon").toString();
					QString BackgroundColor = qSelect.ReadColumn("aest_backgroundcolor").toString();

					emit PatternData(UID, IconName, Pattern, BackgroundColor);
				}
			}
		}

		//???
		//ISDatabase::Instance().DisconnectFromDatabase(Database);
	}
	else
	{
		qDebug() << Database.lastError().text();
	}
}
//-----------------------------------------------------------------------------
void ISPatternActivityForm::SetPatternData(const ISUuid &UID, const QString &IconName, const QString &Pattern, const QString &BackgroundColor)
{
	ISPatternWidget *PatternWidget = Map.value(UID);
	PatternWidget->SetIcon(BUFFER_ICONS(IconName));
	PatternWidget->SetPattern(Pattern);
	PatternWidget->SetBackgroundColor(BackgroundColor);
}
//-----------------------------------------------------------------------------