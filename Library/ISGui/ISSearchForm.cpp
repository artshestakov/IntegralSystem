#include "ISSearchForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISGui.h"
#include "ISDelegates.h"
#include "ISDialogsCommon.h"
#include "ISAlgorithm.h"
#include "ISButtons.h"
#include "ISScrollArea.h"
//-----------------------------------------------------------------------------
ISSearchForm::ISSearchForm(ISTcpModel *tcp_model, const QString &LocalListName, QWidget *parent)
    : ISInterfaceForm(parent),
    TcpModel(tcp_model)
{
    setWindowTitle(LANG("ISSearchForm.Title") + " - " + LocalListName);
    setWindowIcon(BUFFER_ICONS("Search"));
    resize(800, 600);
    GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

    ISScrollArea *ScrollArea = new ISScrollArea(this);
    GetMainLayout()->addWidget(ScrollArea);

    GridLayout = new QGridLayout();
    ScrollArea->widget()->setLayout(GridLayout);

    for (int i = 0, c = TcpModel->columnCount(); i < c; ++i)
    {
        const ISModelField &ModelField = TcpModel->GetField(i);

        //Пропускаем системные поля и поля по которым поиск зарещён
        if (ModelField.IsSystem || ModelField.NotSearch)
        {
            continue;
        }
        AddField(ModelField);
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
void ISSearchForm::AddField(const ISModelField &ModelField)
{
    int RowIndex = GridLayout->rowCount();

    //Заголовок поискового поля
    QLabel *LabelName = new QLabel(ModelField.LocalName + ':', this);
    ISGui::SetFontWidgetBold(LabelName, true);
    GridLayout->addWidget(LabelName, RowIndex, 0);

    //Виджет с выбором оператора
    QString SearchOperatorWidget = ModelField.Type == ISNamespace::FieldType::BigInt && ModelField.IsForeign
        ? "ISComboSearchBase" :
        ModelField.SearchConditionWidget;
    ISComboSearchBase *ComboSearchOperator = ISAlgorithm::CreatePointer<ISComboSearchBase *>(SearchOperatorWidget, Q_ARG(QWidget *, this));
    GridLayout->addWidget(ComboSearchOperator, RowIndex, 1);

    //Поле редактирования
    ISFieldEditBase *FieldEditBase = ISGui::CreateColumnForField(this, ModelField.Type, ModelField.ControlWidget);
    if (ModelField.IsForeign)
    {
        FieldEditBase->setProperty("MainEdit", true);
        dynamic_cast<ISListEdit*>(FieldEditBase)->InvokeList(ModelField.Foreign);
        connect(FieldEditBase, &ISFieldEditBase::ValueChange, this, &ISSearchForm::ListEditChanged);

        QVBoxLayout *Layout = new QVBoxLayout();
        QWidget *Widget = new QWidget(this);
        Widget->setLayout(Layout);
        GridLayout->addWidget(Widget, RowIndex, 2);
        Layout->addWidget(FieldEditBase);

        ISPushButton *ButtonAdd = new ISPushButton(BUFFER_ICONS("Add"), LANG("ISSearchForm.AddValue"), Widget);
        ButtonAdd->setFlat(true);
        ButtonAdd->setProperty("FieldName", ModelField.Name);
        ButtonAdd->setProperty("FieldType", (int)ModelField.Type);
        ButtonAdd->setProperty("FieldControlWidget", ModelField.ControlWidget);
        connect(ButtonAdd, &ISPushButton::clicked, this, &ISSearchForm::AddClicked);
        Layout->addWidget(ButtonAdd, 0, Qt::AlignLeft);
    }
    else
    {
        GridLayout->addWidget(FieldEditBase, RowIndex, 2);
    }
    VectorEdits.push_back({ ModelField.Name, ComboSearchOperator, std::vector<ISFieldEditBase*>{FieldEditBase} });
}
//-----------------------------------------------------------------------------
void ISSearchForm::AddClicked()
{
    ISPushButton *ButtonSender = dynamic_cast<ISPushButton*>(sender());

    ISFieldEditBase *FieldEditBase = ISGui::CreateColumnForField(this, (ISNamespace::FieldType)ButtonSender->property("FieldType").toInt(), ButtonSender->property("FieldControlWidget").toString());
    FieldEditBase->setProperty("MainEdit", false);
    //dynamic_cast<ISListEdit*>(FieldEditBase)->InvokeList(MetaField->Foreign);
    connect(FieldEditBase, &ISFieldEditBase::ValueChange, this, &ISSearchForm::ListEditChanged);

    for (SearchField &Field : VectorEdits)
    {
        if (Field.Name == ButtonSender->property("FieldName").toString())
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
