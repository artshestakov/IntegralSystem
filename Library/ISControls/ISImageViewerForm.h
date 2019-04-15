#pragma once
//-----------------------------------------------------------------------------
#include "iscontrols_global.h"
#include "ISInterfaceForm.h"
#include "ISScrollArea.h"
//-----------------------------------------------------------------------------
class ISCONTROLS_EXPORT ISImageViewerForm : public ISInterfaceForm
{
	Q_OBJECT

public:
	ISImageViewerForm(const QPixmap &Pixmap, QWidget *parent = 0);
	ISImageViewerForm(const QString &FilePath, QWidget *parent = 0);
	virtual ~ISImageViewerForm();

protected:
	void keyPressEvent(QKeyEvent *e);
	void keyReleaseEvent(QKeyEvent *e);
	void wheelEvent(QWheelEvent *e);
	void EscapeClicked() override;

	void Print();
	void Copy();
	void ZoomIn();
	void ZoomOut();
	void NormalSize();
	void FitToWindow();

	void UpdateActions();
	void SetPixmap(const QPixmap &NewPixmap);
	void ScaleImage(double Factor);
	void AdjustScrollBar(QScrollBar *ScrollBar, double Factor);

private:
	QAction *ActionPrint;
	QAction *ActionCopy;
	QAction *ActionZoomIn;
	QAction *ActionZoomOut;
	QAction *ActionNormalSize;
	QAction *ActionFitToWindow;
	QLabel *LabelImage;
	ISScrollArea *ScrollArea;
	QLabel *LabelSize;

	double ScaleFactor;
};
//-----------------------------------------------------------------------------
