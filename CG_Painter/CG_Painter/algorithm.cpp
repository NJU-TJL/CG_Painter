#include"algorithm.h"
#include"pixelset.h"
#include<QDebug>
#include<assert.h>
#include<math.h>
#include<algorithm>

//画一条直线-DDA算法
void drawLine_DDA(int x1, int y1, int x2, int y2, PixelSet &myset)
{
    int xl=x1;int yl=y1;//(xl,yl)表示位于左侧的点(x_left)
    int xr=x2;int yr=y2;//(xr,yr)表示位于右侧的点(x_right)
    if(x1>x2){ //确保左、右点对应(xl,yl)、(xr,yr)
        xl=x2;yl=y2;
        xr=x1;yr=y1;
    }
    double k=(y2-y1)/((double)x2-x1);//斜率m
    if(qAbs(k)<=1){ //斜率绝对值小于等于1：x方向按单位长度1变化
        int dx=1;
        double dy=k;
        int xk=xl;
        double yk=yl; //用于算法循环的(xk,yk)
        int yk_round=qRound(yk);//取整后的yk
        myset.add(xk,yk_round);
        while(xk<xr){
            xk+=dx;yk+=dy;
            yk_round=qRound(yk);
            myset.add(xk,yk_round);
        }
        
    }else{  //斜率绝对值大于1：y方向按单位长度1变化
        double dx=qAbs(1/k);
        int dy=1;
        bool flag=true;
        if(yl>yr){ //左点位于右点左上方的情况
            dy=-1;
            flag=false;
        }
        double xk=xl;
        int yk=yl; //用于算法循环的(xk,yk)
        int xk_round=qRound(xk);//取整后的xk
        myset.add(xk_round,yk);
        while((flag&&yk<yr)||(!flag&&yk>yr)){ //结束条件：y方向从左点以dy的变化到了右点（flag表示y的值变化方向的正负）
            xk+=dx;yk+=dy;
            xk_round=qRound(xk);
            myset.add(xk_round,yk);
        }
    }
}

//画一条直线-Bresenham算法
void drawLine_Bresenham(int x1, int y1, int x2, int y2, PixelSet &myset)
{
    int xl=x1;int yl=y1;//(xl,yl)表示位于左侧的点(x_left)
    int xr=x2;int yr=y2;//(xr,yr)表示位于右侧的点(x_right)
    if(x1>x2){ //确保左、右点对应(xl,yl)、(xr,yr)
        xl=x2;yl=y2;
        xr=x1;yr=y1;
    }
    
    bool flag=true;//左点位于右点左下方（斜率大于等于0的情况）时为true，否则为假
    if(yl>yr) flag=false;
    
    /* 先解决三种特殊情况：水平、垂直、对角线 */
    //垂直线
    if(xl==xr){
        int di=-1;
        if(flag) di=1; //左点位于右点下方时
        for (int i = yl; i != yr; i+=di) {
            myset.add(xl,i);
        }
        myset.add(xr,yr);
        return;
    }
    //水平线
    if(yl==yr){
        for (int i = xl; i <= xr; ++i) {
            myset.add(i,yl);
        }
        return;
    }
    //对角线1
    if((xr-xl)==(yr-yl)){
        for(int i=xl,j=yl; i<=xr;++i,++j){
            myset.add(i,j);
        }
        return;
    }
    //对角线2
    if((xr-xl)==-(yr-yl)){
        for(int i=xl,j=yl; i<=xr;++i,--j){
            myset.add(i,j);
        }
        return;
    }

    /* 下面是针对四类情况的讨论（已确保了左、右点，所以只有四种情况了） */
    int xk,yk;
    int dx,dy;
    int p; //Bresenham画线算法中第k步的决策参数
    
    xk=xl;yk=yl;
    dx=xr-xl;
    dy=yr-yl;
    
    if(flag){ //斜率大于0的情况
        if(dy<dx){ //斜率小于1
            p=2*dy-dx;
            while(xk<=xr){
                myset.add(xk,yk);
                if(p>=0){
                    yk++;
                    p+=2*(dy-dx);
                }else{
                    p+=2*dy;
                }
                xk++;
            }
        }else{ //斜率大于1
            p=2*dx-dy;
            while(yk<=yr){
                myset.add(xk,yk);
                if(p>=0){
                    xk++;
                    p+=2*(dx-dy);
                }else{
                    p+=2*dx;
                }
                yk++;
            }
        }
    }else{ //斜率小于0的情况
        if(-dy<dx){ //斜率大于-1
            p=2*dy+dx;
            while(xk<=xr){
                myset.add(xk,yk);
                if(p<=0){
                    yk--;
                    p+=2*(dy+dx);
                }else{
                    p+=2*dy;
                }
                xk++;
            }
        }else{ //斜率小于-1
            p=-2*dx-dy;
            while(yk>=yr){
                myset.add(xk,yk);
                if(p<=0){
                    xk++;
                    p+=2*(-dx-dy);
                }else{
                    p+=2*(-dx);
                }
                yk--;
            }
        }
    }
}

