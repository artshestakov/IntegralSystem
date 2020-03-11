#pragma once
#ifndef _ISVINEDIT_H_INCLUDED
#define _ISVINEDIT_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class ISVINEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISVINEdit(QWidget *parent);
	~ISVINEdit();
};
//-----------------------------------------------------------------------------
#endif
