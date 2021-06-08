#include "ISSearchForm.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISGui.h"
#include "ISMetaData.h"
#include "ISDelegates.h"
#include "ISDialogsCommon.h"
#include "ISButtons.h"
#include "ISScrollArea.h"
//-----------------------------------------------------------------------------
ISSearchForm::ISSearchForm(PMetaTable *meta_table, QWidget *parent)
    : ISInterfaceForm(parent),
    MetaTable(meta_table)
{
    setWindowTitle(LANG_FMT("ISSearchForm.Title", MetaTable->LocalListName.toStdString().c_str()));
    setWindowIcon(BUFFER_ICONS("Search"));
    resize(800, 600);
    GetMainLayout()->setContentsMargins(QMargins(10, 10, 10, 10));

    ISScrollArea *ScrollArea = new ISScrollArea(this);
    GetMainLayout()->addWidget(ScrollArea);

    GridLayout = new QGridLayout();
    ScrollArea->widget()->setLayout(GridLayout);

    for (PMetaField *MetaField : MetaTable->Fields)
    {
        //Проверяем возможность поиска в движке
        if (!ISMetaData::Instance().GetType(MetaField->Type).SearchAllowed)
        {
            continue;
        }

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

    ISPushButton *ButtonSearch = new ISPushButton(this);
    ButtonSearch->setText(LANG("Search"));
    ButtonSearch->setIcon(BUFFER_ICONS("Search"));
    connect(ButtonSearch, &ISPushButton::clicked, this, static_cast<void(ISSearchForm::*)(void)>(&ISSearchForm::Search));
    LayoutBottom->addWidget(ButtonSearch);

    ISPushButton *ButtonHide = new ISPushButton(this);
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
    Search();
}
//-----------------------------------------------------------------------------
void ISSearchForm::AddField(PMetaField *MetaField)
{
    int RowIndex = GridLayout->rowCount();

    //Заголовок поискового поля
    QLabel *LabelName = new QLabel(MetaField->LabelName + ':', this);
    ISGui::SetFontWidgetBold(LabelName, true);
    GridLayout->addWidget(LabelName, RowIndex, 0);

    //Виджет с выбором оператора
    QString SearchOperatorWidget = MetaField->Type == ISNamespace::FieldType::BigInt && MetaField->Foreign
        ? "ISComboSearchBase" :
        QString::fromStdString(ISMetaData::Instance().GetType(MetaField->Type).SearchConditionWidget);
    ISComboSearchBase *ComboSearchOperator = ISGui::CreatePointer<ISComboSearchBase *>(SearchOperatorWidget, Q_ARG(QWidget *, this));
    GridLayout->addWidget(ComboSearchOperator, RowIndex, 1);

    //Поле редактирования
    ISFieldEditBase *FieldEditBase = ISGui::CreateColumnForField(this, MetaField);
    if (MetaField->Foreign)
    {
        FieldEditBase->setProperty("MainEdit", true);
        dynamic_cast<ISListEdit*>(FieldEditBase)->InvokeList(MetaField->Foreign);
        connect(FieldEditBase, &ISFieldEditBase::ValueChange, this, &ISSearchForm::ListEditChanged);

        QVBoxLayout *Layout = new QVBoxLayout();
        QWidget *Widget = new QWidget(this);
        Widget->setLayout(Layout);
        GridLayout->addWidget(Widget, RowIndex, 2);
        Layout->addWidget(FieldEditBase);

        ISPushButton *ButtonAdd = new ISPushButton(BUFFER_ICONS("Add"), LANG("ISSearchForm.AddValue"), Widget);
        ButtonAdd->setFlat(true);
        ButtonAdd->setProperty("FieldName", MetaField->Name);
        connect(ButtonAdd, &ISPushButton::clicked, this, &ISSearchForm::AddClicked);
        Layout->addWidget(ButtonAdd, 0, Qt::AlignLeft);
    }
    else
    {
        GridLayout->addWidget(FieldEditBase, RowIndex, 2);
    }
    VectorEdits.push_back({ MetaField->Name, ComboSearchOperator, std::vector<ISFieldEditBase*>{FieldEditBase} });
}
//-----------------------------------------------------------------------------
void ISSearchForm::AddClicked()
{
    ISPushButton *ButtonSender = dynamic_cast<ISPushButton*>(sender());
    PMetaField *MetaField = MetaTable->GetField(ButtonSender->property("FieldName").toString());

    ISFieldEditBase *FieldEditBase = ISGui::CreateColumnForField(this, MetaField);
    FieldEditBase->setProperty("MainEdit", false);
    dynamic_cast<ISListEdit*>(FieldEditBase)->InvokeList(MetaField->Foreign);
    connect(FieldEditBase, &ISFieldEditBase::ValueChange, this, &ISSearchForm::ListEditChanged);

    for (SearchField &Field : VectorEdits)
    {
        if (Field.Name == MetaField->Name)
        {
            Field.Edits.push_back(FieldEditBase);
        }
    }

    QVBoxLayout *Layout = dynamic_cast<QVBoxLayout*>(ButtonSender->parentWidget()->layout());
    Layout->insertWidget(Layout->indexOf(ButtonSender), FieldEditBase);
    QTimer::singleShot(100, dynamic_cast<ISListEdit*>(FieldEditBase), &ISListEdit::ShowPopup);
}
//-----------------------------------------------------------------------------
void ISSearchForm::ListEditChanged(const QVariant &Value)
{
    if (!Value.isValid() && !sender()->property("MainEdit").toBool())
    {
        dynamic_cast<ISListEdit*>(sender())->deleteLater();
    }
}
//-----------------------------------------------------------------------------
void ISSearchForm::Search()
{
    QVariantList VariantList;
    for (const SearchField &Field : VectorEdits)
    {
        QVariantList Values;
        for (ISFieldEditBase *FieldEditBase : Field.Edits)
        {
            //Если поле не изменялось - пропускаем его
            QVariant Value = FieldEditBase->GetValue();
            if (!FieldEditBase->GetModificationFlag() && !Value.isValid())
            {
                continue;
            }
            Values.push_back(Value);
        }

        if (Values.isEmpty())
        {
            continue;
        }

        VariantList.push_back(QVariantMap
        {
            { "FieldName", Field.Name },
            { "Operator", static_cast<unsigned int>(Field.SearchOperator->GetOperator()) },
            { "Values", Values }
        });
    }

    if (!VariantList.isEmpty()) //Если значения для поиска есть: виджеты-редакторы заполнены
    {
        emit Search(VariantList);
        hide();
    }
    else //Виджеты-редакторы не заполнены
    {
        ISMessageBox::ShowWarning(this, LANG("Message.Warning.SearchFieldsIsEmpty"));
    }
}
//-----------------------------------------------------------------------------
