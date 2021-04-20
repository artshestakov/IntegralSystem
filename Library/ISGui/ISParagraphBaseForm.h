#pragma once
#ifndef _ISPARAGPATHBASEFORM_H_INCLUDED
#define _ISPARAGPATHBASEFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISParagraphBaseForm : public QWidget
{
    Q_OBJECT

public:
    ISParagraphBaseForm(QWidget *parent = 0);
    virtual ~ISParagraphBaseForm();

    virtual void Invoke() = 0;
};
//-----------------------------------------------------------------------------
#endif
