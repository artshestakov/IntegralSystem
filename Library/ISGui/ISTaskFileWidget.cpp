#include "ISTaskFileWidget.h"
#include "ISDefinesGui.h"
#include "ISBuffer.h"
#include "ISGui.h"
#include "ISLabels.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISTaskFileWidget::ISTaskFileWidget(const QDateTime &DateTime, const QString &UserName, const QString &FileName, const QString &Size, const QByteArray &Icon, const QString &Note, QWidget *parent) : QWidget(parent)
{
	QVBoxLayout *MainLayout = new QVBoxLayout();
	setLayout(MainLayout);

	QLabel *LabelUser = new QLabel(this);
	LabelUser->setText(UserName);
	LabelUser->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
	MainLayout->addWidget(LabelUser);

	QHBoxLayout *Layout = new QHBoxLayout();
	MainLayout->addLayout(Layout);

	QLabel *LabelImage = new QLabel(this);
	Layout->addWidget(LabelImage);

	if (Icon.length())
	{
		QPixmap Pixmap;
		if (Pixmap.loadFromData(Icon))
		{		
			LabelImage->setPixmap(Pixmap.scaled(ISDefines::Gui::SIZE_18_18));
		}
		else
		{
			LabelImage->setPixmap(BUFFER_ICONS("Task.AttachFileItem").pixmap(ISDefines::Gui::SIZE_18_18));
		}
	}
	else
	{
		LabelImage->setPixmap(BUFFER_ICONS("Task.AttachFileItem").pixmap(ISDefines::Gui::SIZE_18_18));
	}

	ISLabelLink *LabelFile = new ISLabelLink(this);
	LabelFile->setText(FileName);
	Layout->addWidget(LabelFile);
	ISGui::SetFontWidgetUnderline(LabelFile, true);

	Layout->addWidget(new QLabel('(' + Size + ')', this));
	Layout->addStretch();
	Layout->addWidget(new QLabel(DateTime.toString(FORMAT_DATE_TIME_V2), this));

	if (Note.length())
	{
		QLabel *LabelNote = new QLabel(this);
		LabelNote->setText(Note);
		MainLayout->addWidget(LabelNote);
	}
}
//-----------------------------------------------------------------------------
ISTaskFileWidget::~ISTaskFileWidget()
{

}
//-----------------------------------------------------------------------------