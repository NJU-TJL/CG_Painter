#include "pixelset.h"
#include "algorithm.h"

void PixelSet::paint(QImage *image) const
{
	int x = image->size().width();
	int y = image->size().height();
	foreach(const Point& var, points) {
		if (var.x >= x || var.y >= y || var.x < 0 || var.y < 0) continue; //跳过超过画布边界的点
		image->setPixelColor(var.x, var.y, color);
	}
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

void Line::clip(int xmin, int ymin, int xmax, int ymax, string algorithm)
{
	if (algorithm == "Cohen-Sutherland") {
		Cohen_Sutherland(x1, y1, x2, y2, xmin, ymin, xmax, ymax);
		if (x1 == -1)
			points.clear();
		else
			refresh();
	}
	else if (algorithm == "Liang-Barsky") {
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
