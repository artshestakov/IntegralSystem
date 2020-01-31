#pragma once
#ifndef _ISBUFFER_H_INCLUDED
#define _ISBUFFER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTypes.h"
//-----------------------------------------------------------------------------
class ISBuffer
{
public:
	ISBuffer(const ISBuffer &) = delete;
	ISBuffer(ISBuffer &&) = delete;
	ISBuffer &operator=(const ISBuffer &) = delete;
	ISBuffer &operator=(ISBuffer &&) = delete;
	~ISBuffer();

	static ISBuffer& GetInstance();

	void Initialize();

	QMovie* GetAnimation(const QString &AnimationName, QObject *parent, const QString &SourceFile, int FileLine);
	QIcon GetIcon(const QString &IconName, const QString &SourceFile, int FileLine);
	QPixmap GetPixmap(const QString &PixmapName, const QString &SourceFile, int FileLine);
	QString GetAudio(const QString &AudioName);

protected:
	void InitializeAnimations(); //������������� ��������
	void InitializeIcons(); //������������� ������
	void InitializePixmaps(); //������������� �����������
	void InitializeAudios(); //������������� ����� ��������

	void AddAnimations(const QString &AnimationName, const QString &AnimationPath); //���������� �������� � �����
	void AddIcon(const QString &IconName, const QString &IconPath); //���������� ������ � �����
	void AddPixmap(const QString &PixmapName, const QString &PixmapPath); //���������� ����������� � �����
	void AddAudio(const QString &AudioName, const QString &AudioPath); //���������� ����� ����� � �����

private:
	ISBuffer();

	std::map<QString, QMovie *> Animations;
	std::map<QString, QIcon> Icons;
	std::map<QString, QPixmap> Pixmaps;
	QStringMap Audios;
};
//-----------------------------------------------------------------------------
#define BUFFER_ANIMATION(ANIMATION_NAME, PARENT) ISBuffer::GetInstance().GetAnimation(ANIMATION_NAME, PARENT, __FILE__, __LINE__)
#define BUFFER_ICONS(ICON_NAME) ISBuffer::GetInstance().GetIcon(ICON_NAME, __FILE__, __LINE__)
#define BUFFER_PIXMAPS(PIXMAP_NAME) ISBuffer::GetInstance().GetPixmap(PIXMAP_NAME, __FILE__, __LINE__)
#define BUFFER_AUDIO(AUDIO_NAME) ISBuffer::GetInstance().GetAudio(AUDIO_NAME)
//-----------------------------------------------------------------------------
#endif
