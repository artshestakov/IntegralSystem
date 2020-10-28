#include "LVTabPanel.h"
//-----------------------------------------------------------------------------
LVTabPanel::LVTabPanel(const QString &FilePath, QWidget *parent)
	: QWidget(parent),
	Model(new LVLogModel(this))
{
	QFile File(FilePath);
	if (File.open(QIODevice::ReadOnly))
	{
		QString Content = File.readAll();
		File.close();

		if (Model->SetContent(Content))
		{

		}
	}
	else
	{

	}
}
//-----------------------------------------------------------------------------
LVTabPanel::~LVTabPanel()
{

}
//-----------------------------------------------------------------------------
