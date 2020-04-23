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

	return ISFileDialog::getOpenFileName(parent, LANG("FileDialog.Image.Select.Title"), QDir::homePath(), FilterString);
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

	return ISFileDialog::getSaveFileName(parent, LANG("FileDialog.Image.Save.Title"), QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + '/' + FileName, FilterString);
}
//-----------------------------------------------------------------------------
QString ISFileDialog::GetSaveFileNameAsteriskRecord(QWidget *parent, const QString &FileName)
{
	return ISFileDialog::getSaveFileName(parent, LANG("FileDialog.AsteriskRecord.Save.Title"), QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + '/' + FileName, LANG("File.Filter.AsteriskRecord"));
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

	return ISFileDialog::getOpenFileName(parent, LANG("FileDialog.File.Open.Title"), Dir, FilterName);
}
//-----------------------------------------------------------------------------
QString ISFileDialog::GetSaveFileName(QWidget *parent, const QString &FilterName, const QString &FileName)
{
	return ISFileDialog::getSaveFileName(parent, LANG("FileDialog.File.Save.Title"), QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + '/' + FileName, FilterName);
}
//-----------------------------------------------------------------------------
QString ISFileDialog::GetDirectoryPath(QWidget *parent)
{
	return ISFileDialog::getExistingDirectory(parent, LANG("FileDialog.Directory.Title"), QDir::homePath(), ISFileDialog::ShowDirsOnly);
}
//-----------------------------------------------------------------------------
QStringList ISFileDialog::GetOpenFilesName(QWidget *parent, const QString &FilterName)
{
	return ISFileDialog::getOpenFileNames(parent, LANG("FileDialog.Files.Open.Title"), QDir::homePath(), FilterName);
}
//-----------------------------------------------------------------------------
