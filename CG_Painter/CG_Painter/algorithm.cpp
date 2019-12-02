#include"algorithm.h"
#include"pixelset.h"

#include<assert.h>
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
    myset.translate(x,y);
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

void scalePoint(int &x, int &y, int sx, int sy, float s)
{
    int x1,y1,x2,y2;
    x1=x-sx;
    y1=y-sy;
    x2=qRound(x1*s);
    y2=qRound(y1*s);
    x=x2+sx;
    y=y2+sy;
}

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

	int new_x1 = x1 + u1 * dx;
	int new_y1 = y1 + u1 * dy;
	int new_x2 = x1 + u2 * dx;
	int new_y2 = y1 + u2 * dy;
	x1 = new_x1;
	y1 = new_y1;
	x2 = new_x2;
	y2 = new_y2;
	return true;

}
