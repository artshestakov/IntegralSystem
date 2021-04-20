#pragma once
#ifndef _ISBUFFER_H_INCLUDED
#define _ISBUFFER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISBuffer
{
public:
    ISCurrentUserInfo CurrentUserInfo; //���������� � ������� ������������
    ISConfigurationInfo ConfigurationInfo; //���������� � ������������

public:
    static ISBuffer& Instance();

    QVariantList GetTaskPriority() const;
    void SetTaskPriority(const QVariantList &task_priority);

    void Initialize(); //�������������

    QMovie* GetAnimation(const QString &AnimationName, QObject *parent, const char *SourceFile, int FileLine); //�������� ��������� �� ��������
    QIcon GetIcon(const QString &IconName, const char *SourceFile, int FileLine); //�������� ������
    QPixmap GetPixmap(const QString &PixmapName, const char *SourceFile, int FileLine); //�������� �����������
    QString GetAudio(const QString &AudioName); //�������� �����
    QString GetStyle(const QString &StyleName, const char *SourceFile, int FileLine) const; //�������� �����

private:
    void InitializeAnimations(); //������������� ��������
    void InitializeIcons(); //������������� ������
    void InitializePixmaps(); //������������� �����������
    void InitializeAudios(); //������������� ����� ��������
    void InitializeStyleSheets(); //������������� CSS-������

    void AddAnimations(const QString &AnimationName, const QString &AnimationPath); //���������� �������� � �����
    void AddImageIcon(const QString &IconName, const QString &IconPath); //���������� ������ � �����
    void AddImage(const QString &PixmapName, const QString &PixmapPath); //���������� ����������� � �����
    void AddAudio(const QString &AudioName, const QString &AudioPath); //���������� ����� ����� � �����
    void AddStyle(const QString &FileName, const QString &FilePath); //���������� ����� � �����

private:
    ISBuffer();
    ~ISBuffer();
    ISBuffer(const ISBuffer&) = delete;
    ISBuffer(ISBuffer&&) = delete;
    ISBuffer& operator=(const ISBuffer&) = delete;
    ISBuffer& operator=(ISBuffer&&) = delete;

private:
    ISStringMap Animations; //��������
    std::map<QString, QIcon> Icons; //������
    std::map<QString, QPixmap> Pixmaps; //�����������
    ISStringMap Audios; //�����
    ISStringMap StyleSheets; //CSS-�����

    QVariantList TaskPriority; //������ ����������� �����
};
//-----------------------------------------------------------------------------
#define CURRENT_USER_ID ISBuffer::Instance().CurrentUserInfo.ID
//-----------------------------------------------------------------------------
#define BUFFER_ANIMATION(ANIMATION_NAME, PARENT) ISBuffer::Instance().GetAnimation(ANIMATION_NAME, PARENT, __FILE__, __LINE__)
#define BUFFER_ICONS(ICON_NAME) ISBuffer::Instance().GetIcon(ICON_NAME, __FILE__, __LINE__)
#define BUFFER_PIXMAPS(PIXMAP_NAME) ISBuffer::Instance().GetPixmap(PIXMAP_NAME, __FILE__, __LINE__)
#define BUFFER_AUDIO(AUDIO_NAME) ISBuffer::Instance().GetAudio(AUDIO_NAME)
#define BUFFER_STYLE_SHEET(STYLE_NAME) ISBuffer::Instance().GetStyle(STYLE_NAME, __FILE__, __LINE__)
//-----------------------------------------------------------------------------
#endif
