#ifndef PIXELSET_H
#define PIXELSET_H

#include<vector>
using namespace std;
#include<QImage>

//表示一个坐标点的类
class Point
{
public:
	int x, y;
	Point(int ix = 0, int iy = 0) { x = ix; y = iy; }
	//设定坐标
	void set(int ix, int iy) { x = ix; y = iy; }
};

enum PS_TYPE
{
	LINE, POLYGON, ELLIPSE
};

//表示像素点集合的类，即表示一个图元
class PixelSet
{
protected:
	int id;//图元编号
	QColor color;//图元颜色
	vector<Point> points;//构成图元的所有像素点的坐标
	PS_TYPE type;//图元类型

	friend class Canvas;
public:
	PixelSet(int i = 0, QColor icolor = Qt::black) { id = i; color = icolor; }
	PixelSet(const PixelSet& B) {
		id = B.id; color = B.color; type = B.type;
		points.clear();
		points.assign(B.points.begin(), B.points.end());
	}
	//设定图元ID
	void setID(int i) { id = i; }
	//设定图元颜色
	void setColor(const QColor &icolor) { color = icolor; }
	//增加一个指定坐标的像素点
	void add(int x, int y) { points.push_back(Point(x, y)); }
	//将本图元绘制于画布上
	void paint(QImage *image) const;
	//平移:(dx,dy)
	void translate(int dx, int dy);
	//根据参数绘制相应的图元
	virtual void refresh() = 0;
	//旋转
	virtual void rotate(int x, int y, int r) = 0;
	//缩放
	virtual void scale(int x, int y, float s) = 0;
	//裁剪
	virtual void clip(int x1, int y1, int x2, int y2, string algorithm) {}
};

//线段
class Line :public PixelSet
{
	//图元的参数
	int x1, y1, x2, y2;
	string algorithm;
public:
	Line() { type = LINE; }
	Line(const Line& B) :PixelSet(B) { 
		x1 = B.x1; 
		y1 = B.y1;
		x2 = B.x2;
		y2 = B.y2;
		algorithm = B.algorithm;
	}
	Line(int ix1, int iy1, int ix2, int iy2, string ialgorithm) {
		type = LINE;
		set(ix1, iy1, ix2, iy2, ialgorithm);
	}
	//参数设定
	void set(int ix1, int iy1, int ix2, int iy2, string ialgorithm) {
		x1 = ix1; y1 = iy1;
		x2 = ix2; y2 = iy2;
		algorithm = ialgorithm;
	}
	//根据参数绘制图元
	void refresh();
	//旋转
	void rotate(int x, int y, int r);
	//缩放
	void scale(int x, int y, float s);
	//裁剪
	void clip(int x1, int y1, int x2, int y2, string algorithm);
};

//多边形
class Polygon :public PixelSet
{
	//图元的参数
	vector<Point> vertexs;
	string algorithm;
	bool polygon_closed = true;
public:
	Polygon() { type = POLYGON; }
	Polygon(const Polygon& B) : PixelSet(B) {
		vertexs.clear();
		vertexs.assign(B.vertexs.begin(), B.vertexs.end());
		algorithm = B.algorithm;
		polygon_closed = B.polygon_closed;
	}
	Polygon(const vector<Point>& ivertexs, string ialgorithm) {
		type = POLYGON;
		set(ivertexs, ialgorithm);
	}
	//参数设定
	void set(const vector<Point>& ivertexs, string ialgorithm) {
		vertexs.assign(ivertexs.begin(), ivertexs.end());
		algorithm = ialgorithm;
	}
	//根据参数绘制图元
	void refresh();
	//旋转
	void rotate(int x, int y, int r);
	//缩放
	void scale(int x, int y, float s);
};

//椭圆
class Ellipse :public PixelSet
{
	//图元的参数
	int x, y;
	int rx, ry;
	//椭圆绕椭圆中心顺时针旋转的角度
	int r = 0;
public:
	Ellipse() { type = ELLIPSE; }
	Ellipse(const Ellipse& B):PixelSet(B) {
		x = B.x; 
		y = B.y;
		rx = B.rx;
		ry = B.ry;
		r = B.r;
	}
	Ellipse(int ix, int iy, int irx, int iry) {
		type = ELLIPSE;
		set(ix, iy, irx, iry);
		r = 0;
	}
	//参数设定
	void set(int ix, int iy, int irx, int iry) {
		x = ix; y = iy;
		rx = irx; ry = iry;
		r = 0;
	}
	//根据参数绘制图元
	void refresh();
	//旋转
	void rotate(int ix, int iy, int ir); //绕点(ix,iy旋转)顺时针
	void rotate(int ir); //绕自身中心旋转
	//缩放
	void scale(int x, int y, float s);
};

#endif // PIXELSET_H
