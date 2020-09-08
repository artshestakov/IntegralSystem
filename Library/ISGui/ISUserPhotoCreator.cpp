#include "ISUserPhotoCreator.h"
#include "ISDefinesGui.h"
#include "ISRandom.h"
#include "ISSystem.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISUserPhotoCreator::ISUserPhotoCreator()
{
	std::vector<QColor> VectorColor =
	{
		QColor(255, 127, 127),
		QColor(160, 160, 160),
		QColor(255, 178, 127),
		QColor(255, 233, 127),
		QColor(218, 255, 127),
		QColor(165, 255, 127),
		QColor(127, 255, 142), 
		QColor(127, 255, 197),
		QColor(127, 255, 255),
		QColor(127, 201, 255),
		QColor(127, 146, 255),
		QColor(161, 127, 255),
		QColor(214, 127, 255),
		QColor(255, 127, 237),
		QColor(255, 127, 182)
	};
	Color = VectorColor[ISRandom().Get(0, VectorColor.size() - 1)];
}
//-----------------------------------------------------------------------------
ISUserPhotoCreator::~ISUserPhotoCreator()
{

}
//-----------------------------------------------------------------------------
QPixmap ISUserPhotoCreator::Create(const QString &FIO)
{
	QString Fio = FIO;
	if (!Fio.isEmpty())
	{
		ISSystem::RemoveBeginSymbolLoop(Fio, SYMBOL_SPACE);
		ISSystem::RemoveLastSymbolLoop(Fio, SYMBOL_SPACE);
		QStringList StringList = Fio.split(SYMBOL_SPACE);
		for (int i = 0, c = StringList.size(); i < c; ++i)
		{
			QString String = StringList[i];
			ISSystem::RemoveBeginSymbolLoop(String, SYMBOL_SPACE);
			ISSystem::RemoveLastSymbolLoop(String, SYMBOL_SPACE);
			StringList[i] = String;
		}
		StringList.removeAll(QString());

		if (StringList.size() > 0)
		{
			Fio = StringList[0][0];
		}
		if (StringList.size() > 1)
		{
			Fio.push_back(StringList[1][0]);
		}
	}

	QPixmap Pixmap(204, 204);
	Pixmap.fill(Qt::transparent);
	if (!Fio.isEmpty())
	{
		QPainter Painter(&Pixmap);
		Painter.setPen(QPen(Color, 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
		Painter.setBrush(QBrush(Color));
		Painter.drawEllipse(2, 2, 200, 200);
		Painter.setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
		Painter.setBrush(QBrush(Qt::black));
		Painter.setFont(QFont("Tahoma", 60));
		Painter.drawText(Pixmap.rect(), Qt::AlignCenter, Fio.toUpper());
	}
	return Pixmap;
}
//-----------------------------------------------------------------------------
