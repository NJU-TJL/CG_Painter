#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QLabel>
#include <QColorDialog>
#include <QFileDialog>
#include "ui_CG_Painter.h"
#include "canvas.h"

class CG_Painter : public QMainWindow
{
	Q_OBJECT

public:
	CG_Painter(QWidget *parent = Q_NULLPTR);

private:
	Ui::CG_PainterClass ui;

	int mouse_x = 0, mouse_y = 0;//鼠标位置
	QLabel *statusLabel;
	void refreshStateLabel();

	Canvas myCanvas;
	Canvas bufCanvas;
	bool buf_flag = false;
	QColor nowColor=Qt::black;

	//表示当前状态：画直线、多边形、椭圆
	enum PAINTER_STATE {
		NOT_DRAWING, DRAW_LINE, DRAW_POLYGON, DRAW_ELLIPSE, DRAW_ROTATE
	};
	PAINTER_STATE state = NOT_DRAWING;
	QString state_info = "";
	void setState(PAINTER_STATE newState);
	//表示当前所用算法
	ALGORITHM algorithm;
	QString algo_info="";
	void setAlgo(ALGORITHM newAlgo);

	//鼠标按下
	void mousePressEvent(QMouseEvent *event);
	//鼠标移动
	void mouseMoveEvent(QMouseEvent *event);
	//鼠标释放
	void mouseReleaseEvent(QMouseEvent *event);
	//鼠标双击
	void mouseDoubleClickEvent(QMouseEvent *event);


	//重绘事件
	void paintEvent(QPaintEvent *event);

	/*直线*/
	int line_Ax, line_Ay, line_Bx, line_By;
	enum LINE_STATE { LINE_NON_POINT, LINE_POINTA };
	LINE_STATE line_state = LINE_NON_POINT;

	/*多边形*/
	int startX, startY;
	vector<Point> poly_points;
	vector<Point> bufpoly_points;
	int poly_id;
	enum POLY_STATE { POLY_NON_POINT, POLY_PAINTING };
	POLY_STATE poly_state = POLY_NON_POINT;
	int AUTO_BIAS = 15; //多边形绘图自动贴合开启的距离（圆形区域的半径）
	bool autoPoly(int &nowx, int &nowy);

	/*椭圆*/
	int cx, cy;//圆心
	int DEFAULT_RX = 50;
	int DEFAULT_RY = 25;
	int rx = DEFAULT_RX, ry = DEFAULT_RY;//长短轴
	enum ELLI_STATE { ELLI_NON_POINT, ELLI_PAINTING };
	ELLI_STATE elli_state = ELLI_NON_POINT;
	int init_x, init_y;
	int changing_dx, changing_dy;
	int elli_id;

	/*平移*/
	int trans_ID;
	int trans_ix, trans_iy;
	enum TRANS_STATE { TRANS_NON, TRANS_BEGIN};
	TRANS_STATE trans_state = TRANS_NON;
	
	/*选中*/
	int selected_ID;

	/*旋转*/
	enum ROTATE_STATE { ROTATE_NON, ROTATE_READY, ROTATE_BEGIN };
	ROTATE_STATE rotate_state = ROTATE_NON;
	int rotate_rx, rotate_ry;

	/*辅助示意元素*/

	//ID管理
	int ID_Counter = 0;
	int getNewID();

private slots:
	void action_to_set_color() {
		nowColor = QColorDialog::getColor(Qt::black,this,u8"绘图颜色选择");
		myCanvas.setColor(nowColor);
	}
	void action_to_save() {
		QString filename = QFileDialog::getSaveFileName(this, u8"保存路径选择", ".", u8"图片 (*.bmp)");
		if (!filename.isNull()) {
			QImage *image = new QImage(geometry().width(), geometry().height(), QImage::Format_RGB888);
			myCanvas.getIamge(image);
			(*image).save(filename);
		}
	}
	void action_to_delete() {
		myCanvas.delID(selected_ID);
		update();
	}
	void action_to_rotate() {
		setState(DRAW_ROTATE);
	}
	void state_to_lineDDA() { setState(DRAW_LINE); setAlgo(DDA); }
	void state_to_lineBresenham() { setState(DRAW_LINE); setAlgo(BRESENHAM); }
	void state_to_polygonDDA() { setState(DRAW_POLYGON); setAlgo(DDA); }
	void state_to_polygonBresenham() { setState(DRAW_POLYGON); setAlgo(BRESENHAM); }
	void state_to_ellipse() { 
		setState(DRAW_ELLIPSE); }

};
