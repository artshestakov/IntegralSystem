#include "ISSearchForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISGui.h"
#include "ISMetaData.h"
#include "ISComboSearchWidgets.h"
//-----------------------------------------------------------------------------
ISSearchForm::ISSearchForm(PMetaTable *meta_table, QWidget *parent)
	: ISInterfaceForm(parent),
	MetaTable(meta_table)
{
	setWindowTitle(LANG("ISSearchForm.Title") + " - " + MetaTable->LocalListName);
	setWindowIcon(BUFFER_ICONS("Search"));
	resize(800, 600);
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	LayoutScroll = new QVBoxLayout();
	GetMainLayout()->addLayout(LayoutScroll);

	ScrollArea = new ISScrollArea(this);
	ScrollArea->widget()->setLayout(LayoutScroll);
	GetMainLayout()->addWidget(ScrollArea);

	for (PMetaField *MetaField : MetaTable->Fields)
	{
		//Пропускаем системные поля и поля по которым поиск зарещён
		if (MetaField->IsSystem || MetaField->NotSearch)
		{
			continue;
		}
		AddField(MetaField);
	}

	QHBoxLayout *LayoutBottom = new QHBoxLayout();
	LayoutBottom->addStretch();
	GetMainLayout()->addLayout(LayoutBottom);

	ButtonSearch = new ISPushButton(this);
	ButtonSearch->setEnabled(false);
	ButtonSearch->setText(LANG("Search"));
	ButtonSearch->setIcon(BUFFER_ICONS("Search"));
	connect(ButtonSearch, &ISPushButton::clicked, this, &ISSearchForm::Search);
	LayoutBottom->addWidget(ButtonSearch);

	ButtonHide = new ISPushButton(this);
	ButtonHide->setText(LANG("Search.HideForm"));
	connect(ButtonHide, &ISPushButton::clicked, this, &ISSearchForm::hide);
	LayoutBottom->addWidget(ButtonHide);
}
//-----------------------------------------------------------------------------
ISSearchForm::~ISSearchForm()
{

}
//-----------------------------------------------------------------------------
void ISSearchForm::closeEvent(QCloseEvent *e)
{
	e->ignore();
	hide();
}
//-----------------------------------------------------------------------------
void ISSearchForm::EscapeClicked()
{
	hide();
}
//-----------------------------------------------------------------------------
void ISSearchForm::EnterClicked()
{
	
}
//-----------------------------------------------------------------------------
void ISSearchForm::AfterShowEvent()
{
	int MaxSize = 0;
	for (QLabel *Label : VectorLabels)
	{
		if (Label->width() > MaxSize)
		{
			MaxSize = Label->width();
		}
	}

	for (QLabel *Label : VectorLabels)
	{
		Label->setFixedWidth(MaxSize);
	}
}
//-----------------------------------------------------------------------------
void ISSearchForm::AddField(PMetaField *MetaField, int Index)
{
	QHBoxLayout *LayoutRow = new QHBoxLayout();

	//Заголовок поискового поля
	QLabel *LabelName = new QLabel(MetaField->LabelName + ':', ScrollArea);
	LabelName->setSizePolicy(QSizePolicy::Maximum, LabelName->sizePolicy().verticalPolicy());
	VectorLabels.emplace_back(LabelName);
	LayoutRow->addWidget(LabelName);

	//Виджет с выбором оператора
	QString SearchOperatorWidget = MetaField->Type == ISNamespace::FT_Int && MetaField->Foreign
		? "ISComboSearchBase" :
		ISMetaData::Instance().GetSearchWidget(MetaField->Type);
	ISComboSearchBase *ComboSearchOperator = ISAlgorithm::CreatePointer<ISComboSearchBase *>(SearchOperatorWidget, Q_ARG(QWidget *, ScrollArea));
	ComboSearchOperator->setSizePolicy(QSizePolicy::Maximum, ComboSearchOperator->sizePolicy().verticalPolicy());
	LayoutRow->addWidget(ComboSearchOperator);

	//Поле редактирования
	ISFieldEditBase *FieldEditBase = ISGui::CreateColumnForField(ScrollArea, MetaField);
	if (MetaField->Foreign) //Если поле является справочным - инициализируем его
	{
		dynamic_cast<ISListEdit*>(FieldEditBase)->InvokeList(MetaField->Foreign);
	}
	FieldEditBase->SetSizePolicyHorizontal(QSizePolicy::Minimum);
	LayoutRow->addWidget(FieldEditBase);

	//Кнопка добавления
	ISServiceButton *ButtonAdd = new ISServiceButton(BUFFER_ICONS("Add"), LANG("ISSearchForm.AddValue"), ScrollArea);
	connect(ButtonAdd, &ISServiceButton::clicked, this, &ISSearchForm::AddClicked);
	ButtonAdd->setProperty("Index", LayoutScroll->count());
	ButtonAdd->setProperty("FieldName", MetaField->Name);
	LayoutRow->addWidget(ButtonAdd);

	if (Index > 0)
	{

	}
	else
	{
		LayoutScroll->addLayout(LayoutRow);
	}
}
//-----------------------------------------------------------------------------
void ISSearchForm::AddClicked()
{

}
//-----------------------------------------------------------------------------
void ISSearchForm::Search()
{

}
//-----------------------------------------------------------------------------
