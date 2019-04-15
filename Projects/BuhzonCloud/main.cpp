#include "StdAfx.h"
#include "BCMainWindow.h"
#include "EXDefines.h"
#include "BCSplashScreen.h"
#include "BCContentManager.h"
//-----------------------------------------------------------------------------
bool CheckAdminRole(); //Проверка прав администратора
void ClearOldRDP(); //Очистка старых RDP-файлов
void ReadVersion(); //Чтение версии
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	QApplication Application(argc, argv);
	Application.setOrganizationName("Softline");
	Application.setFont(FONT_APPLICATION);

	if (!CheckAdminRole())
	{
		return EXIT_CODE_ERROR;
	}

	BCSplashScreen SplashScreen;
	SplashScreen.setPixmap(QPixmap(":Resources/Logo.png"));
	SplashScreen.repaint();
	SplashScreen.show();
	qApp->processEvents();

	ClearOldRDP();
	ReadVersion();

	BCContentManager::GetInstance();

	BCMainWindow MainWindow;
	QObject::connect(&MainWindow, &BCMainWindow::HideBanner, &SplashScreen, &BCSplashScreen::close);
	MainWindow.show();

	return Application.exec();
}
//-----------------------------------------------------------------------------
bool CheckAdminRole()
{
	QFile File(APPLICATION_DIR_PATH + "/CheckAdmin");
	bool Opened = File.open(QIODevice::WriteOnly);

	if (Opened)
	{
		File.close();
		File.remove();
	}
	else
	{
		if (File.error() == QFileDevice::OpenError)
		{
			QMessageBox::critical(nullptr, QString::fromLocal8Bit("Ошибка"), QString::fromLocal8Bit("Недостаточно прав! Запустите программу от имени админитстратора или обратитесь к поставщику облачного сервиса «Buhzon»."), QMessageBox::Ok);
		}
		else
		{
			Opened = true;
		}
	}

	return Opened;
}
//-----------------------------------------------------------------------------
void ClearOldRDP()
{
	QFile FileRDP(APPLICATION_DIR_PATH + "/Buhzon.rdp");
	if (FileRDP.exists())
	{
		if (!FileRDP.remove())
		{
			QMessageBox::warning(nullptr, QString::fromLocal8Bit("Ошибка"), QString::fromLocal8Bit("Не удалось удалить файл: %1 \nОшибка: %2").arg(FileRDP.fileName()).arg(FileRDP.errorString()), QMessageBox::Ok);
		}
	}
}
//-----------------------------------------------------------------------------
void ReadVersion()
{
	QFile FileVersion(":Resources/Version.txt");
	if (FileVersion.open(QIODevice::ReadOnly))
	{
		QString VersionString = FileVersion.readAll();
		VersionString.remove(VersionString.length() - 3, 3);
		qApp->setApplicationVersion(VersionString);
		FileVersion.close();
	}
}
//-----------------------------------------------------------------------------
