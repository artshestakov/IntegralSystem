#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISBannerProgressBar : public QProgressBar
{
	Q_OBJECT

public:
	ISBannerProgressBar(QWidget *parent = 0);
	virtual ~ISBannerProgressBar();
};
//-----------------------------------------------------------------------------
