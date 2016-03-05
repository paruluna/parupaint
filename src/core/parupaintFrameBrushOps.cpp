#include "parupaintFrameBrushOps.h"

#include "parupaintFrame.h"
#include "parupaintLayer.h"
#include "parupaintPanvas.h"
#include "parupaintBrush.h"

#include <QtMath>
#include <QDebug>
#include <QBitmap>


// Lifted from:
// src/gui/painting/qbrush.cpp
static uchar patterns[][8] = {
	{0xaa, 0x44, 0xaa, 0x11, 0xaa, 0x44, 0xaa, 0x11}, // Dense5Pattern
	{0x00, 0x11, 0x00, 0x44, 0x00, 0x11, 0x00, 0x44}, // Dense6Pattern (modified to interweave Dense5Pattern)
	{0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81}, // DiagCrossPattern
	{0xFF, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80}  // Custom checker pattern
};

// QBitmap (inherits QPixmap) does not work on non-gui builds.
// Qt segfaults (???) if you attempt to do it anyways...
// so we end up using QImage instead as a brush texture.
inline QImage customPatternToImage(int pattern, const QColor & col = QColor(Qt::black))
{
	QImage img(QSize(8, 8), QImage::Format_MonoLSB);
	img.setColor(0, QColor(Qt::transparent).rgba());
	img.setColor(1, QColor(Qt::white).rgba());
	if(col.alpha() != 0) img.setColor(1, col.rgba());

	for(int y = 0; y < 8; ++y){
		memcpy(img.scanLine(y), patterns[pattern] + y, 1);
	}
	return img;
}


QRect ParupaintFrameBrushOps::stroke(ParupaintPanvas * panvas, ParupaintBrush * brush, const QPointF & pos, const QPointF & old_pos, const qreal s1)
{
	return ParupaintFrameBrushOps::stroke(panvas, brush, QLineF(old_pos, pos), s1);
}
QRect ParupaintFrameBrushOps::stroke(ParupaintPanvas * panvas, ParupaintBrush * brush, const QPointF & pos, const qreal s1)
{
	return ParupaintFrameBrushOps::stroke(panvas, brush, QLineF(pos, pos), s1);
}
QRect ParupaintFrameBrushOps::stroke(ParupaintPanvas * panvas, ParupaintBrush * brush, const QLineF & line, const qreal s1)
{
	ParupaintLayer * layer = panvas->layerAt(brush->layer());
	if(!layer) return QRect();

	ParupaintFrame * frame = layer->frameAt(brush->frame());
	if(!frame) return QRect();

	qreal size = brush->pressureSize();
	QColor color = brush->color();

	QPointF op = line.p1(), np = line.p2();
	// do not move this block
	if(size <= 1){
		size = 1;
		// pixel brush? pixel position.
		op = QPointF(qFloor(op.x()), qFloor(op.y()));
		np = QPointF(qFloor(np.x()), qFloor(np.y()));
	}
	if(brush->tool() == ParupaintBrushToolTypes::BrushToolOpacityDrawing){
		size = brush->size();
	}

	QRect urect(op.toPoint() + QPoint(-size/2, -size/2), QSize(size, size));
	urect |= QRect(np.toPoint() + QPoint(-size/2, -size/2), QSize(size, size));

	QPen pen;
	QBrush pen_brush(color);
	pen.setCapStyle(Qt::RoundCap);
	pen.setWidthF(size);
	pen.setMiterLimit(size);

	if(brush->tool() != ParupaintBrushToolTypes::BrushToolLine){
		pen.setMiterLimit((s1 > -1) ? s1 : size);
	}

	switch(brush->tool()){
		case ParupaintBrushToolTypes::BrushToolDotShadingPattern: {
			pen_brush.setTextureImage(customPatternToImage(0, color));
			break;
		}
		case ParupaintBrushToolTypes::BrushToolDotHighlightPattern: {
			pen_brush.setTextureImage(customPatternToImage(1, color));
			break;
		}
		case ParupaintBrushToolTypes::BrushToolCrossPattern: {
			pen_brush.setTextureImage(customPatternToImage(2, color));
			break;
		}
		case ParupaintBrushToolTypes::BrushToolGrid: {
			pen_brush.setTextureImage(customPatternToImage(3, color));
			break;
		}
	}
	pen.setBrush(pen_brush);

	switch(brush->tool()){
		case ParupaintBrushToolTypes::BrushToolFloodFill: {
			urect = frame->drawFill(np, color);
			break;
		}
		// intentional fallthrough
		case ParupaintBrushToolTypes::BrushToolOpacityDrawing: {

			color.setAlpha(brush->pressure() * color.alpha());
			if(color.alpha() == 0) color.setAlpha(1);

			pen_brush.setColor(color);
			pen.setBrush(pen_brush);
			pen.setColor(color);
		}
		default: {
			frame->drawLine(QLineF(op, np), pen);
		}
	}
	return urect.adjusted(-1, -1, 1, 1);
}
