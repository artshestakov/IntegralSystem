#include "ISQuerySubSystem.h"
#include "ISDefinesGui.h"
#include "ISTcpQuery.h"
#include "ISDialogsCommon.h"
#include "ISLocalization.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISQuerySubSystem::ISQuerySubSystem(QWidget *parent) : ISInterfaceMetaForm(parent)
{
    GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

    QHBoxLayout *Layout = new QHBoxLayout();
    GetMainLayout()->addLayout(Layout);

    GroupBoxQuery = new QGroupBox(LANG("ISQuerySubSystem.GroupBoxQuery").arg(0), this);
    GroupBoxQuery->setLayout(new QVBoxLayout());
    Layout->addWidget(GroupBoxQuery);

    QHBoxLayout *LayoutTitle = new QHBoxLayout();
    dynamic_cast<QVBoxLayout*>(GroupBoxQuery->layout())->addLayout(LayoutTitle);

    EditSearch = new ISSearchEdit(GroupBoxQuery);
    connect(EditSearch, &ISSearchEdit::ValueChange, this, &ISQuerySubSystem::SearchEvent);
    LayoutTitle->addWidget(EditSearch);

    ListWidgetQuery = new ISListWidget(GroupBoxQuery);
    ListWidgetQuery->setAlternatingRowColors(true);
    connect(ListWidgetQuery, &ISListWidget::itemClicked, this, &ISQuerySubSystem::ItemClicked);
    GroupBoxQuery->layout()->addWidget(ListWidgetQuery);

    TextEdit = new ISTextEdit(GroupBoxQuery);
    TextEdit->SetReadOnly(true);
    GroupBoxQuery->layout()->addWidget(TextEdit);

    QVBoxLayout *LayoutRight = new QVBoxLayout();
    Layout->addLayout(LayoutRight);

    ISPushButton *ButtonClear = new ISPushButton(LANG("ISQuerySubSystem.ClearHistory"), this);
    connect(ButtonClear, &ISPushButton::clicked, this, &ISQuerySubSystem::ClearHistory);
    LayoutRight->addWidget(ButtonClear, 0, Qt::AlignRight);

    QGroupBox *GroupBoxHistory = new QGroupBox(LANG("ISQuerySubSystem.GroupBoxHistory"), this);
    GroupBoxHistory->setLayout(new QVBoxLayout());
    LayoutRight->addWidget(GroupBoxHistory);

    ListWidgetHistory = new ISListWidget(GroupBoxHistory);
    ListWidgetHistory->setAlternatingRowColors(true);
    GroupBoxHistory->layout()->addWidget(ListWidgetHistory);
}
//-----------------------------------------------------------------------------
ISQuerySubSystem::~ISQuerySubSystem()
{

}
//-----------------------------------------------------------------------------
void ISQuerySubSystem::LoadData()
{
    ListWidgetQuery->Clear();
    TextEdit->Clear();
    EditSearch->Clear();

    ISTcpQuery qStatementQueryGet(API_STATEMENTS_QUERY_GET);

    ISGui::SetWaitGlobalCursor(true);
    bool Result = qStatementQueryGet.Execute();
    ISGui::SetWaitGlobalCursor(false);
    if (!Result)
    {
        ISMessageBox::ShowCritical(this, qStatementQueryGet.GetErrorString());
        return;
    }

    QVariantMap AnswerMap = qStatementQueryGet.TakeAnswer();
    QVariantList QueryList = AnswerMap["QueryList"].toList();
    QStringList DateList = AnswerMap["DateList"].toStringList();

    for (const QVariant &Variant : QueryList)
    {
        QVariantMap QueryMap = Variant.toMap();

        QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidgetQuery);
        ListWidgetItem->setText(LANG("ISQuerySubSystem.ItemText").
            arg(QueryMap["UserID"].toString()).
            arg(QueryMap["Login"].toString()).
            arg(QueryMap["Calls"].toString()).
            arg(QueryMap["MSec"].toString()));
        ListWidgetItem->setData(Qt::UserRole, QueryMap["SqlQuery"]);
    }
    GroupBoxQuery->setTitle(LANG("ISQuerySubSystem.GroupBoxQuery").arg(QueryList.size()));

    for (const QString &DateText : DateList)
    {
        QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidgetHistory);
        ListWidgetItem->setText(DateText);
        ListWidgetItem->setSizeHint(QSize(ListWidgetItem->sizeHint().width(), 30));
    }
}
//-----------------------------------------------------------------------------
void ISQuerySubSystem::SearchEvent(const QVariant &Value)
{
    ISGui::SetWaitGlobalCursor(true);
    ListWidgetQuery->SetVisibleItems(true);
    if (Value.isValid())
    {
        for (int i = 0, c = ListWidgetQuery->count(); i < c; ++i)
        {
            QListWidgetItem *ListWidgetItem = ListWidgetQuery->item(i);
            if (!ListWidgetItem->data(Qt::UserRole).toString().toLower().contains(Value.toString().toLower()))
            {
                ListWidgetQuery->setItemHidden(ListWidgetItem, true);
            }
        }
    }
    ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISQuerySubSystem::ItemClicked(QListWidgetItem *ListWidgetItem)
{
    TextEdit->SetValue(ListWidgetItem->data(Qt::UserRole));
}
//-----------------------------------------------------------------------------
void ISQuerySubSystem::ClearHistory()
{
    if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.ClearStatementsQuery")))
    {
        ISTcpQuery qStatementsReset(API_STATEMENTS_QUERY_RESET);
        if (qStatementsReset.Execute())
        {
            LoadData();
        }
        else
        {
            ISMessageBox::ShowCritical(this, qStatementsReset.GetErrorString());
        }
    }
}
//-----------------------------------------------------------------------------
