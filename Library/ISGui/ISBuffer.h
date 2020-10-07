#pragma once
#ifndef _ISBUFFER_H_INCLUDED
#define _ISBUFFER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISBuffer
{
public:
	ISBuffer(const ISBuffer &) = delete;
	ISBuffer(ISBuffer &&) = delete;
	ISBuffer &operator=(const ISBuffer &) = delete;
	ISBuffer &operator=(ISBuffer &&) = delete;
	~ISBuffer();

	static ISBuffer& GetInstance();

	void Initialize(); //Инициализация

	QMovie* GetAnimation(const QString &AnimationName, QObject *parent, const QString &SourceFile, int FileLine); //Получить указатель на анимацию
	QIcon GetIcon(const QString &IconName, const QString &SourceFile, int FileLine); //Получить иконку
	QPixmap GetPixmap(const QString &PixmapName, const QString &SourceFile, int FileLine); //Получить изображение
	QString GetAudio(const QString &AudioName); //Получить аудио
	QString GetStyle(const QString &StyleName, const QString &SourceFile, int FileLine) const; //Получить стиль

private:
	void InitializeAnimations(); //Инициализация анимаций
	void InitializeIcons(); //Инициализация иконок
	void InitializePixmaps(); //Инициализация изображений
	void InitializeAudios(); //Инициализация аудио ресурсов
	void InitializeStyleSheets(); //Инициализация CSS-стилей

	void AddAnimations(const QString &AnimationName, const QString &AnimationPath); //Добавление анимации в буфер
	void AddImageIcon(const QString &IconName, const QString &IconPath); //Добавление иконки в буфер
	void AddImage(const QString &PixmapName, const QString &PixmapPath); //Добавление изображения в буфер
	void AddAudio(const QString &AudioName, const QString &AudioPath); //Добавление аудио файла в буфер
	void AddStyle(const QString &FileName, const QString &FilePath); //Добавление стиля в буфер

private:
	ISBuffer();

	std::map<QString, QString> Animations; //Анимации
	std::map<QString, QIcon> Icons; //Иконки
	std::map<QString, QPixmap> Pixmaps; //Изображения
	std::map<QString, QString> Audios; //Звуки
	std::map<QString, QString> StyleSheets; //CSS-стили
};
//-----------------------------------------------------------------------------
#define BUFFER_ANIMATION(ANIMATION_NAME, PARENT) ISBuffer::GetInstance().GetAnimation(ANIMATION_NAME, PARENT, __FILE__, __LINE__)
#define BUFFER_ICONS(ICON_NAME) ISBuffer::GetInstance().GetIcon(ICON_NAME, __FILE__, __LINE__)
#define BUFFER_PIXMAPS(PIXMAP_NAME) ISBuffer::GetInstance().GetPixmap(PIXMAP_NAME, __FILE__, __LINE__)
#define BUFFER_AUDIO(AUDIO_NAME) ISBuffer::GetInstance().GetAudio(AUDIO_NAME)
#define BUFFER_STYLE_SHEET(STYLE_NAME) ISBuffer::GetInstance().GetStyle(STYLE_NAME, __FILE__, __LINE__)
//-----------------------------------------------------------------------------
#endif
