#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISServiceButton.h"
#include "ISSliderDuration.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISPlayerWidget : public QWidget
{
	Q_OBJECT

public:
	ISPlayerWidget(const QString &FilePath, bool auto_play = false, QWidget *parent = 0);
	virtual ~ISPlayerWidget();

	void SetFilePlay(const QString &FilePath);
	void Play();
	void Pause();
	void Stop();
	void Previous();
	void Next();

protected:
	void CreateInterface();
	
	void VolumeClicked();
	void SliderValueChanged(int Value);
	void SliderPress();
	void SliderMoved(int Position);
	void SliderRelease();

	void MediaDurationChanged(qint64 Duration);
	void MediaError(QMediaPlayer::Error NewError);
	void MediaStatusChanged(QMediaPlayer::MediaStatus NewStatus);
	void MediaPositionChanged(qint64 Position);
	void MediaStateChanged(QMediaPlayer::State NewState);

	void UpdatePreviousNextEnable();

private:
	QMediaPlayer *MediaPlayer;
	bool AutoPlay;

	ISServiceButton *ButtonPlay;
	ISServiceButton *ButtonPause;
	ISServiceButton *ButtonStop;
	QLabel *LabelCurrentDuration;
	ISSliderDuration *SliderDuration;
	QLabel *LabelDuration;

	ISServiceButton *ButtonPrevious;
	ISServiceButton *ButtonNext;
	ISServiceButton *ButtonVolume;
	QSlider *SliderVolume;

	QLabel *LabelErrorIcon;
	QLabel *LabelErrorText;

	bool VolumeOn;
	int LastVolume;
	QPropertyAnimation *PropertyAnimation;
};
//-----------------------------------------------------------------------------
