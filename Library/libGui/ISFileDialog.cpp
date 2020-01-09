#include "StdAfx.h"
#include "ISFileDialog.h"
#include "ISLocalization.h"
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
	FilterString += LOCALIZATION("File.Filter.Images") + ";;";
	FilterString += LOCALIZATION("File.Filter.Image.PNG") + ";;";
	FilterString += LOCALIZATION("File.Filter.Image.JPEG") + ";;";
	FilterString += LOCALIZATION("File.Filter.Image.JPG") + ";;";
	FilterString += LOCALIZATION("File.Filter.Image.DDS") + ";;";
	FilterString += LOCALIZATION("File.Filter.Image.GIF") + ";;";
	FilterString += LOCALIZATION("File.Filter.Image.ICNS") + ";;";
	FilterString += LOCALIZATION("File.Filter.Image.ICO") + ";;";
	FilterString += LOCALIZATION("File.Filter.Image.SVG") + ";;";
	FilterString += LOCALIZATION("File.Filter.Image.TGA") + ";;";
	FilterString += LOCALIZATION("File.Filter.Image.TIFF") + ";;";
	FilterString += LOCALIZATION("File.Filter.Image.WBMP") + ";;";
	FilterString += LOCALIZATION("File.Filter.Image.BMP") + ";;";
	FilterString += LOCALIZATION("File.Filter.Image.WEBP");

	return ISFileDialog::getOpenFileName(parent, LOCALIZATION("FileDialog.Image.Select.Title"), QDir::homePath(), FilterString);
}
//-----------------------------------------------------------------------------
QString ISFileDialog::GetSaveFileNameImage(QWidget *parent, const QString &FileName)
{
	QString FilterString;
	FilterString += LOCALIZATION("File.Filter.Images") + ";;";
	FilterString += LOCALIZATION("File.Filter.Image.PNG") + ";;";
	FilterString += LOCALIZATION("File.Filter.Image.JPEG") + ";;";
	FilterString += LOCALIZATION("File.Filter.Image.JPG") + ";;";
	FilterString += LOCALIZATION("File.Filter.Image.DDS") + ";;";
	FilterString += LOCALIZATION("File.Filter.Image.GIF") + ";;";
	FilterString += LOCALIZATION("File.Filter.Image.ICNS") + ";;";
	FilterString += LOCALIZATION("File.Filter.Image.ICO") + ";;";
	FilterString += LOCALIZATION("File.Filter.Image.SVG") + ";;";
	FilterString += LOCALIZATION("File.Filter.Image.TGA") + ";;";
	FilterString += LOCALIZATION("File.Filter.Image.TIFF") + ";;";
	FilterString += LOCALIZATION("File.Filter.Image.WBMP") + ";;";
	FilterString += LOCALIZATION("File.Filter.Image.BMP") + ";;";
	FilterString += LOCALIZATION("File.Filter.Image.WEBP");

	return ISFileDialog::getSaveFileName(parent, LOCALIZATION("FileDialog.Image.Save.Title"), QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/" + FileName, FilterString);
}
//-----------------------------------------------------------------------------
QString ISFileDialog::GetSaveFileNameAsteriskRecord(QWidget *parent, const QString &FileName)
{
	return ISFileDialog::getSaveFileName(parent, LOCALIZATION("FileDialog.AsteriskRecord.Save.Title"), QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/" + FileName, LOCALIZATION("File.Filter.AsteriskRecord"));
}
//-----------------------------------------------------------------------------
QString ISFileDialog::GetOpenFileName(QWidget *parent, const QString &FilePath, const QString &FilterName)
{
	QString Dir;

	if (FilePath.length())
	{
		Dir = FilePath;
	}
	else
	{
		Dir = QDir::homePath();
	}

	return ISFileDialog::getOpenFileName(parent, LOCALIZATION("FileDialog.File.Open.Title"), Dir, FilterName);
}
//-----------------------------------------------------------------------------
QString ISFileDialog::GetSaveFileName(QWidget *parent, const QString &FilterName, const QString &FileName)
{
	return ISFileDialog::getSaveFileName(parent, LOCALIZATION("FileDialog.File.Save.Title"), QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/" + FileName, FilterName);
}
//-----------------------------------------------------------------------------
QString ISFileDialog::GetDirectoryPath(QWidget *parent)
{
	return ISFileDialog::getExistingDirectory(parent, LOCALIZATION("FileDialog.Directory.Title"), QDir::homePath(), ISFileDialog::ShowDirsOnly);
}
//-----------------------------------------------------------------------------
QStringList ISFileDialog::GetOpenFilesName(QWidget *parent, const QString &FilterName)
{
	return ISFileDialog::getOpenFileNames(parent, LOCALIZATION("FileDialog.Files.Open.Title"), QDir::homePath(), FilterName);
}
//-----------------------------------------------------------------------------
