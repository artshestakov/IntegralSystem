#pragma once
#ifndef _ISUSERPHOTOCREATOR_H_INCLUDED
#define _ISUSERPHOTOCREATOR_H_INCLUDED
//-----------------------------------------------------------------------------
#include "isgui_global.h"
//-----------------------------------------------------------------------------
class ISUserPhotoCreator
{
public:
	ISUserPhotoCreator();
	~ISUserPhotoCreator();

	QPixmap Create(const QString &FIO);

private:
	QColor Color;
};
//-----------------------------------------------------------------------------
#endif
