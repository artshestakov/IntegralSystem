#pragma once
#ifndef _ISBUFFER_H_INCLUDED
#define _ISBUFFER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISBuffer
{
public:
	ISBuffer(const ISBuffer &) = delete;
	ISBuffer(ISBuffer &&) = delete;
	ISBuffer &operator=(const ISBuffer &) = delete;
	ISBuffer &operator=(ISBuffer &&) = delete;
	~ISBuffer();

	static ISBuffer& Instance();

	void Initialize(); //Инициализация

	QMovie* GetAnimation(const QString &AnimationName, QObject *parent, const char *SourceFile, int FileLine); //Получить указатель на анимацию
	QIcon GetIcon(const QString &IconName, const char *SourceFile, int FileLine); //Получить иконку
	QPixmap GetPixmap(const QString &PixmapName, const char *SourceFile, int FileLine); //Получить изображение
	QString GetAudio(const QString &AudioName); //Получить аудио
	QString GetStyle(const QString &StyleName, const char *SourceFile, int FileLine) const; //Получить стиль
	QString GetSQL(const QString &QueryName, const char *SourceFile, int FileLine) const; //Получить запрос

	ISVectorMap GetAccessTableType() const;
	void SetAccessTableType(const QVariantList &VariantList);

private:
	void InitializeAnimations(); //Инициализация анимаций
	void InitializeIcons(); //Инициализация иконок
	void InitializePixmaps(); //Инициализация изображений
	void InitializeAudios(); //Инициализация аудио ресурсов
	void InitializeStyleSheets(); //Инициализация CSS-стилей
	void InitializeSqlQueryes(); //Инициализация SQL-запросов

	void AddAnimations(const QString &AnimationName, const QString &AnimationPath); //Добавление анимации в буфер
	void AddImageIcon(const QString &IconName, const QString &IconPath); //Добавление иконки в буфер
	void AddImage(const QString &PixmapName, const QString &PixmapPath); //Добавление изображения в буфер
	void AddAudio(const QString &AudioName, const QString &AudioPath); //Добавление аудио файла в буфер
	void AddStyle(const QString &FileName, const QString &FilePath); //Добавление стиля в буфер
	void AddSQL(const QString &FileName, const QString &FilePath); //Добавление запроса в буфер

private:
	ISBuffer();

	ISStringMap Animations; //Анимации
	std::map<QString, QIcon> Icons; //Иконки
	std::map<QString, QPixmap> Pixmaps; //Изображения
	ISStringMap Audios; //Звуки
	ISStringMap StyleSheets; //CSS-стили
	ISStringMap SqlQueryes; //SQL-запросы

	ISVectorMap AccessTableType;
};
//-----------------------------------------------------------------------------
#define BUFFER_ANIMATION(ANIMATION_NAME, PARENT) ISBuffer::Instance().GetAnimation(ANIMATION_NAME, PARENT, __FILE__, __LINE__)
#define BUFFER_ICONS(ICON_NAME) ISBuffer::Instance().GetIcon(ICON_NAME, __FILE__, __LINE__)
#define BUFFER_PIXMAPS(PIXMAP_NAME) ISBuffer::Instance().GetPixmap(PIXMAP_NAME, __FILE__, __LINE__)
#define BUFFER_AUDIO(AUDIO_NAME) ISBuffer::Instance().GetAudio(AUDIO_NAME)
#define BUFFER_STYLE_SHEET(STYLE_NAME) ISBuffer::Instance().GetStyle(STYLE_NAME, __FILE__, __LINE__)
#define BUFFER_SQL(QUERY_NAME) ISBuffer::Instance().GetSQL(QUERY_NAME, __FILE__, __LINE__)
//-----------------------------------------------------------------------------
#endif
