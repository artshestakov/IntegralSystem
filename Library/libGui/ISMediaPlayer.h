#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISMediaPlayer : public QMediaPlayer
{
	Q_OBJECT

public:
	ISMediaPlayer(const ISMediaPlayer &) = delete;
	ISMediaPlayer(ISMediaPlayer &&) = delete;
	ISMediaPlayer &operator=(const ISMediaPlayer &) = delete;
	ISMediaPlayer &operator=(ISMediaPlayer &&) = delete;
	~ISMediaPlayer();

	static ISMediaPlayer& GetInstance();

	void Play(const QString &BufferAudio);

protected:
	void StateChanged(QMediaPlayer::State NewState);

private:
	ISMediaPlayer();

	ISVectorString Queue;
};
//-----------------------------------------------------------------------------
