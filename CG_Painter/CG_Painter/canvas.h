#ifndef CANVAS_H
#define CANVAS_H

#include<QString>

#include"pixelset.h"

enum ALGORITHM {
	DDA, BRESENHAM
};

//表示画布的类
class Canvas
{
	//当前画笔颜色
	QColor color;
	//当前所有的图元
	vector<PixelSet *> pixelsets;

public:
	Canvas() :color(0, 0, 0) {}
	Canvas(const Canvas& B);
	const Canvas& operator=(const Canvas& B);
	~Canvas();

	//设置当前画笔颜色
	void setColor(int r, int g, int b);
	//将当前所有图元输出到画布，且跳过超出画布边界的点
	void getIamge(QImage *image);

	//画一条直线
	void drawLine(int id, int x1, int y1, int x2, int y2, ALGORITHM algorithm);

	//画多边形
	void drawPolygon(int id, const vector<Point>& vertexs, ALGORITHM algorithm);

	//画椭圆-中点椭圆生成算法
	void drawEllipse(int id, int x, int y, int rx, int ry);

	//指定id的图元平移
	void translate(int id, int dx, int dy);

	//指定id的图元旋转
	void rotate(int id, int x, int y, int r);

	//指定id的图元缩放
	void scale(int id, int x, int y, float s);

	//指定id的图元裁剪
	void clip(int id, int x1, int y1, int x2, int y2, string algorithm);

	//查看指定某点是否对应于某个图元ID，是则返回ID，否则返回-1；
	int CLICK_BIAS = 4; //允许的误差范围（圆形区域的半径）
	int getID(int x, int y);
		
};

#endif // CANVAS_H
