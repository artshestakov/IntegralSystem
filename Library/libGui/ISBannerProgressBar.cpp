#include "ISBannerProgressBar.h"
#include "ISStyleSheet.h"
//-----------------------------------------------------------------------------
ISBannerProgressBar::ISBannerProgressBar(QWidget *parent) : QProgressBar(parent)
{
	setValue(0);
	setRange(0, 0);
	setStyleSheet(STYLE_SHEET("Banner.ProgressBar"));
}
//-----------------------------------------------------------------------------
ISBannerProgressBar::~ISBannerProgressBar()
{

}
//-----------------------------------------------------------------------------
