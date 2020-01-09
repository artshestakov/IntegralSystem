#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISWaitWidget : public QWidget
{
	Q_OBJECT

public:
    ISWaitWidget(QWidget *parent = 0, bool centerOnParent = true, bool disableParentWhenSpinning = true);
    ISWaitWidget(Qt::WindowModality modality, QWidget *parent = 0, bool centerOnParent = true, bool disableParentWhenSpinning = true);

public slots:
    void Start();
    void Stop();

public:
    void SetColor(QColor color);
    void SetRoundness(qreal roundness);
    void SetMinimumTrailOpacity(qreal minimumTrailOpacity);
    void SetTrailFadePercentage(qreal trail);
    void SetRevolutionsPerSecond(qreal revolutionsPerSecond);
    void SetNumberOfLines(int lines);
    void SetLineLength(int length);
    void SetLineWidth(int width);
    void SetInnerRadius(int radius);

    QColor color();
    qreal roundness();
    qreal minimumTrailOpacity();
    qreal trailFadePercentage();
    qreal revolutionsPersSecond();
    int numberOfLines();
    int lineLength();
    int lineWidth();
    int innerRadius();

    bool isSpinning() const;

private slots:
    void Rotate();

protected:
    void paintEvent(QPaintEvent *PaintEvent);

private:
    static int lineCountDistanceFromPrimary(int current, int primary, int totalNrOfLines);
    static QColor currentLineColor(int distance, int totalNrOfLines, qreal trailFadePerc, qreal minOpacity, QColor color);

    void initialize();
    void UpdateSize();
    void UpdateTimer();
    void UpdatePosition();

private:
    QColor  _color;
    qreal   _roundness; // 0..100
    qreal   _minimumTrailOpacity;
    qreal   _trailFadePercentage;
    qreal   _revolutionsPerSecond;
    int     _numberOfLines;
    int     _lineLength;
    int     _lineWidth;
    int     _innerRadius;

private:
    ISWaitWidget(const ISWaitWidget&);
    ISWaitWidget& operator=(const ISWaitWidget&);

    QTimer *Timer;
    bool _centerOnParent;
    bool _disableParentWhenSpinning;
    int _currentCounter;
    bool _isSpinning;
};
//-----------------------------------------------------------------------------
