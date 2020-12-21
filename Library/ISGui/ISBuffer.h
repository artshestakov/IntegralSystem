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

	void Initialize(); //�������������

	QMovie* GetAnimation(const QString &AnimationName, QObject *parent, const char *SourceFile, int FileLine); //�������� ��������� �� ��������
	QIcon GetIcon(const QString &IconName, const char *SourceFile, int FileLine); //�������� ������
	QPixmap GetPixmap(const QString &PixmapName, const char *SourceFile, int FileLine); //�������� �����������
	QString GetAudio(const QString &AudioName); //�������� �����
	QString GetStyle(const QString &StyleName, const char *SourceFile, int FileLine) const; //�������� �����
	QString GetSQL(const QString &QueryName, const char *SourceFile, int FileLine) const; //�������� ������

	ISVectorMap GetAccessTableType() const;
	void SetAccessTableType(const QVariantList &VariantList);

private:
	void InitializeAnimations(); //������������� ��������
	void InitializeIcons(); //������������� ������
	void InitializePixmaps(); //������������� �����������
	void InitializeAudios(); //������������� ����� ��������
	void InitializeStyleSheets(); //������������� CSS-������
	void InitializeSqlQueryes(); //������������� SQL-��������

	void AddAnimations(const QString &AnimationName, const QString &AnimationPath); //���������� �������� � �����
	void AddImageIcon(const QString &IconName, const QString &IconPath); //���������� ������ � �����
	void AddImage(const QString &PixmapName, const QString &PixmapPath); //���������� ����������� � �����
	void AddAudio(const QString &AudioName, const QString &AudioPath); //���������� ����� ����� � �����
	void AddStyle(const QString &FileName, const QString &FilePath); //���������� ����� � �����
	void AddSQL(const QString &FileName, const QString &FilePath); //���������� ������� � �����

private:
	ISBuffer();

	ISStringMap Animations; //��������
	std::map<QString, QIcon> Icons; //������
	std::map<QString, QPixmap> Pixmaps; //�����������
	ISStringMap Audios; //�����
	ISStringMap StyleSheets; //CSS-�����
	ISStringMap SqlQueryes; //SQL-�������

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