//画多边形-DDA算法
void drawPolygon_DDA(const vector<Point>& vertexs, PixelSet& myset)
{
    size_t n=vertexs.size();
    for(size_t i=0;i<n-1;i++){
        drawLine_DDA(vertexs[i].x,vertexs[i].y,vertexs[i+1].x,vertexs[i+1].y,myset);
    }
}

//画多边形-Bresenham算法
void drawPolygon_Bresenham(const vector<Point> &vertexs, PixelSet& myset)
{
    size_t n=vertexs.size();
    for(size_t i=0;i<n-1;i++){
        drawLine_Bresenham(vertexs[i].x,vertexs[i].y,vertexs[i+1].x,vertexs[i+1].y,myset);
    }
}

//画椭圆-中点椭圆生成算法
void drawEllipse(int x,int y,int rx,int ry, PixelSet& myset)
{

    //先绘制四个顶点
    myset.add(0,ry);myset.add(0,-ry);
    myset.add(rx,0);myset.add(-rx,0);
    
    bool flag=true;//为true表示ry<=rx,即正常的扁平型椭圆
    if(ry>rx){ //高瘦型椭圆，化为正常扁平型的：将作出的点，以y=x对称（也等价于旋转了90°）
        flag=false;
        int temp=rx;//交换rx ry
        rx=ry;
        ry=temp;
    }
    
    /* 先以原点(0,0)画，再平移；且以下仅需要在第一象限内考虑，其他三个象限可对称得到 */
    /* 椭圆区域1：切线斜率<=1 */
    double p1= ry*ry - rx*rx*ry + rx*rx*0.25; //区域1的决策参量及其初始值
    int xk=0,yk=ry;
    while(2*ry*ry*xk<2*rx*rx*yk){
        xk++;
        if(p1<0){
            p1+= (2*ry*ry*xk + ry*ry);
        }else{
            yk--;
            p1+= (2*ry*ry*xk + ry*ry - 2*rx*rx*yk);
        }
        if(flag){
            myset.add(xk,yk);myset.add(xk,-yk);myset.add(-xk,yk);myset.add(-xk,-yk);
        }else{ //将作出的点，以y=x对称（也等价于旋转了90°）
            myset.add(yk,xk);myset.add(yk,-xk);myset.add(-yk,xk);myset.add(-yk,-xk);
        }
        
    }
    /* 椭圆区域2：切线斜率>1 */
	double p2 = ry * ry*(xk + 0.5)*(xk + 0.5) + 1.0*rx * rx*(yk - 1)*(yk - 1) - 1.0*rx*rx*ry*ry;
    while(yk>1){ //循环到(rx,0)即可
        yk--;
        if(p2>0){
            p2+= (-2*rx*rx*yk + rx*rx);
        }else{
            xk++;
            p2+= (2*ry*ry*xk -2*rx*rx*yk + rx*rx);
        }
        if(flag){
            myset.add(xk,yk);myset.add(xk,-yk);myset.add(-xk,yk);myset.add(-xk,-yk);
        }else{ //将作出的点，以y=x对称（也等价于旋转了90°）
            myset.add(yk,xk);myset.add(yk,-xk);myset.add(-yk,xk);myset.add(-yk,-xk);
        }
    }    
    //平移：(0,0)->(x,y)
    myset.PixelSet::translate(x,y);
	
}

