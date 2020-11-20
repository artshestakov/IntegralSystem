#pragma once
#ifndef _ISAUDIOPLAYERFORM_H_INCLUDED
#define _ISAUDIOPLAYERFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISInterfaceForm.h"
#include "ISButtons.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISAudioPlayerForm : public ISInterfaceForm
{
	Q_OBJECT

public:
	static ISAudioPlayerForm& Instance();

	void SetMedia(const QString &FilePath);
	void SetMedia(const QByteArray &ByteArray);

protected:
	void closeEvent(QCloseEvent *CloseEvent);

private:
	ISAudioPlayerForm();
	virtual ~ISAudioPlayerForm();
	ISAudioPlayerForm(ISAudioPlayerForm const &) {};
	ISAudioPlayerForm& operator=(ISAudioPlayerForm const&) { return *this; };

private:
	void PlayClicked();
	void PauseClicked();
	void StopClicked();
	void PosBackClicked();
	void PosForwardClicked();
	void MuteClicked();
	void SliderVolumeChanged(int Volume);

private:
	void DurationChanged(qint64 Duration);
	void Error(QMediaPlayer::Error Error);
	void MediaStatusChanged(QMediaPlayer::MediaStatus MediaStatus);
	void MutedChanged(bool Muted);
	void PositionChanged(qint64 Position);
	void StateChanged(QMediaPlayer::State State);

private:
	ISServiceButton *ButtonPlay;
	ISServiceButton *ButtonPause;
	ISServiceButton *ButtonStop;
	ISServiceButton *ButtonPosBack;
	ISServiceButton *ButtonPosForward;
	ISServiceButton *ButtonMute;
	QLabel *LabelPosition;
	QSlider *SliderPosition;
	QSlider *SliderVolume;

	QMediaPlayer *MediaPlayer;
	QByteArray *BytePointer;
	QBuffer *Buffer;
	QString TimeDuration;
};
//-----------------------------------------------------------------------------
#endif
