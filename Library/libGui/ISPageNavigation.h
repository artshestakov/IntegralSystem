#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISServiceButton.h"
#include "ISPushButton.h"
//-----------------------------------------------------------------------------
class ISPageNavigation : public QWidget
{
	Q_OBJECT

signals:
	void Begin();
	void Previous();
	void Next();
	void End();

	void OffsetSignal(int offset);
	void Update();

public:
	ISPageNavigation(QWidget *parent = 0);
	virtual ~ISPageNavigation();

	void SetRowCount(int row_count);
	void SetLimit(int limit);

	void BeginClicked();
	void PreviousClicked();
	void Select();
	void NextClicked();
	void EndClicked();

	void PageChanged();

private:
	ISServiceButton *ButtonBegin;
	ISServiceButton *ButtonPrevious;
	ISPushButton *ButtonSelect;
	ISServiceButton *ButtonNext;
	ISServiceButton *ButtonEnd;

	int RowCount;
	int PageCount;
	int CurrentPage;
	int Limit;
	int Offset;
};
//-----------------------------------------------------------------------------
