#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISDiscussionListForm : public ISListBaseForm
{
    Q_OBJECT

public:
    Q_INVOKABLE ISDiscussionListForm(QWidget *parent = 0);
    virtual ~ISDiscussionListForm();

    void LoadData() override;

protected:
    void Create() override;
    void CreateCopy() override;
    void Edit() override;
};
//-----------------------------------------------------------------------------
