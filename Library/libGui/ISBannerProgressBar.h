#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISBannerProgressBar : public QProgressBar
{
	Q_OBJECT

public:
	ISBannerProgressBar(QWidget *parent = 0);
	virtual ~ISBannerProgressBar();
};
//-----------------------------------------------------------------------------
