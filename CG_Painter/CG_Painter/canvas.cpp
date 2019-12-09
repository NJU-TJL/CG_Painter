#include "canvas.h"

#include<QDebug>
#include<fstream>
#include<cassert>
using namespace std;

Canvas::~Canvas()
{
	for (int i = 0; i < pixelsets.size(); i++) {
		delete pixelsets[i];
	}
}

Canvas::Canvas(const Canvas & B)
{
	if (this != &B) {
		color = B.color;
		pixelsets.clear();
		for (int i = 0; i < B.pixelsets.size(); i++) {
			PixelSet *p=nullptr;
			switch (B.pixelsets[i]->type)
			{
			case LINE:
				p = new Line(*((Line*)B.pixelsets[i]));
				break;
			case POLYGON:
				p = new Polygon(*((Polygon*)B.pixelsets[i]));
				break;
			case ELLIPSE:
				p = new Ellipse(*((Ellipse*)B.pixelsets[i]));
				break;
			default:
				break;
			}
			if (p != nullptr)
				pixelsets.push_back(p);
		}
	}
}

const Canvas & Canvas::operator=(const Canvas & B)
{
	if (this != &B) {
		color = B.color;
		pixelsets.clear();
		for (int i = 0; i < B.pixelsets.size(); i++) {
			//TODO:图元复制
			PixelSet *p;
			switch (B.pixelsets[i]->type)
			{
			case LINE:
				p = new Line(*((Line*)B.pixelsets[i]));
				break;
			case POLYGON:
				p = new Polygon(*((Polygon*)B.pixelsets[i]));
				break;
			case ELLIPSE:
				p = new Ellipse(*((Ellipse*)B.pixelsets[i]));
				break;
			default:
				p = nullptr;
				break;
			}
			if (p!=nullptr)
				pixelsets.push_back(p);
		}
	}
	return *this;
}

void Canvas::setColor(int r, int g, int b)
{
	color.setRgb(r, g, b);
}

void Canvas::getIamge(QImage *image)
{
	image->fill(Qt::white);
	for (int i = 0; i < pixelsets.size(); i++) {
		pixelsets[i]->paint(image);
	}
}

void Canvas::translate(int id, int dx, int dy)
{
	for (size_t i = 0; i < pixelsets.size(); i++) {
		if (pixelsets[i]->id == id) {
			pixelsets[i]->translate(dx, dy);
			return;
		}
	}
}

void Canvas::rotate(int id, int x, int y, int r)
{
	for (auto i = pixelsets.begin(); i != pixelsets.end(); i++) {
		if ((*i)->id == id) {
			(*i)->rotate(x, y, r);
			return;
		}
	}
}

void Canvas::scale(int id, int x, int y, float s)
{
	for (auto i = pixelsets.begin(); i != pixelsets.end(); i++) {
		if ((*i)->id == id) {
			(*i)->scale(x, y, s);
			return;
		}
	}
}

void Canvas::clip(int id, int x1, int y1, int x2, int y2, string algorithm)
{
	for (auto i = pixelsets.begin(); i != pixelsets.end(); i++) {
		if ((*i)->id == id) {
			(*i)->clip(x1, y1, x2, y2, algorithm);
			return;
		}
	}
}


int Canvas::getID(int x, int y)
{
	int res_ID = -1;
	for (size_t i = 0; i < pixelsets.size(); i++) {
		for (size_t j = 0; j < pixelsets[i]->points.size(); j++) {
			int ix = pixelsets[i]->points[j].x;
			int iy = pixelsets[i]->points[j].y;
			if ((ix - x)*(ix - x) + (iy - y)*(iy - y) <= CLICK_BIAS * CLICK_BIAS) {
				return pixelsets[i]->id;
			}
		}
	}
	return res_ID;
}

void Canvas::delID(int id)
{
	for (auto it = pixelsets.begin(); it != pixelsets.end();) {
		if ((*it)->id == id) {
			delete (*it);
			it = pixelsets.erase(it);
			return;
		}
		else {
			++it;
		}
	}
	return;
}

void Canvas::drawLine(int id, int x1, int y1, int x2, int y2, ALGORITHM algorithm)
{
	
	PixelSet *p;
	switch (algorithm)
	{
	case DDA:
		p = new Line(x1, y1, x2, y2, "DDA");
		break;
	case BRESENHAM:
		p = new Line(x1, y1, x2, y2, "Bresenham");
		break;
	default:
		p = new Line(x1, y1, x2, y2, "DDA");
		break;
	}
	p->refresh();
	p->setID(id);
	p->setColor(color);
	pixelsets.push_back(p);
}

void Canvas::drawPolygon(int id, const vector<Point> &vertexs, ALGORITHM algorithm)
{
	bool hasID = false;
	int keyIndex = -1;
	for (int i = 0; i < pixelsets.size(); i++) {
		if (pixelsets[i]->id == id) {
			hasID = true; keyIndex = i;
		}
	}
	if (hasID) { //已有，则重画
		delete pixelsets[keyIndex];
		if (algorithm == DDA)
			pixelsets[keyIndex] = new Polygon(vertexs, "DDA");
		else
			pixelsets[keyIndex] = new Polygon(vertexs, "Bresenham");
		pixelsets[keyIndex]->refresh();
		pixelsets[keyIndex]->setID(id);
		pixelsets[keyIndex]->setColor(color);
	}
	else {
		PixelSet *p;
		if (algorithm == DDA)
			p = new Polygon(vertexs, "DDA");
		else
			p = new Polygon(vertexs, "Bresenham");
		p->refresh();
		p->setID(id);
		p->setColor(color);
		pixelsets.push_back(p);
	}
}

void Canvas::drawEllipse(int id, int x, int y, int rx, int ry)
{
	PixelSet *p = new Ellipse(x, y, rx, ry);
	p->refresh();
	p->setID(id);
	p->setColor(color);
	pixelsets.push_back(p);
}

void Canvas::drawDotPoint(int id, int x, int y, int iwidth, QColor icolor)
{
	PixelSet *p = new DotPoint(x, y, iwidth, icolor);
	p->setID(id);
	pixelsets.push_back(p);
}
