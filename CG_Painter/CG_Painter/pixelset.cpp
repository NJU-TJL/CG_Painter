#include "pixelset.h"
#include "algorithm.h"
#include "canvas.h"

#include <QDebug>
#include <QPainter> 
#include<cassert>

void PixelSet::paint(QImage *image)
{
	int x = image->size().width();
	int y = image->size().height();
	foreach(const Point& var, points) {
		if (var.x >= x || var.y >= y || var.x < 0 || var.y < 0) continue; //跳过超过画布边界的点
		image->setPixelColor(var.x, var.y, color);
	}
}

int PixelSet::getID(int x, int y)
{
	int res_ID = -1;
	for (size_t j = 0; j < points.size(); j++) {
		int ix = points[j].x;
		int iy = points[j].y;
		if ((ix - x)*(ix - x) + (iy - y)*(iy - y) <= CLICK_BIAS * CLICK_BIAS) {
			return id;
		}
	}
	return res_ID;
}

void PixelSet::translate(int dx, int dy)
{
	for (auto var = points.begin(); var != points.end(); ++var) {
		var->x += dx;
		var->y += dy;
	}
}

void Line::refresh()
{
	points.clear();
	if (algorithm == "DDA") {
		drawLine_DDA(x1, y1, x2, y2, *this);
	}
	else if (algorithm == "Bresenham") {
		drawLine_Bresenham(x1, y1, x2, y2, *this);
	}
}

void Line::rotate(int x, int y, int r)
{
	rotatePoint(x1, y1, x, y, r);
	rotatePoint(x2, y2, x, y, r);
	refresh();
}

void Line::scale(int x, int y, float s)
{
	scalePoint(x1, y1, x, y, s);
	scalePoint(x2, y2, x, y, s);
	refresh();
}

void Line::clip(int xmin, int ymin, int xmax, int ymax, ALGORITHM algorithm)
{
	if (algorithm == COHEN) {
		Cohen_Sutherland(x1, y1, x2, y2, xmin, ymin, xmax, ymax);
		if (x1 == -1)
			points.clear();
		else
			refresh();
	}
	else if (algorithm == LIANG) {
		if (Liang_Barsky(x1, y1, x2, y2, xmin, ymin, xmax, ymax))
			refresh();
		else
			points.clear();
	}
}

void Polygon::refresh()
{
	points.clear();
	if (algorithm == "DDA") {
		drawPolygon_DDA(vertexs, *this);
	}
	else if (algorithm == "Bresenham") {
		drawPolygon_Bresenham(vertexs, *this);
	}
}

void Polygon::rotate(int x, int y, int r)
{
	size_t n = vertexs.size();
	for (size_t i = 0; i < n; i++) {
		rotatePoint(vertexs[i].x, vertexs[i].y, x, y, r);
	}
	refresh();
}

void Polygon::scale(int x, int y, float s)
{
	size_t n = vertexs.size();
	for (size_t i = 0; i < n; i++) {
		scalePoint(vertexs[i].x, vertexs[i].y, x, y, s);
	}
	refresh();
}

void Ellipse::refresh()
{
	points.clear();
	drawEllipse(x, y, rx, ry, *this);
	rotate(r);
}

void Ellipse::rotate(int ix, int iy, int ir)
{
	r += ir;
	rotatePoint(x, y, ix, iy, ir);
	refresh();
}

void Ellipse::rotate(int ir)
{
	size_t n = points.size();
	for (size_t i = 0; i < n; i++) {
		rotatePoint(points[i].x, points[i].y, x, y, ir);
	}
}

void Ellipse::scale(int ix, int iy, float s)
{
	scalePoint(x, y, ix, iy, s);
	rx = qRound(rx*s);
	ry = qRound(ry*s);
	refresh();
}

void DotPoint::paint(QImage * image)
{
	QPainter myPainter(image);
	QPen myPen(color);
	myPen.setWidth(width);
	myPen.setCapStyle(Qt::RoundCap);
	myPainter.setPen(myPen);
	myPainter.drawPoint(x, y);
}

void Rectangle::paint(QImage * image)
{
	QPainter myPainter(image);
	QPen myPen(color);
	myPen.setWidth(width);
	myPen.setStyle(Qt::DashLine);
	myPainter.setPen(myPen);
	myPainter.drawRect(x1, y1, x2 - x1, y2 - y1);
}

Curve::Curve(const Curve & B, Canvas & canvas) : PixelSet(B) {
	vertexs = B.vertexs;
	algorithm = B.algorithm;
	foldline = (FoldLine*)(canvas.getPixelSet(B.foldline->id));
	assert(foldline != nullptr);
}
void Curve::refresh()
{
	points.clear();
	if (algorithm == BEZIER) {
		drawCurve_Bezier(vertexs, *this);
	}
	else if (algorithm == B_SPLINE) {

	}
}

void Curve::paint(QImage * image)
{
	vertexs = foldline->vertexs;
	refresh();
	PixelSet::paint(image);
}

void FoldLine::paint(QImage * image)
{
	QPainter myPainter(image);
	QPen myPen(color);
	myPen.setWidth(width);
	myPen.setStyle(Qt::DashLine);
	myPainter.setPen(myPen);
	for (size_t i = 1; i < vertexs.size(); i++) {
		myPainter.drawLine(vertexs[i - 1].x, vertexs[i - 1].y, vertexs[i].x, vertexs[i].y);
	}
}

void FoldLine::rotate(int x, int y, int r)
{
	size_t n = vertexs.size();
	for (size_t i = 0; i < n; i++) {
		rotatePoint(vertexs[i].x, vertexs[i].y, x, y, r);
	}
}

void FoldLine::scale(int x, int y, float s)
{
	size_t n = vertexs.size();
	for (size_t i = 0; i < n; i++) {
		scalePoint(vertexs[i].x, vertexs[i].y, x, y, s);
	}
}

CtrlPoint::CtrlPoint(const CtrlPoint & B, Canvas & canvas) :PixelSet(B) {
	type = CTRLPOINT;
	width = B.width;
	index = B.index;
	clear_flag = B.clear_flag;
	if (clear_flag) return;
	foldline = (FoldLine*)(canvas.getPixelSet(B.foldline->id));
	assert(foldline != nullptr);
}

int CtrlPoint::getID(int x, int y)
{
	if (clear_flag) return -1;
	int ix = foldline->vertexs[index].x;
	int iy = foldline->vertexs[index].y;
	if ((ix - x)*(ix - x) + (iy - y)*(iy - y) <= ((width + 2) * (width + 2))/4) {
		return id;
	}
	else {
		return -1;
	}
}

void CtrlPoint::translate(int dx, int dy)
{
	if (clear_flag) return;
	foldline->vertexs[index].x+=dx;
	foldline->vertexs[index].y+=dy;
}

void CtrlPoint::paint(QImage * image)
{
	if (clear_flag) return;
	if (foldline->clear_flag) {
		clear_flag = true;
		return;
	}
	int x = foldline->vertexs[index].x;
	int y = foldline->vertexs[index].y;
	QPainter myPainter(image);
	QPen myPen(color);
	myPen.setWidth(width);
	myPen.setCapStyle(Qt::RoundCap);
	myPainter.setPen(myPen);
	myPainter.drawPoint(x, y);
}
