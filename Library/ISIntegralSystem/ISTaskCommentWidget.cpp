#include "StdAfx.h"
#include "ISTaskCommentWidget.h"
#include "EXDefines.h"
//-----------------------------------------------------------------------------
ISTaskCommentWidget::ISTaskCommentWidget(const QDateTime &CreationDate, const QString &UserFullName, const QString &Comment, QWidget *parent) : QWidget(parent)
{
	QHBoxLayout *MainLayout = new QHBoxLayout();
	MainLayout->setContentsMargins(LAYOUT_MARGINS_2_PX);
	setLayout(MainLayout);

	QVBoxLayout *Layout = new QVBoxLayout();
	MainLayout->addLayout(Layout);

	QLabel *LabelUser = new QLabel(this);
	LabelUser->setText(UserFullName);
	LabelUser->setFont(FONT_APPLICATION_BOLD);
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
