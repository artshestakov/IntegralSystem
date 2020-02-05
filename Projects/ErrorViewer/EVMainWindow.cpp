#include "EVMainWindow.h"
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtGui/QClipboard>
#include <QtWidgets/QApplication>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QAction>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
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

	TextEdit = new QTextEdit(GroupBox);
	TextEdit->setReadOnly(true);
	TextEdit->setContextMenuPolicy(Qt::NoContextMenu);
	GroupBox->layout()->addWidget(TextEdit);

	QHBoxLayout *LayoutBottom = new QHBoxLayout();
	LayoutBottom->addStretch();
	MainLayout->addLayout(LayoutBottom);

	QPushButton *ButtonOK = new QPushButton(this);
	ButtonOK->setText("OK");
	connect(ButtonOK, &QPushButton::clicked, this, &EVMainWindow::close);
	LayoutBottom->addWidget(ButtonOK);

	QPushButton *ButtonClipboard = new QPushButton(this);
	ButtonClipboard->setText("Copy to clipboard");
	connect(ButtonClipboard, &QPushButton::clicked, this, &EVMainWindow::CopyToClipboard);
	LayoutBottom->addWidget(ButtonClipboard);

	QPushButton *ButtonSave = new QPushButton(this);
	ButtonSave->setText("Save...");
	connect(ButtonSave, &QPushButton::clicked, this, &EVMainWindow::Save);
	LayoutBottom->addWidget(ButtonSave);

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
void EVMainWindow::CopyToClipboard()
{
	QApplication::clipboard()->setText(TextEdit->toPlainText());
}
//-----------------------------------------------------------------------------
void EVMainWindow::Save()
{
	QString FilePath = QFileDialog::getSaveFileName(this, "Save", QDir::homePath(), "Log file (*.log)");
	if (!FilePath.isEmpty())
	{
		QFile FileCrash(FilePath);
		if (FileCrash.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			FileCrash.write(TextEdit->toPlainText().toUtf8());
			FileCrash.close();
			
			QApplication::beep();
			QMessageBox::information(this, "Information", "File saving with path: " + FilePath, QMessageBox::Ok, QMessageBox::Ok);
		}
		else
		{
			QApplication::beep();
			QMessageBox::warning(this, "Warning", "Error save file \"" + FilePath + "\": " + FileCrash.errorString(), QMessageBox::Ok, QMessageBox::Ok);
		}
	}
}
//-----------------------------------------------------------------------------
