#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISFieldEditBase.h"
#include "ISServiceButton.h"
//-----------------------------------------------------------------------------
class ISVolumeEdit : public ISFieldEditBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISVolumeEdit(QWidget *parent);
	virtual ~ISVolumeEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;
	void Clear() override;
	void SetVisibleClear(bool Visible) override;

protected:
	void ValueChanged() override;
	void VolumeCheck();
	void MediaStateChanged(QMediaPlayer::State NewState);

private:
	QSlider *Slider;
	QLabel *Label;
	ISServiceButton *ButtonCheck;
	
	QMediaPlayer *MediaPlayer;
};
//-----------------------------------------------------------------------------
