#include "ISTelephonyForm.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
#include "ISAsteriskCallsListForm.h"
#include "ISAsteriskTotalForm.h"
#include "ISPatternActivityForm.h"
//-----------------------------------------------------------------------------
ISTelephonyForm::ISTelephonyForm(QWidget *parent) : ISParagraphBaseForm(parent)
{
	MainLayout = new QHBoxLayout();
	setLayout(MainLayout);

	ListWidget = new ISListWidget(this);
	ListWidget->setSizePolicy(QSizePolicy::Maximum, ListWidget->sizePolicy().verticalPolicy());
	connect(ListWidget, &QListWidget::itemSelectionChanged, this, &ISTelephonyForm::ItemSelectionChanged);
	MainLayout->addWidget(ListWidget);

	CreateItem(BUFFER_ICONS("TelephonyForm.Calls"), LANG("TelephonyForm.Journal"), ISNamespace::TF_Journal);
	CreateItem(BUFFER_ICONS("TelephonyForm.Summary"), LANG("TelephonyForm.Summary"), ISNamespace::TF_Summary);
	CreateItem(BUFFER_ICONS("TelephonyForm.Pattern"), LANG("TelephonyForm.Patterns"), ISNamespace::TF_Pattern);
	CreateItem(BUFFER_ICONS("TelephonyForm.Activity"), LANG("TelephonyForm.Activity"), ISNamespace::TF_Activity);

	QHBoxLayout *LayoutCentral = new QHBoxLayout();
	LayoutCentral->addStretch();

	CentralWidget = new QWidget(this);
	CentralWidget->setLayout(LayoutCentral);
	MainLayout->addWidget(CentralWidget);
}
//-----------------------------------------------------------------------------
ISTelephonyForm::~ISTelephonyForm()
{

}
//-----------------------------------------------------------------------------
void ISTelephonyForm::Invoke()
{

}
//-----------------------------------------------------------------------------
void ISTelephonyForm::CreateItem(const QIcon &Icon, const QString &Text, ISNamespace::TelephonyForm FormType)
{
	QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);
	ListWidgetItem->setText(Text);
	ListWidgetItem->setIcon(Icon);
	ListWidgetItem->setData(Qt::UserRole, FormType);
	ListWidgetItem->setSizeHint(QSize(ListWidgetItem->sizeHint().width(), 30));
}
//-----------------------------------------------------------------------------
void ISTelephonyForm::ItemSelectionChanged()
{
	POINTER_DELETE(CentralWidget);
	ISNamespace::TelephonyForm FormType = qvariant_cast<ISNamespace::TelephonyForm>(ListWidget->currentItem()->data(Qt::UserRole));
	if (FormType == ISNamespace::TF_Journal) //Журнал звонков
	{
		ISAsteriskCallsListForm *AsteriskCallsListForm = new ISAsteriskCallsListForm(this);
		AsteriskCallsListForm->LoadData();
		CentralWidget = AsteriskCallsListForm;
	}
	else if (FormType == ISNamespace::TF_Summary) //Сводная статистика
	{
		CentralWidget = new ISAsteriskTotalForm(this);
	}
	else if (FormType == ISNamespace::TF_Pattern) //Внутренние номера
	{
		ISListBaseForm *ListBaseForm = new ISListBaseForm("_AsteriskPattern", this);
		ListBaseForm->LoadData();
		connect(ListBaseForm, &ISListBaseForm::AddFormFromTab, [=](QWidget *ObjectFormBase)
		{
			ObjectFormBase->setParent(nullptr);
			ObjectFormBase->adjustSize();
			ISGui::MoveWidgetToDesktop(ObjectFormBase, ISNamespace::MWD_Center);
			ObjectFormBase->show();
		});
		CentralWidget = ListBaseForm;
	}
	else if (FormType == ISNamespace::TF_Activity)
	{
		CentralWidget = new ISPatternActivityForm(this);
	}
	MainLayout->addWidget(CentralWidget);
}
//-----------------------------------------------------------------------------
