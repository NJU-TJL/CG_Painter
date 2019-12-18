#ifndef PIXELSET_H
#define PIXELSET_H

#include<vector>
using namespace std;
#include<QImage>


enum ALGORITHM {
	DDA, BRESENHAM, COHEN, LIANG, BEZIER, B_SPLINE
};

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
	LINE, POLYGON, ELLIPSE, DOTPOINT, RECTANGLE, CURVE, FOLDLINE, CTRLPOINT
};

//表示像素点集合的类，即表示一个图元
class PixelSet
{
protected:
	int id;//图元编号
	QColor color;//图元颜色
	vector<Point> points;//构成图元的所有像素点的坐标
	bool clear_flag = false;//标记是否无效，需要被清除
	friend class Canvas;
public:
	PS_TYPE type;//图元类型
	PixelSet(int i = 0, QColor icolor = Qt::black) { id = i; color = icolor; }
	PixelSet(const PixelSet& B) {
		id = B.id; color = B.color; type = B.type;
		points.clear();
		points.assign(B.points.begin(), B.points.end());
	}
	//设定图元ID
	void setID(int i) { id = i; }
	//判断一个点是否在本图元上（如果在，返回ID；不在，返回-1）
	int CLICK_BIAS = 3; //允许的误差范围（圆形区域的半径）
	virtual int getID(int x, int y);
	//设定图元颜色
	void setColor(const QColor &icolor) { color = icolor; }
	//增加一个指定坐标的像素点
	void add(int x, int y) { points.push_back(Point(x, y)); }
	//将本图元绘制于画布上
	virtual void paint(QImage *image);
	//平移:(dx,dy)
	virtual void translate(int dx, int dy);
	//根据参数绘制相应的图元
	virtual void refresh() {}
	//旋转
	virtual void rotate(int x, int y, int r) {}
	//缩放
	virtual void scale(int x, int y, float s) {}
	//裁剪
	virtual void clip(int x1, int y1, int x2, int y2, ALGORITHM algorithm) {}
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
	//平移
	void translate(int dx, int dy) {
		PixelSet::translate(dx, dy);
		x1 += dx; y1 += dy;
		x2 += dx; y2 += dy;
	}
	//旋转
	void rotate(int x, int y, int r);
	//缩放
	void scale(int x, int y, float s);
	//裁剪
	void clip(int x1, int y1, int x2, int y2, ALGORITHM algorithm);
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
	//平移
	void translate(int dx, int dy) {
		PixelSet::translate(dx, dy);
		for (int i = 0; i < vertexs.size(); i++) {
			vertexs[i].x += dx;
			vertexs[i].y += dy;
		}
	}
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
	//平移
	void translate(int dx, int dy) {
		PixelSet::translate(dx, dy);
		x += dx; y += dy;
	}
	//旋转
	void rotate(int ix, int iy, int ir); //绕点(ix,iy旋转)顺时针
	void rotate(int ir); //绕自身中心旋转
	//缩放
	void scale(int x, int y, float s);
};

//示意点
class DotPoint :public PixelSet {
	//点所在的位置
	int x, y;
	//宽度
	int width;
public:
	DotPoint() { type = DOTPOINT; }
	DotPoint(const DotPoint& B) :PixelSet(B) {
		x = B.x; y = B.y;
		width = B.width;
	}
	DotPoint(int ix, int iy, int iwidth = 5, QColor icolor = QColor(0xB2, 0xDF,0xEE)) {
		x = ix; y = iy;
		width = iwidth; color = icolor;
		type = DOTPOINT;
	}
	//根据参数绘制图元
	void paint(QImage *image);
};

//矩形框（用于剪裁时的示意剪裁窗口）
class Rectangle :public PixelSet {
	//点所在的位置
	int x1, y1;
	int x2, y2;
	//宽度
	int width;
public:
	Rectangle() { type = RECTANGLE; }
	Rectangle(const Rectangle& B) :PixelSet(B) {
		x1 = B.x1; y1 = B.y1;
		x2 = B.x2; y2 = B.y2;
		width = B.width;
		type = RECTANGLE;
	}
	Rectangle(int ix1, int iy1, int ix2, int iy2, int iwidth = 5, QColor icolor = QColor(0xB2, 0xDF, 0xEE)) {
		x1 = min(ix1, ix2); x2 = max(ix1, ix2);
		y1 = min(iy1, iy2); y2 = max(iy1, iy2);
		width = iwidth; color = icolor;
		type = RECTANGLE;
	}
	//根据参数绘制图元
	void paint(QImage *image);
};

//折线段（用于辅助示意：曲线控制点）
class FoldLine :public PixelSet
{
	//图元的参数
	vector<Point> vertexs;
	int width;
	friend class Curve;
	friend class CtrlPoint;
public:
	FoldLine() { type = FOLDLINE; }
	FoldLine(const vector<Point>& ivertexs, int iwidth = 2, QColor icolor = QColor(0xB2, 0xDF, 0xEE)) {
		set(ivertexs); type = FOLDLINE;
		color = icolor;
		width = iwidth;
	}
	//参数设定
	void set(const vector<Point>& ivertexs) {
		vertexs = ivertexs;
	}
	//根据参数绘制图元
	void paint(QImage *image);
	//平移
	void translate(int dx, int dy) {
		for (int i = 0; i < vertexs.size(); i++) {
			vertexs[i].x += dx;
			vertexs[i].y += dy;
		}
	}
	//旋转
	void rotate(int x, int y, int r);
	//缩放
	void scale(int x, int y, float s);
};
//曲线的控制点
class CtrlPoint :public PixelSet 
{
	//在曲线控制点组中的位置下标
	size_t index;
	//曲线控制点组（存在虚折线中）
	FoldLine *foldline;
	int width;//点的粗细
public:
	CtrlPoint() { type = CTRLPOINT; }
	CtrlPoint(const CtrlPoint& B, Canvas&canvas);
	CtrlPoint(size_t iindex, FoldLine *ifoldline, int iwidth = 10, QColor icolor = QColor(0xB2, 0xDF, 0xEE)) {
		type = CTRLPOINT;
		index = iindex; foldline = ifoldline;
		width = iwidth; color = icolor;
	}
	//鼠标判断
	int getID(int x, int y);
	//平移
	void translate(int dx, int dy);
	//根据参数绘制图元
	void paint(QImage *image);
};

//曲线
class Curve :public PixelSet
{
	//图元的参数
	vector<Point> vertexs;
	ALGORITHM algorithm;
	FoldLine* foldline; //用于辅助示意的虚折线（此处注意：拷贝赋值的时候要额外处理）
public:
	Curve() { type = CURVE; }
	Curve(ALGORITHM ialgorithm, FoldLine* ifoldline) {
		type = CURVE; foldline = ifoldline;
		set(ialgorithm);
	}
	Curve(const Curve& B, Canvas& canvas);
	//参数设定
	void set(ALGORITHM ialgorithm) {
		algorithm = ialgorithm;
	}
	//根据参数绘制图元
	void refresh();
	void paint(QImage *image);
	//平移
	void translate(int dx, int dy) {
		foldline->translate(dx, dy);
	}
	//旋转
	void rotate(int x, int y, int r) {
		foldline->rotate(x, y, r);
	}
	//缩放
	void scale(int x, int y, float s) {
		foldline->scale(x, y, s);
	}
	~Curve() {
		foldline->clear_flag = true;
	}
};
#endif // PIXELSET_H
