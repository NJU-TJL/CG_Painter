#ifndef ALGORITHM_H
#define ALGORITHM_H

#include<vector>
using namespace std;

const double PI=3.1415926535;

class PixelSet;
class Point;

//画一条直线-DDA算法
void drawLine_DDA(int x1,int y1,int x2,int y2, PixelSet& myset);
//画一条直线-Bresenham算法
void drawLine_Bresenham(int x1,int y1,int x2,int y2, PixelSet& myset);

//画多边形-DDA算法
void drawPolygon_DDA(const vector<Point>& vertexs, PixelSet& myset);
//画多边形-Bresenham算法
void drawPolygon_Bresenham(const vector<Point> &vertexs, PixelSet& myset);

//画椭圆-中点椭圆生成算法
void drawEllipse(int x,int y,int rx,int ry, PixelSet& myset);


//将某个点(x,y)绕(xr,yr)旋转r°角
void rotatePoint(int &x,int &y,int xr,int yr,int r);
//(x1,y1)->(x2,y2)绕(xr,yr)  获得旋转的角度r°（返回值）
int getRotateR(int x1, int y1, int xr, int yr,int x2,int y2);

//将某个点(x,y)以(sx,sy)为中心按s的比例缩放
void scalePoint(int &x,int &y,int sx,int sy,double s);
//(x1,y1)->(x2,y2)以(sx,sy)缩放  获得缩放比例s（返回值）
double getScaleS(int x1, int y1, int sx, int sy, int x2, int y2);


//裁剪
//Cohen-Sutherland算法
void Cohen_Sutherland(int &x1, int &y1, int &x2, int &y2, int xmin, int ymin, int xmax, int ymax);
//Liang-Barsky算法
bool Liang_Barsky(int &x1, int &y1, int &x2, int &y2, int xmin, int ymin, int xmax, int ymax);

/*曲线部分*/
//Bezier曲线生成
void drawCurve_Bezier(const vector<Point>& vertexs, PixelSet& myset);
//B-spline曲线生成
void drawCurve_Bspline(const vector<Point> &vertexs, PixelSet &myset);

#endif // ALGORITHM_H
