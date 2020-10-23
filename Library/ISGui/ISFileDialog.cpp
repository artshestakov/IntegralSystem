#include "ISFileDialog.h"
#include "ISLocalization.h"
#include "ISProperty.h"
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
	
	QString Path = ISFileDialog::getOpenFileName(parent, LANG("FileDialog.Image.Select.Title"), PROPERTY_GET("PathLastDirectory").toString(), FilterString);
	if (!Path.isEmpty())
	{
		PROPERTY_SET("PathLastDirectory", QFileInfo(Path).dir().path());
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

	QString Path = ISFileDialog::getSaveFileName(parent, LANG("FileDialog.Image.Save.Title"), PROPERTY_GET("PathLastDirectory").toString() + (FileName.isEmpty() ? QString() : '/' + FileName), FilterString);
	if (!Path.isEmpty())
	{
		PROPERTY_SET("PathLastDirectory", QFileInfo(Path).dir().path());
	}
	return Path;
}
//-----------------------------------------------------------------------------
QString ISFileDialog::GetOpenFileName(QWidget *parent, const QString &FilePath, const QString &FilterName)
{
	QString Path = ISFileDialog::getOpenFileName(parent, LANG("FileDialog.File.Open.Title"), FilePath.isEmpty() ? PROPERTY_GET("PathLastDirectory").toString() : FilePath, FilterName);
	if (!Path.isEmpty())
	{
		PROPERTY_SET("PathLastDirectory", QFileInfo(Path).dir().path());
	}
	return Path;
}
//-----------------------------------------------------------------------------
QString ISFileDialog::GetSaveFileName(QWidget *parent, const QString &FilterName, const QString &FileName)
{
	QString Path = ISFileDialog::getSaveFileName(parent, LANG("FileDialog.File.Save.Title"), PROPERTY_GET("PathLastDirectory").toString() + (FileName.isEmpty() ? QString() : '/' + FileName), FilterName);
	if (!Path.isEmpty())
	{
		PROPERTY_SET("PathLastDirectory", QFileInfo(Path).dir().path());
	}
	return Path;
}
//-----------------------------------------------------------------------------
QString ISFileDialog::GetDirectoryPath(QWidget *parent)
{
	QString Path = ISFileDialog::getExistingDirectory(parent, LANG("FileDialog.Directory.Title"), PROPERTY_GET("PathLastDirectory").toString(), ISFileDialog::ShowDirsOnly);
	if (!Path.isEmpty())
	{
		PROPERTY_SET("PathLastDirectory", QFileInfo(Path).dir().path());
	}
	return Path;
}
//-----------------------------------------------------------------------------
QStringList ISFileDialog::GetOpenFileNames(QWidget *parent, const QString &FilterName)
{
	QStringList StringList = ISFileDialog::getOpenFileNames(parent, LANG("FileDialog.Files.Open.Title"), PROPERTY_GET("PathLastDirectory").toString(), FilterName);
	if (!StringList.isEmpty())
	{
		PROPERTY_SET("PathLastDirectory", QFileInfo(StringList.front()).dir().path());
	}
	return StringList;
}
//-----------------------------------------------------------------------------
