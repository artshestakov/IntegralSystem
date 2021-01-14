#include "ISFullTextSearchSubSystem.h"
#include "ISDefinesGui.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMetaData.h"
#include "ISTcpQuery.h"
#include "ISLabels.h"
#include "ISGui.h"
#include "ISControls.h"
#include "ISMessageBox.h"
//-----------------------------------------------------------------------------
ISFullTextSearchSubSystem::ISFullTextSearchSubSystem(QWidget *parent) : ISInterfaceMetaForm(parent)
{
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	QHBoxLayout *LayoutTitle = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutTitle);

	EditSearch = new ISSearchEdit(this);
	EditSearch->SetFont(ISDefines::Gui::FONT_TAHOMA_10);
	LayoutTitle->addWidget(EditSearch);

	WaitWidget = new ISWaitWidget(this, false, false);
	WaitWidget->SetRoundness(70);
	WaitWidget->SetMinimumTrailOpacity(15);
	WaitWidget->SetTrailFadePercentage(70);
	WaitWidget->SetNumberOfLines(9);
	WaitWidget->SetLineLength(7);
	WaitWidget->SetLineWidth(2);
	WaitWidget->SetInnerRadius(4);
	WaitWidget->SetRevolutionsPerSecond(2);
	LayoutTitle->addWidget(WaitWidget);

	ButtonSearch = new ISPushButton(BUFFER_ICONS("Search"), LANG("Search"), this);
	ButtonSearch->setCursor(CURSOR_POINTING_HAND);
	connect(ButtonSearch, &ISPushButton::clicked, this, &ISFullTextSearchSubSystem::Search);
	LayoutTitle->addWidget(ButtonSearch);

	QHBoxLayout *LayoutLabels = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutLabels);

	LayoutLabels->addStretch();

	LabelResult = new QLabel(this);
	LayoutLabels->addWidget(LabelResult);

	Frame = ISControls::CreateHorizontalLine(this);
	Frame->setVisible(false);
	GetMainLayout()->addWidget(Frame);

	ScrollArea = new ISScrollArea(this);
	GetMainLayout()->addWidget(ScrollArea);
}
//-----------------------------------------------------------------------------
ISFullTextSearchSubSystem::~ISFullTextSearchSubSystem()
{

}
//-----------------------------------------------------------------------------
void ISFullTextSearchSubSystem::LoadData()
{
	EditSearch->SetFocus();
}
//-----------------------------------------------------------------------------
void ISFullTextSearchSubSystem::SetSearchInProgress(bool InProgress)
{
	ISGui::SetWaitGlobalCursor(InProgress);
	EditSearch->setEnabled(!InProgress);
	ButtonSearch->setEnabled(!InProgress);
	InProgress ? WaitWidget->Start() : WaitWidget->Stop();

	if (InProgress)
	{
		while (!WidgetList.isEmpty())
		{
			delete WidgetList.takeFirst();
		}
		LabelResult->clear();
	}
	else
	{
		EditSearch->SetFocus();
		EditSearch->SelectAll();
		Frame->setVisible(true);
	}
}
//-----------------------------------------------------------------------------
void ISFullTextSearchSubSystem::Search()
{
	if (EditSearch->GetValue().toString().isEmpty())
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.SearchFieldNotValid"));
		EditSearch->BlinkRed();
		return;
	}
	SetSearchInProgress(true);
	
	ISTcpQuery qSearchFullText(API_SEARCH_FULL_TEXT);
	qSearchFullText.BindValue("Value", EditSearch->GetValue());
	if (!qSearchFullText.Execute())
	{
		SetSearchInProgress(false);
		ISMessageBox::ShowCritical(this, qSearchFullText.GetErrorString());
		return;
	}

	if (ScrollArea->widget()->layout())
	{
		delete ScrollArea->widget()->layout();
	}
	ScrollArea->widget()->setLayout(new QVBoxLayout());

	QVariantList ResultList = qSearchFullText.TakeAnswer()["Result"].toList();
	if (!ResultList.isEmpty()) //Поиск дал результаты
	{
		int Index = 0;
		for (const QVariant &Variant : ResultList) //Обходим результаты поиска
		{
			QVariantMap ResultMap = Variant.toMap();
			PMetaTable *MetaTable = ISMetaData::Instance().GetMetaTable(ResultMap["TableName"].toString());

			ISQLabel *LabelLink = new ISQLabel(QString("%1. %2: %3").arg(++Index).arg(MetaTable->LocalName).arg(ResultMap["ObjectName"].toString()), ScrollArea);
			LabelLink->SetIsLinked(true);
			LabelLink->setSizePolicy(QSizePolicy::Maximum, LabelLink->sizePolicy().verticalPolicy());
			LabelLink->setProperty("TableName", MetaTable->Name);
			LabelLink->setProperty("ObjectID", ResultMap["ID"]);
			connect(LabelLink, &ISQLabel::Clicked, this, &ISFullTextSearchSubSystem::ClickedRecord);
			dynamic_cast<QVBoxLayout*>(ScrollArea->widget()->layout())->addWidget(LabelLink);
			WidgetList.append(LabelLink);
		}
	}
	else //Поиск не дал результатов
	{
		QLabel *LabelNotFound = new QLabel(LANG("FullSearchTextNotResult"), ScrollArea);
		ISGui::SetFontWidgetBold(LabelNotFound, true);
		dynamic_cast<QVBoxLayout*>(ScrollArea->widget()->layout())->addWidget(LabelNotFound);
		WidgetList.append(LabelNotFound);
	}
	dynamic_cast<QVBoxLayout*>(ScrollArea->widget()->layout())->addStretch();
	LabelResult->setText(LANG("Search.ResultCount") + ": " + QString::number(ResultList.size()));
	SetSearchInProgress(false);
}
//-----------------------------------------------------------------------------
void ISFullTextSearchSubSystem::ClickedRecord()
{
	ISGui::ShowObjectForm(ISGui::CreateObjectForm(ISNamespace::OFT_Edit, sender()->property("TableName").toString(), sender()->property("ObjectID").toInt()));
}
//-----------------------------------------------------------------------------
