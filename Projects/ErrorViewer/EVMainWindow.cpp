#include "EVMainWindow.h"
#include <QtCore/QFile>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QAction>
//-----------------------------------------------------------------------------
EVMainWindow::EVMainWindow(const QString &PathCrashFile, QWidget *parent) : QWidget(parent)
{
	QVBoxLayout *MainLayout = new QVBoxLayout();
	setLayout(MainLayout);

	QLabel *LabelTitle = new QLabel(this);
	LabelTitle->setText("Application failed:");
	MainLayout->addWidget(LabelTitle, 0, Qt::AlignLeft);

	QGroupBox *GroupBox = new QGroupBox(this);
	GroupBox->setTitle("Error");
	GroupBox->setLayout(new QVBoxLayout());
	MainLayout->addWidget(GroupBox);

	QTextEdit *TextEdit = new QTextEdit(GroupBox);
	TextEdit->setReadOnly(true);
	TextEdit->setContextMenuPolicy(Qt::NoContextMenu);
	GroupBox->layout()->addWidget(TextEdit);

	QPushButton *ButtonOK = new QPushButton(this);
	ButtonOK->setText("OK");
	ButtonOK->setFocus(Qt::FocusReason::TabFocusReason);
	connect(ButtonOK, &QPushButton::clicked, this, &EVMainWindow::close);
	MainLayout->addWidget(ButtonOK, 0, Qt::AlignHCenter);

	QAction *ActionEscape = new QAction(this);
	ActionEscape->setShortcut(Qt::Key_Escape);
	connect(ActionEscape, &QAction::triggered, this, &EVMainWindow::close);
	addAction(ActionEscape);

	QAction *ActionEnter = new QAction(this);
	ActionEnter->setShortcut(Qt::Key_Enter);
	connect(ActionEnter, &QAction::triggered, this, &EVMainWindow::close);
	addAction(ActionEnter);

	QAction *ActionReturn = new QAction(this);
	ActionReturn->setShortcut(Qt::Key_Return);
	connect(ActionReturn, &QAction::triggered, this, &EVMainWindow::close);
	addAction(ActionReturn);

	if (PathCrashFile.isEmpty())
	{
		TextEdit->setText("Path to crash file is empty.");
	}
	else
	{
		QFile FileCrash(PathCrashFile);
		if (FileCrash.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			TextEdit->setText(FileCrash.readAll());
			FileCrash.close();
		}
		else
		{
			TextEdit->setText("Error open crash file \"" + PathCrashFile + "\": " + FileCrash.errorString());
		}
	}
}
//-----------------------------------------------------------------------------
EVMainWindow::~EVMainWindow()
{

}
//-----------------------------------------------------------------------------
