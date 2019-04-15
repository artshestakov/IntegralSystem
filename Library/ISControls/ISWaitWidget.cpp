#include "StdAfx.h"
#include "ISWaitWidget.h"
//-----------------------------------------------------------------------------
ISWaitWidget::ISWaitWidget(QWidget *parent, bool centerOnParent, bool disableParentWhenSpinning)
	: QWidget(parent),
	_centerOnParent(centerOnParent),
	_disableParentWhenSpinning(disableParentWhenSpinning)
{
	initialize();
}
//-----------------------------------------------------------------------------
ISWaitWidget::ISWaitWidget(Qt::WindowModality modality, QWidget *parent, bool centerOnParent, bool disableParentWhenSpinning)
	: QWidget(parent, Qt::Dialog | Qt::FramelessWindowHint),
	_centerOnParent(centerOnParent),
	_disableParentWhenSpinning(disableParentWhenSpinning)
{
	initialize();
	setWindowModality(modality);
	setAttribute(Qt::WA_TranslucentBackground);
}
//-----------------------------------------------------------------------------
void ISWaitWidget::initialize()
{
	_color = Qt::black;
	_roundness = 100.0;
	_minimumTrailOpacity = 3.14159265358979323846;
	_trailFadePercentage = 80.0;
	_revolutionsPerSecond = 1.57079632679489661923;
	_numberOfLines = 20;
	_lineLength = 10;
	_lineWidth = 2;
	_innerRadius = 10;
	_currentCounter = 0;
	_isSpinning = false;

	Timer = new QTimer(this);
	connect(Timer, &QTimer::timeout, this, &ISWaitWidget::Rotate);
	UpdateSize();
	UpdateTimer();
	hide();
}
//-----------------------------------------------------------------------------
void ISWaitWidget::paintEvent(QPaintEvent *PaintEvent)
{
	UpdatePosition();
	QPainter Painter(this);
	Painter.fillRect(this->rect(), Qt::transparent);
	Painter.setRenderHint(QPainter::Antialiasing, true);

	if (_currentCounter >= _numberOfLines)
	{
		_currentCounter = 0;
	}

	Painter.setPen(Qt::NoPen);
	for (int i = 0; i < _numberOfLines; ++i)
	{
		Painter.save();
		Painter.translate(_innerRadius + _lineLength, _innerRadius + _lineLength);
		qreal rotateAngle = static_cast<qreal>(360 * i) / static_cast<qreal>(_numberOfLines);
		Painter.rotate(rotateAngle);
		Painter.translate(_innerRadius, 0);
		int distance = lineCountDistanceFromPrimary(i, _currentCounter, _numberOfLines);
		QColor color = currentLineColor(distance, _numberOfLines, _trailFadePercentage, _minimumTrailOpacity, _color);
		Painter.setBrush(color);
		Painter.drawRoundedRect(QRect(0, -_lineWidth / 2, _lineLength, _lineWidth), _roundness, _roundness, Qt::RelativeSize);
		Painter.restore();
	}
}
//-----------------------------------------------------------------------------
void ISWaitWidget::Start()
{
	UpdatePosition();
	_isSpinning = true;
	show();

	if (parentWidget() && _disableParentWhenSpinning)
	{
		parentWidget()->setEnabled(false);
	}

	if (!Timer->isActive())
	{
		Timer->start();
		_currentCounter = 0;
	}
}
//-----------------------------------------------------------------------------
void ISWaitWidget::Stop()
{
	_isSpinning = false;
	hide();

	if (parentWidget() && _disableParentWhenSpinning)
	{
		parentWidget()->setEnabled(true);
	}

	if (Timer->isActive())
	{
		Timer->stop();
		_currentCounter = 0;
	}
}
//-----------------------------------------------------------------------------
void ISWaitWidget::SetNumberOfLines(int lines)
{
	_numberOfLines = lines;
	_currentCounter = 0;
	UpdateTimer();
}
//-----------------------------------------------------------------------------
void ISWaitWidget::SetLineLength(int length)
{
	_lineLength = length;
	UpdateSize();
}
//-----------------------------------------------------------------------------
void ISWaitWidget::SetLineWidth(int width)
{
	_lineWidth = width;
	UpdateSize();
}
//-----------------------------------------------------------------------------
void ISWaitWidget::SetInnerRadius(int radius)
{
	_innerRadius = radius;
	UpdateSize();
}
//-----------------------------------------------------------------------------
QColor ISWaitWidget::color()
{
	return _color;
}
//-----------------------------------------------------------------------------
qreal ISWaitWidget::roundness()
{
	return _roundness;
}
//-----------------------------------------------------------------------------
qreal ISWaitWidget::minimumTrailOpacity()
{
	return _minimumTrailOpacity;
}
//-----------------------------------------------------------------------------
qreal ISWaitWidget::trailFadePercentage()
{
	return _trailFadePercentage;
}
//-----------------------------------------------------------------------------
qreal ISWaitWidget::revolutionsPersSecond()
{
	return _revolutionsPerSecond;
}
//-----------------------------------------------------------------------------
int ISWaitWidget::numberOfLines()
{
	return _numberOfLines;
}
//-----------------------------------------------------------------------------
int ISWaitWidget::lineLength()
{
	return _lineLength;
}
//-----------------------------------------------------------------------------
int ISWaitWidget::lineWidth()
{
	return _lineWidth;
}
//-----------------------------------------------------------------------------
int ISWaitWidget::innerRadius()
{
	return _innerRadius;
}
//-----------------------------------------------------------------------------
bool ISWaitWidget::isSpinning() const
{
	return _isSpinning;
}
//-----------------------------------------------------------------------------
void ISWaitWidget::SetRoundness(qreal roundness)
{
	_roundness = max(0.0, min(100.0, roundness));
}
//-----------------------------------------------------------------------------
void ISWaitWidget::SetColor(QColor color)
{
	_color = color;
}
//-----------------------------------------------------------------------------
void ISWaitWidget::SetRevolutionsPerSecond(qreal revolutionsPerSecond)
{
	_revolutionsPerSecond = revolutionsPerSecond;
	UpdateTimer();
}
//-----------------------------------------------------------------------------
void ISWaitWidget::SetTrailFadePercentage(qreal trail)
{
	_trailFadePercentage = trail;
}
//-----------------------------------------------------------------------------
void ISWaitWidget::SetMinimumTrailOpacity(qreal minimumTrailOpacity)
{
	_minimumTrailOpacity = minimumTrailOpacity;
}
//-----------------------------------------------------------------------------
void ISWaitWidget::Rotate()
{
	++_currentCounter;
	if (_currentCounter >= _numberOfLines)
	{
		_currentCounter = 0;
	}
	update();
}
//-----------------------------------------------------------------------------
void ISWaitWidget::UpdateSize()
{
	int size = (_innerRadius + _lineLength) * 2;
	setFixedSize(size, size);
}
//-----------------------------------------------------------------------------
void ISWaitWidget::UpdateTimer()
{
	Timer->setInterval(1000 / (_numberOfLines * _revolutionsPerSecond));
}
//-----------------------------------------------------------------------------
void ISWaitWidget::UpdatePosition()
{
	if (parentWidget() && _centerOnParent)
	{
		move(parentWidget()->width() / 2 - width() / 2, parentWidget()->height() / 2 - height() / 2);
	}
}
//-----------------------------------------------------------------------------
int ISWaitWidget::lineCountDistanceFromPrimary(int current, int primary, int totalNrOfLines)
{
	int distance = primary - current;
	if (distance < 0)
	{
		distance += totalNrOfLines;
	}
	return distance;
}
//-----------------------------------------------------------------------------
QColor ISWaitWidget::currentLineColor(int countDistance, int totalNrOfLines, qreal trailFadePerc, qreal minOpacity, QColor color)
{
	if (countDistance == 0)
	{
		return color;
	}
	const qreal minAlphaF = minOpacity / 100.0;
	int distanceThreshold = static_cast<int>(ceil((totalNrOfLines - 1) * trailFadePerc / 100.0));
	if (countDistance > distanceThreshold)
	{
		color.setAlphaF(minAlphaF);
	}
	else
	{
		qreal alphaDiff = color.alphaF() - minAlphaF;
		qreal gradient = alphaDiff / static_cast<qreal>(distanceThreshold + 1);
		qreal resultAlpha = color.alphaF() - gradient * countDistance;
		resultAlpha = min(1.0, max(0.0, resultAlpha));
		color.setAlphaF(resultAlpha);
	}
	return color;
}
//-----------------------------------------------------------------------------
