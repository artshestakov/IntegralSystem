#pragma once
#ifndef _ISSEARCHFORM_H_INCLUDED
#define _ISSEARCHFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISInterfaceForm.h"
#include "PMetaClass.h"
#include "ISComboSearchWidgets.h"
#include "ISFieldEditBase.h"
#include "ISTcpModels.h"
//-----------------------------------------------------------------------------
struct SearchField
{
    QString Name;
    ISComboSearchBase *SearchOperator;
    std::vector<ISFieldEditBase*> Edits;
};
//-----------------------------------------------------------------------------
class ISSearchForm : public ISInterfaceForm
{
    Q_OBJECT

signals:
    void Search(const QVariantList &VariantList);

public:
    ISSearchForm(ISTcpModel *tcp_model, const QString &LocalListName, QWidget *parent = 0);
    virtual ~ISSearchForm();

protected:
    void closeEvent(QCloseEvent *e) override;
    void EscapeClicked() override;
    void EnterClicked() override;

private:
    void AddField(const ISModelField &ModelField);
    void AddClicked();
    void ListEditChanged(const QVariant &Value);
    void Search();

private:
    ISTcpModel *TcpModel;
    std::vector<SearchField> VectorEdits;

private:
    QGridLayout *GridLayout;
};
//-----------------------------------------------------------------------------
#endif