void rotatePoint(int &x, int &y, int xr, int yr, int r)
{
    double theta=r*PI/180.0;
    int x1,y1,x2,y2;
    x1=x-xr;
    y1=y-yr;
    x2=qRound(x1*cos(theta)-y1*sin(theta));
    y2=qRound(x1*sin(theta)+y1*cos(theta));
    x=x2+xr;
    y=y2+yr;
}
//利用数学公式 cos=(a2+b2-c2)/2ab（c为对边）
int getRotateR(int x1, int y1, int xr, int yr, int x2, int y2)
{
	x1 = x1 - xr;
	y1 = y1 - yr;
	x2 = x2 - xr;
	y2 = y2 - yr;
	double a2 = x1 * x1 + y1 * y1;
	double b2 = x2 * x2 + y2 * y2;
	double c2 = (x1-x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
	double cos_temp = (a2 + b2 - c2) / (2 * sqrt(a2)*sqrt(b2));
	double theta = acos(cos_temp);
	if (x1*y2 < x2*y1) theta = -theta;
	int r = qRound(theta * 180 / PI);
	return r;
}

void scalePoint(int &x, int &y, int sx, int sy, double s)
{
    int x1,y1,x2,y2;
    x1=x-sx;
    y1=y-sy;
    x2=qRound(x1*s);
    y2=qRound(y1*s);
    x=x2+sx;
    y=y2+sy;
}

double getScaleS(int x1, int y1, int sx, int sy, int x2, int y2)
{
	x1 -= sx; 
	y1 -= sy;
	x2 -= sx;
	y2 -= sy;
	double res = 1;
	double x1y1 = (x1 * x1 + y1 * y1);
	double x2y2 = (x2 * x2 + y2 * y2);
	res = sqrt(x2y2 / x1y1);
	return res;
}

//求交点
void crossX(int x1,int y1,int x2,int y2,int x,int &y){
    double a=x;
    y=qRound((y2-y1)*(a-x1)/(x2-x1)+y1);
}
void crossY(int x1,int y1,int x2,int y2,int &x,int y){
    double b=y;
    x=qRound((x2-x1)*(b-y1)/(y2-y1)+x1);
}

void Cohen_Sutherland(int &x1, int &y1, int &x2, int &y2, int xmin, int ymin, int xmax, int ymax){
    int code1=0;
    int code2=0;
    //按区域编码（从编码位左到右依次是对应裁剪边界的上、下、右、左）
    if(x1<xmin) code1|=0x0001;
    if(x1>xmax) code1|=0x0010;
    if(y1<ymin) code1|=0x0100;
    if(y1>ymax) code1|=0x1000;
    if(x2<xmin) code2|=0x0001;
    if(x2>xmax) code2|=0x0010;
    if(y2<ymin) code2|=0x0100;
    if(y2>ymax) code2|=0x1000;
    //完全在窗口内的情况
    if((code1|code2)==0) return;
    //完全在窗口外的情况
    if((code1&code2)!=0){
        x1=x2=y1=y2=-1;
        return;
    }
    //其余情况，按照上、下、右、左的顺序进行边界求交
    if(((code1|code2)&0x1000)!=0){
        int x3,y3=ymax;
        crossY(x1,y1,x2,y2,x3,y3);
        if(y1>y2){
            x1=x3;y1=y3;
        }
        else{
            x2=x3;y2=y3;
        }
            
    }
    else if(((code1|code2)&0x0100)!=0){
        int x3,y3=ymin;
        crossY(x1,y1,x2,y2,x3,y3);
        if(y1<y2){
            x1=x3;y1=y3;
        }
        else{
            x2=x3;y2=y3;
        }
         
    }
    else if(((code1|code2)&0x0010)!=0){
        int x3=xmax,y3;
        crossX(x1,y1,x2,y2,x3,y3);
        if(x1>x2){
            x1=x3;y1=y3;
        }
        else{
            x2=x3;y2=y3;
        }
         
    }
    else if(((code1|code2)&0x0001)!=0){
        int x3=xmin,y3;
        crossX(x1,y1,x2,y2,x3,y3);
        if(x1<x2){
            x1=x3;y1=y3;
        }
        else{
            x2=x3;y2=y3;
        }
            
    }   
    return Cohen_Sutherland(x1,y1,x2,y2,xmin,ymin,xmax,ymax);
}

bool Liang_Barsky(int & x1, int & y1, int & x2, int & y2, int xmin, int ymin, int xmax, int ymax)
{
	int dx = x2 - x1;
	int dy = y2 - y1;
	int q[5]; int p[5];//q[0]、p[0]弃用

	p[1] = -dx; q[1] = x1 - xmin;
	p[2] = dx; q[2] = xmax - x1;
	p[3] = -dy; q[3] = y1 - ymin;
	p[4] = dy; q[4] = ymax - y1;

	//初始时，u1、u2分别表示(x1,y1)、(x2,y2)两个点的参数
	double u1 = 0, u2 = 1;
	for (int i = 1; i <= 4; i++) {
		if (p[i] < 0) {
			u1 = max(u1, ((double)q[i]) / p[i]);
		}
		else if (p[i] > 0) {
			u2 = min(u2, ((double)q[i]) / p[i]);
		}
		else if (p[i]==0&&q[i]<0) {
			return false;
		}

		//判断是否舍弃
		if (u1 > u2) {
			return false;
		}
	}

	int new_x1 = qRound(x1 + u1 * dx);
	int new_y1 = qRound(y1 + u1 * dy);
	int new_x2 = qRound(x1 + u2 * dx);
	int new_y2 = qRound(y1 + u2 * dy);
	x1 = new_x1;
	y1 = new_y1;
	x2 = new_x2;
	y2 = new_y2;
	return true;

}

/*曲线部分*/
/*Bezier曲线生成*/
static double **P_x = nullptr; //中间计算过程需要用到的二维数组
static double **P_y = nullptr; //中间计算过程需要用到的二维数组
//获得参数u对应的离散点（函数调用前需要保证P已分配了足够的空间）
Point getPoint_Bezier(double u, const vector<Point> &vertexs) {
	//初始化
	size_t n = vertexs.size();
	for (size_t i = 0; i < n; i++) {
		P_x[i][0] = vertexs[i].x;
		P_y[i][0] = vertexs[i].y;
	}
	//依次降阶计算（类似动态规划的计算过程）
	for (size_t r = 1; r < n; r++) {
		for (size_t i = 0; i < n - r; i++) {
			//P[i][r]=(1-u)*P[i][r-1]+u*P[i+1][r-1]; //递推式 - 关键
			P_x[i][r] = (1 - u)*P_x[i][r - 1] + u * P_x[i + 1][r - 1]; //注：此出处计算应该全程使用double类型，最终取点的时候再取整，否则误差很大，曲线不够光滑
			P_y[i][r] = (1 - u)*P_y[i][r - 1] + u * P_y[i + 1][r - 1];
		}
	}
	return Point(qRound(P_x[0][n - 1]), qRound(P_y[0][n - 1]));
}
//决策步长
double getStep(const vector<Point> &vertexs) {
	double sum = 1;
	for (size_t i = 1; i < vertexs.size(); i++) {
		sum += sqrt((vertexs[i].x - vertexs[i - 1].x)*(vertexs[i].x - vertexs[i - 1].x) + (vertexs[i].y - vertexs[i - 1].y)*(vertexs[i].y - vertexs[i - 1].y));
	}
	return 1 / (sum*1.5);
}
//根据参数得出Bezier曲线
void drawCurve_Bezier(const vector<Point> &vertexs, PixelSet &myset)
{
	size_t n = vertexs.size();
	//分配空间
	P_x = new double*[n];
	P_y = new double*[n];
	for (size_t i = 0; i < n; i++) {
		P_x[i] = new double[n];
		P_y[i] = new double[n];
	}

	//以一定步长生成曲线上的离散点
	double STEP = getStep(vertexs);
	for (double u = 0.0; u <= 1.0; u += STEP) {
		Point temp = getPoint_Bezier(u, vertexs);
		myset.add(temp.x, temp.y);
	}
	//释放空间
	for (size_t i = 0; i < n; i++) {
		delete[] P_x[i];
		delete[] P_y[i];
	}
	delete[] P_x;
	delete[] P_y;
}

/*B-spline曲线*/
int k = 3;
int n = 0;
double *u;
vector<Point> P;
//计算λ（约定0/0=0）
double getLambda(int i, int r, double t) {
	if (fabs(u[i + k - r] - u[i]) <= 1e-7) {
		return 0;
	}
	else {
		return (t - u[i]) / (u[i + k - r] - u[i]);
	}
}
//使用de Boor Cox递推公式来分别递归计算点的x，y坐标
double deBoorCox_X(int i, int r, double t) {
	if (r == 0) return P[i].x;
	else {
		return getLambda(i, r, t)*deBoorCox_X(i, r - 1, t) + (1 - getLambda(i, r, t))*deBoorCox_X(i - 1, r - 1, t);
	}
}
double deBoorCox_Y(int i, int r, double t) {
	if (r == 0) return P[i].y;
	else {
		return getLambda(i, r, t)*deBoorCox_Y(i, r - 1, t) + (1 - getLambda(i, r, t))*deBoorCox_Y(i - 1, r - 1, t);
	}
}
//根据参数得出B-spline曲线
void drawCurve_Bspline(const vector<Point> &vertexs, PixelSet &myset) {
	if (vertexs.size() <= 1) return;
	//初始化
	P = vertexs;
	n = vertexs.size() - 1;
	u = new double[n + k + 2];
	for (int i = 0; i <= n + k + 1; i++) {
		u[i] = i;
	}
	//在区间u(k-1)~u(n+1)，分割区间计算
	double step = 0.0005;
	for (int j = k - 1; j < n + 1; j++) {
		for (double t = u[j]; t < u[j + 1]; t += step) {
			int x = qRound(deBoorCox_X(j, k - 1, t));
			int y = qRound(deBoorCox_Y(j, k - 1, t));
			myset.add(x, y);
		}
	}
	delete[] u;
}
