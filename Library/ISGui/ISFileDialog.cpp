#include "ISFileDialog.h"
#include "ISLocalization.h"
#include "ISDefinesCore.h"
//-----------------------------------------------------------------------------
ISFileDialog::ISFileDialog(QWidget *parent) : QFileDialog(parent)
{
	
}
//-----------------------------------------------------------------------------
ISFileDialog::~ISFileDialog()
{
	
}
//-----------------------------------------------------------------------------
QString ISFileDialog::GetOpenFileNameImage(QWidget *parent)
{
	QString FilterString;
	FilterString += LANG("File.Filter.Images") + ";;";
	FilterString += LANG("File.Filter.Image.PNG") + ";;";
	FilterString += LANG("File.Filter.Image.JPEG") + ";;";
	FilterString += LANG("File.Filter.Image.JPG") + ";;";
	FilterString += LANG("File.Filter.Image.DDS") + ";;";
	FilterString += LANG("File.Filter.Image.GIF") + ";;";
	FilterString += LANG("File.Filter.Image.ICNS") + ";;";
	FilterString += LANG("File.Filter.Image.ICO") + ";;";
	FilterString += LANG("File.Filter.Image.SVG") + ";;";
	FilterString += LANG("File.Filter.Image.TGA") + ";;";
	FilterString += LANG("File.Filter.Image.TIFF") + ";;";
	FilterString += LANG("File.Filter.Image.WBMP") + ";;";
	FilterString += LANG("File.Filter.Image.BMP") + ";;";
	FilterString += LANG("File.Filter.Image.WEBP");
	
	QString Path = ISFileDialog::getOpenFileName(parent, LANG("FileDialog.Image.Select.Title"), ISDefines::Core::PATH_LAST_DIRECTORY, FilterString);
	if (!Path.isEmpty())
	{
		ISDefines::Core::PATH_LAST_DIRECTORY = QFileInfo(Path).dir().path();
	}
	return Path;
}
//-----------------------------------------------------------------------------
QString ISFileDialog::GetSaveFileNameImage(QWidget *parent, const QString &FileName)
{
	QString FilterString;
	FilterString += LANG("File.Filter.Images") + ";;";
	FilterString += LANG("File.Filter.Image.PNG") + ";;";
	FilterString += LANG("File.Filter.Image.JPEG") + ";;";
	FilterString += LANG("File.Filter.Image.JPG") + ";;";
	FilterString += LANG("File.Filter.Image.DDS") + ";;";
	FilterString += LANG("File.Filter.Image.GIF") + ";;";
	FilterString += LANG("File.Filter.Image.ICNS") + ";;";
	FilterString += LANG("File.Filter.Image.ICO") + ";;";
	FilterString += LANG("File.Filter.Image.SVG") + ";;";
	FilterString += LANG("File.Filter.Image.TGA") + ";;";
	FilterString += LANG("File.Filter.Image.TIFF") + ";;";
	FilterString += LANG("File.Filter.Image.WBMP") + ";;";
	FilterString += LANG("File.Filter.Image.BMP") + ";;";
	FilterString += LANG("File.Filter.Image.WEBP");

	QString Path = ISFileDialog::getSaveFileName(parent, LANG("FileDialog.Image.Save.Title"), ISDefines::Core::PATH_LAST_DIRECTORY + (FileName.isEmpty() ? QString() : '/' + FileName), FilterString);
	if (!Path.isEmpty())
	{
		ISDefines::Core::PATH_LAST_DIRECTORY = QFileInfo(Path).dir().path();
	}
	return Path;
}
//-----------------------------------------------------------------------------
QString ISFileDialog::GetOpenFileName(QWidget *parent, const QString &FilePath, const QString &FilterName)
{
	QString Path = ISFileDialog::getOpenFileName(parent, LANG("FileDialog.File.Open.Title"), FilePath.isEmpty() ? ISDefines::Core::PATH_LAST_DIRECTORY : FilePath, FilterName);
	if (!Path.isEmpty())
	{
		ISDefines::Core::PATH_LAST_DIRECTORY = QFileInfo(Path).dir().path();
	}
	return Path;
}
//-----------------------------------------------------------------------------
QString ISFileDialog::GetSaveFileName(QWidget *parent, const QString &FilterName, const QString &FileName)
{
	QString Path = ISFileDialog::getSaveFileName(parent, LANG("FileDialog.File.Save.Title"), ISDefines::Core::PATH_LAST_DIRECTORY + (FileName.isEmpty() ? QString() : '/' + FileName), FilterName);
	if (!Path.isEmpty())
	{
		ISDefines::Core::PATH_LAST_DIRECTORY = QFileInfo(Path).dir().path();
	}
	return Path;
}
//-----------------------------------------------------------------------------
QString ISFileDialog::GetDirectoryPath(QWidget *parent)
{
	QString Path = ISFileDialog::getExistingDirectory(parent, LANG("FileDialog.Directory.Title"), ISDefines::Core::PATH_LAST_DIRECTORY, ISFileDialog::ShowDirsOnly);
	if (!Path.isEmpty())
	{
		ISDefines::Core::PATH_LAST_DIRECTORY = QFileInfo(Path).dir().path();
	}
	return Path;
}
//-----------------------------------------------------------------------------
QStringList ISFileDialog::GetOpenFileNames(QWidget *parent, const QString &FilterName)
{
	QStringList StringList = ISFileDialog::getOpenFileNames(parent, LANG("FileDialog.Files.Open.Title"), ISDefines::Core::PATH_LAST_DIRECTORY, FilterName);
	if (!StringList.isEmpty())
	{
		ISDefines::Core::PATH_LAST_DIRECTORY = QFileInfo(StringList.front()).dir().path();
	}
	return StringList;
}
//-----------------------------------------------------------------------------
