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
	ISAudioPlayerForm(const QString &FilePath, QWidget *parent = 0);
	virtual ~ISAudioPlayerForm();

private:
	void PlayClicked();
	void PauseClicked();
	void StopClicked();
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
	QSlider *SliderPosition;
	QSlider *SliderVolume;

	QMediaPlayer *MediaPlayer;
};
//-----------------------------------------------------------------------------
#endif
