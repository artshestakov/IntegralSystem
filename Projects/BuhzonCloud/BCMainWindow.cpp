#include "StdAfx.h"
#include "BCMainWindow.h"
#include "EXDefines.h"
#include "ISCrypterLicense.h"
#include "BCComboDelegate.h"
#include "BCContentManager.h"
//-----------------------------------------------------------------------------
#define CONNECTION_TYPE_REMOTE "{81215263-D911-4246-B9DD-03911C1725EF}"
#define CONNECTION_TYPE_LOCAL "{45DE6851-539D-4504-8FFD-CA17BEC851D8}"
//-----------------------------------------------------------------------------
BCMainWindow::BCMainWindow(QWidget *parent) : QWidget(parent)
{
	setWindowTitle(QString::fromLocal8Bit("Облачный сервис «Buhzon»"));
	setWindowIcon(QIcon(":Resources/Icon.png"));
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint | Qt::MSWindowsFixedSizeDialogHint); //Запрет на изменение размера окна
	setAutoFillBackground(true);

	Process = new QProcess(this);
	Process->setProgram(APPLICATION_DIR_PATH + "/CryptPasswordRDP.exe");
	connect(Process, &QProcess::readyReadStandardOutput, this, &BCMainWindow::ReadyReadStandardOutput);
	connect(Process, &QProcess::readyReadStandardError, this, &BCMainWindow::ReadyReadStandardError);

	Settings = new QSettings(APPLICATION_DIR_PATH + "/Config.ini", QSettings::IniFormat, this);

	QPalette Palette = palette();
	Palette.setColor(QPalette::Window, Qt::white);
	setPalette(Palette);
	
	QHBoxLayout *MainLayout = new QHBoxLayout();
	setLayout(MainLayout);

	QVBoxLayout *LayoutLeft = new QVBoxLayout();
	MainLayout->addLayout(LayoutLeft);

	QLabel *LabelLogo = new QLabel(this);
	LabelLogo->setPixmap(QPixmap(":Resources/Logo.png"));
	LayoutLeft->addWidget(LabelLogo, 0, Qt::AlignHCenter);

	QLabel *LalelVersion = new QLabel(this);
	LalelVersion->setText(QString::fromLocal8Bit("Версия: ") + qApp->applicationVersion());
	LayoutLeft->addWidget(LalelVersion, 0, Qt::AlignRight);

	QFormLayout *FormLayout = new QFormLayout();
	LayoutLeft->addLayout(FormLayout);

	EditLogin = new QLineEdit(this);
	EditLogin->setText(ISCrypterLicense::Decrypt(Settings->value("Connection/Login").toString()));
	EditLogin->setPlaceholderText(QString::fromLocal8Bit("Введите логин"));
	FormLayout->addRow(QString::fromLocal8Bit("Логин") + ":", EditLogin);

	EditPassword = new QLineEdit(this);
	EditPassword->setEchoMode(QLineEdit::Password);
	EditPassword->setText(ISCrypterLicense::Decrypt(Settings->value("Connection/Password").toString()));
	EditPassword->setPlaceholderText(QString::fromLocal8Bit("Введите пароль"));
	FormLayout->addRow(QString::fromLocal8Bit("Пароль") + ":", EditPassword);

	CheckRemember = new QCheckBox(this);
	CheckRemember->setChecked(true);
	CheckRemember->setCursor(CURSOR_POINTING_HAND);
	FormLayout->addRow(QString::fromLocal8Bit("Запомнить логин и пароль") + ":", CheckRemember);

	ComboBox = new QComboBox(this);
	ComboBox->setCursor(CURSOR_POINTING_HAND);
	ComboBox->setItemDelegate(new BCComboDelegate(ComboBox));
	ComboBox->addItem(QIcon(":Resources/Connection.Remote.png"), QString::fromLocal8Bit("Удаленный рабочий стол"), CONNECTION_TYPE_REMOTE);
	ComboBox->addItem(QIcon(":Resources/Connection.Local.png"), QString::fromLocal8Bit("1С Локально"), CONNECTION_TYPE_LOCAL);
	FormLayout->addRow(QString::fromLocal8Bit("Тип подключения") + ":", ComboBox);

	if (Settings->value("Connection/ConnectionType").toString() == CONNECTION_TYPE_REMOTE)
	{
		ComboBox->setCurrentIndex(0);
	}
	else if (Settings->value("Connection/ConnectionType").toString() == CONNECTION_TYPE_LOCAL)
	{
		ComboBox->setCurrentIndex(1);
	}

	QHBoxLayout *LayoutButtons = new QHBoxLayout();
	LayoutButtons->setContentsMargins(LAYOUT_MARGINS_10_PX);
	LayoutLeft->addLayout(LayoutButtons);

	CheckHideContent = new QCheckBox(this);
	CheckHideContent->setText(QString::fromLocal8Bit("Скрывать рекламу"));
	CheckHideContent->setToolTip(QString::fromLocal8Bit("Нажмите, чтобы реклама не отображалась при следующем запуске."));
	CheckHideContent->setCursor(CURSOR_POINTING_HAND);
	LayoutButtons->addWidget(CheckHideContent);

	LayoutButtons->addStretch();

	ButtonConnect = new QPushButton(this);
	ButtonConnect->setText(QString::fromLocal8Bit("Подключиться"));
	ButtonConnect->setToolTip(QString::fromLocal8Bit("Нажмите, чтобы подключиться к облачному сервису «Buhzon»."));
	ButtonConnect->setCursor(CURSOR_POINTING_HAND);
	connect(ButtonConnect, &QPushButton::clicked, this, &BCMainWindow::Connect);
	LayoutButtons->addWidget(ButtonConnect);

	ButtonExit = new QPushButton(this);
	ButtonExit->setText(QString::fromLocal8Bit("Выход"));
	ButtonExit->setToolTip(QString::fromLocal8Bit("Нажмите, чтобы закрыть это окно."));
	ButtonExit->setCursor(CURSOR_POINTING_HAND);
	connect(ButtonExit, &QPushButton::clicked, this, &BCMainWindow::close);
	LayoutButtons->addWidget(ButtonExit);

	QVBoxLayout *LayoutContent = new QVBoxLayout();

	if (Settings->value("Other/HideContent").toBool())
	{
		CheckHideContent->setCheckState(Qt::Checked);
	}
	else
	{
		QGroupBox *GroupBox = new QGroupBox(this);
		GroupBox->setLayout(LayoutContent);
		MainLayout->addWidget(GroupBox);

		LabelContent = new QLabel(this);
		LayoutContent->addWidget(LabelContent, 0, Qt::AlignCenter);

		LoadContent();
	}
}
//-----------------------------------------------------------------------------
BCMainWindow::~BCMainWindow()
{

}
//-----------------------------------------------------------------------------
void BCMainWindow::keyPressEvent(QKeyEvent *e)
{
	QWidget::keyPressEvent(e);

	Qt::Key KeyPressed = static_cast<Qt::Key>(e->key());

	if (KeyPressed == Qt::Key_Enter || KeyPressed == Qt::Key_Return)
	{
		Connect();
	}
	else if (KeyPressed == Qt::Key_Escape)
	{
		close();
	}
}
//-----------------------------------------------------------------------------
void BCMainWindow::showEvent(QShowEvent *e)
{
	setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), qApp->desktop()->availableGeometry()));
	QWidget::showEvent(e);
	emit HideBanner();
}
//-----------------------------------------------------------------------------
void BCMainWindow::LoadContent()
{
	QByteArray ByteArray = BCContentManager::GetInstance().GetFile();
	QString Extension = BCContentManager::GetInstance().GetExtension();
	if (Extension == "gif")
	{
		QFile f(APPLICATION_DIR_PATH + "/" + QUuid::createUuid().toString() + ".gif");
		if (f.open(QIODevice::WriteOnly))
		{
			f.write(ByteArray);
			f.close();
			QMovie *Movie = new QMovie(f.fileName(), nullptr, this);
			if (Movie->isValid())
			{
				Movie->setScaledSize(SIZE_300_300);
				Movie->start();
				LabelContent->setMovie(Movie);
			}
		}
	}
	else
	{
		QPixmap Pixmap;
		QByteArray s = QImageReader(new QBuffer(&ByteArray, this)).format();
		if (Pixmap.loadFromData(ByteArray, s))
		{
			LabelContent->setPixmap(Pixmap.scaled(SIZE_300_300, Qt::KeepAspectRatio));
		}
	}
}
//-----------------------------------------------------------------------------
void BCMainWindow::Connect()
{
	if (!EditLogin->text().length())
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("Предупреждение"), QString::fromLocal8Bit("Поле \"Логин\" обязательно для заполнения!"), QMessageBox::Ok);
		EditLogin->setFocus();
		return;
	}

	if (!EditPassword->text().length())
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("Предупреждение"), QString::fromLocal8Bit("Поле \"Пароль\" обязательно для заполнения!"), QMessageBox::Ok);
		EditPassword->setFocus();
		return;
	}

	Settings->setValue("Connection/ConnectionType", ComboBox->itemData(ComboBox->currentIndex()));

	if (CheckRemember->isChecked())
	{
		Settings->setValue("Connection/Login", ISCrypterLicense::Crypt(EditLogin->text()));
		Settings->setValue("Connection/Password", ISCrypterLicense::Crypt(EditPassword->text()));
	}
	else
	{
		Settings->setValue("Connection/Login", QString());
		Settings->setValue("Connection/Password", QString());
	}

	if (CheckHideContent->isChecked())
	{
		Settings->setValue("Other/HideContent", true);
	}
	else
	{
		Settings->setValue("Other/HideContent", false);
	}

	SetWaitCursor(true);

	if (QFile::exists(Process->program()))
	{
		Process->setArguments(QStringList() << EditPassword->text());
		Process->start();
	}
	else
	{
		SetWaitCursor(false);
		QMessageBox::warning(this, QString::fromLocal8Bit("Предупреждение"), QString::fromLocal8Bit("Не найдена утилита шифрования пароля!"), QMessageBox::Ok, QMessageBox::Ok);
	}
}
//-----------------------------------------------------------------------------
void BCMainWindow::ReadyReadStandardOutput()
{
	GenerateRdpFile(Process->readAllStandardOutput());
}
//-----------------------------------------------------------------------------
void BCMainWindow::ReadyReadStandardError()
{
	QMessageBox::critical(this, QString::fromLocal8Bit("Ошибка"), Process->readAllStandardError(), QMessageBox::Ok, QMessageBox::Ok);
}
//-----------------------------------------------------------------------------
void BCMainWindow::GenerateRdpFile(const QString &Password)
{
	QFile FileTemplate;

	if (ComboBox->itemData(ComboBox->currentIndex()).toString() == CONNECTION_TYPE_REMOTE)
	{
		FileTemplate.setFileName(":Resources/TemplateRemote.txt");
	}
	else
	{
		FileTemplate.setFileName(":Resources/TemplateLocal.txt");
	}

	if (!FileTemplate.open(QIODevice::ReadOnly))
	{
		SetWaitCursor(false);
		QMessageBox::warning(this, QString::fromLocal8Bit("Ошибка"), FileTemplate.errorString(), QMessageBox::Ok);
		return;
	}

	QString Template = FileTemplate.readAll();
	FileTemplate.close();
	FileTemplate.setFileName(APPLICATION_DIR_PATH + "/Buhzon.rdp");
	if (!FileTemplate.open(QIODevice::WriteOnly))
	{
		SetWaitCursor(false);
		QMessageBox::warning(this, QString::fromLocal8Bit("Ошибка"), FileTemplate.errorString(), QMessageBox::Ok);
		return;
	}

	Template.replace("%Login%", EditLogin->text() + "@buhzon.ru");
	Template.replace("%Password%", Password);

	FileTemplate.write(Template.toUtf8());
	FileTemplate.close();

	bool Opended = QDesktopServices::openUrl(QUrl(QUrl::fromLocalFile(FileTemplate.fileName())));
	if (Opended)
	{
		close();
	}
	else
	{
		SetWaitCursor(false);
	}
}
//-----------------------------------------------------------------------------
void BCMainWindow::SetWaitCursor(bool Wait)
{
	if (Wait)
	{
		QApplication::setOverrideCursor(CURSOR_WAIT);
	}
	else
	{
		QApplication::restoreOverrideCursor();

		if (QApplication::overrideCursor())
		{
			if (QApplication::overrideCursor()->shape() == CURSOR_WAIT)
			{
				QApplication::restoreOverrideCursor();
			}
		}
	}

	ButtonConnect->setEnabled(!Wait);
}
//-----------------------------------------------------------------------------
