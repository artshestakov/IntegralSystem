#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISTypes.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISMediaPlayer : public QMediaPlayer
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

	QVectorString Queue;
};
//-----------------------------------------------------------------------------
