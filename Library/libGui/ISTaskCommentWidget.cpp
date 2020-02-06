#include "ISTaskCommentWidget.h"
#include "ISDefinesGui.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISTaskCommentWidget::ISTaskCommentWidget(const QDateTime &CreationDate, const QString &UserFullName, const QString &Comment, QWidget *parent) : QWidget(parent)
{
	QHBoxLayout *MainLayout = new QHBoxLayout();
	MainLayout->setContentsMargins(DEFINES_GUI.MARGINS_LAYOUT_2_PX);
	setLayout(MainLayout);

	QVBoxLayout *Layout = new QVBoxLayout();
	MainLayout->addLayout(Layout);

	QLabel *LabelUser = new QLabel(this);
	LabelUser->setText(UserFullName);
	LabelUser->setFont(DEFINES_GUI.FONT_APPLICATION_BOLD);
	Layout->addWidget(LabelUser);

	QLabel *LabelComment = new QLabel(this);
	LabelComment->setText(Comment);
	LabelComment->setWordWrap(true);
	LabelComment->adjustSize();
	LabelComment->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
	Layout->addWidget(LabelComment);

	QLabel *LabelDateTime = new QLabel(this);
	LabelDateTime->setText(CreationDate.toString(DATE_TIME_FORMAT_V2));
	LabelDateTime->setSizePolicy(QSizePolicy::Maximum, LabelDateTime->sizePolicy().verticalPolicy());
	MainLayout->addWidget(LabelDateTime);
}
//-----------------------------------------------------------------------------
ISTaskCommentWidget::~ISTaskCommentWidget()
{

}
//-----------------------------------------------------------------------------
