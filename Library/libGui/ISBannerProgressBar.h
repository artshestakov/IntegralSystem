#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISBannerProgressBar : public QProgressBar
{
	Q_OBJECT

public:
	ISBannerProgressBar(QWidget *parent = 0);
	virtual ~ISBannerProgressBar();
};
//-----------------------------------------------------------------------------
