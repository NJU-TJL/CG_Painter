#ifndef CANVAS_H
#define CANVAS_H

#include<QString>

#include"pixelset.h"


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
	void setColor(QColor icolor) { color = icolor; }
	//将当前所有图元输出到画布，且跳过超出画布边界的点
	void getIamge(QImage *image);

	//画一条直线
	void drawLine(int id, int x1, int y1, int x2, int y2, ALGORITHM algorithm);

	//画多边形
	void drawPolygon(int id, const vector<Point>& vertexs, ALGORITHM algorithm);

	//画椭圆-中点椭圆生成算法
	void drawEllipse(int id, int x, int y, int rx, int ry);

	//画一个实心圆点（用于辅助示意）
	void drawDotPoint(int id, int x, int y, int iwidth = 10, QColor icolor = QColor(0x32, 0xCD, 0x32));

	//画一个虚线矩形框（用于辅助示意）
	void drawRectangle(int id, int x1, int y1, int x2, int y2, int iwidth = 2, QColor icolor = QColor(0x32, 0xCD, 0x32));

	//指定id的图元平移
	void translate(int id, int dx, int dy);

	//指定id的图元旋转
	void rotate(int id, int x, int y, int r);

	//指定id的图元缩放
	void scale(int id, int x, int y, double s);

	//指定id的图元裁剪
	void clip(int id, int ix1, int iy1, int ix2, int iy2, ALGORITHM algorithm = COHEN);
	//用一个剪裁窗口剪裁所有图元
	void clipAll(int ix1, int iy1, int ix2, int iy2, ALGORITHM algorithm = COHEN);

	//查看指定某点是否对应于某个图元ID，是则返回ID，否则返回-1；
	int CLICK_BIAS = 4; //允许的误差范围（圆形区域的半径）
	int getID(int x, int y);

	//删除指定ID的图元
	void delID(int id);
		
};

#endif // CANVAS_H
