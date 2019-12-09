﻿#include "CG_Painter.h"
#include "algorithm.h"
#include <QMouseEvent>
#include <QPainter> 
#include <QDebug>

CG_Painter::CG_Painter(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	
	//增加绘制椭圆的Action
	QAction* actionEllipse = new QAction(tr(u8"椭圆"));
	actionEllipse->setStatusTip(tr(u8"【绘制椭圆】左键单击产生椭圆，左键按下拖动改变大小，双击左键确定并退出，按下右键取消绘制）"));
	connect(actionEllipse, &QAction::triggered, this, &CG_Painter::state_to_ellipse);
	ui.menuBar->addAction(actionEllipse);

	//旋转图元Action
	QAction* actionRotate = new QAction(tr(u8"旋转"));
	actionRotate->setStatusTip(tr(u8"【旋转图元】首先单击左键确定旋转中心，然后按住左键拖动图元旋转，右键退出"));
	connect(actionRotate, &QAction::triggered, this, &CG_Painter::action_to_rotate);
	ui.menuBar->addAction(actionRotate);

	//缩放图元Action
	QAction* actionScale = new QAction(tr(u8"缩放"));
	actionScale->setStatusTip(tr(u8"【缩放图元】首先单击左键确定缩放中心，然后按住左键拖动图元缩放，右键退出"));
	connect(actionScale, &QAction::triggered, this, &CG_Painter::action_to_scale);
	ui.menuBar->addAction(actionScale);

	//状态栏显示鼠标位置
	statusLabel = new QLabel();
	statusLabel->resize(100, 30);
	ui.statusBar->addPermanentWidget(statusLabel);
}

void CG_Painter::refreshStateLabel()
{
	//状态栏展示鼠标位置
	QString str = "(" + QString::number(mouse_x) + "," + QString::number(mouse_y) + ")";
	statusLabel->setText(state_info + algo_info + str);
}

void CG_Painter::setState(PAINTER_STATE newState)
{
	state = newState;
	buf_flag = false;
	update();
	switch (state)
	{
	case CG_Painter::NOT_DRAWING:
		state_info = "";
		algo_info = "";
		break;
	case CG_Painter::DRAW_LINE:
		state_info = u8"状态：画直线 | ";
		line_state = LINE_NON_POINT;
		break;
	case CG_Painter::DRAW_POLYGON:
		state_info = u8"状态：画多边形 | ";
		poly_state = POLY_NON_POINT;
		poly_points.clear();
		poly_id = getNewID();
		break;
	case CG_Painter::DRAW_ELLIPSE:
		state_info = u8"状态：画椭圆 | ";
		algo_info = "";
		elli_state = ELLI_NON_POINT;
		elli_id = getNewID();
		break;
	case CG_Painter::DRAW_ROTATE:
		state_info = u8"状态：旋转图元 | ";
		algo_info = "";
		rotate_state = ROTATE_NON;
		break;
	case CG_Painter::DRAW_SCALE:
		state_info = u8"状态：缩放图元 | ";
		algo_info = "";
		scale_state = SCALE_NON;
		break;
	default:
		break;
	}
	refreshStateLabel();
}

void CG_Painter::setAlgo(ALGORITHM newAlgo)
{
	algorithm = newAlgo;
	switch (newAlgo)
	{
	case DDA:
		algo_info = "DDA | ";
		break;
	case BRESENHAM:
		algo_info = "Bresenham | ";
		break;
	default:
		break;
	}
}

void CG_Painter::mousePressEvent(QMouseEvent * event)
{
	//鼠标当前位置
	int x = event->pos().x();
	int y = event->pos().y();
	mouse_x = x;
	mouse_y = y;

	if (state == NOT_DRAWING) {
		if (event->button() == Qt::LeftButton) {
			if (trans_state == TRANS_NON) {
				trans_ID = myCanvas.getID(x, y);
				if (trans_ID != -1) {
					trans_ix = x; trans_iy = y;
					bufCanvas = myCanvas;
					trans_state = TRANS_BEGIN;
				}
			}
		}
	}
	else if (state == DRAW_ELLIPSE) {
		if (event->button() == Qt::LeftButton) {
			if (elli_state == ELLI_PAINTING) {
				init_x = x; init_y = y;
			}
		}
	}
	else if (state == DRAW_ROTATE) {
		if (event->button() == Qt::LeftButton) {
			if (rotate_state == ROTATE_READY && myCanvas.getID(x, y) != -1) {
				selected_ID = myCanvas.getID(x, y);
				init_x = x; init_y = y;
				rotate_state = ROTATE_BEGIN;
			}
		}
	}
	else if (state == DRAW_SCALE) {
		if (event->button() == Qt::LeftButton) {
			if (scale_state == SCALE_READY && myCanvas.getID(x, y) != -1) {
				selected_ID = myCanvas.getID(x, y);
				init_x = x; init_y = y;
				scale_state = SCALE_BEGIN;
			}
		}
	}

	refreshStateLabel();
}

void CG_Painter::mouseMoveEvent(QMouseEvent * event)
{
	//鼠标当前位置
	int x = event->pos().x();
	int y = event->pos().y();
	mouse_x = x;
	mouse_y = y;

	//设置鼠标形状
	if (buf_flag) {
		if (bufCanvas.getID(x, y) != -1) {
			setCursor(Qt::PointingHandCursor);
		}
		else {
			setCursor(Qt::ArrowCursor);
		}
	}
	else {
		if (myCanvas.getID(x, y) != -1) {
			setCursor(Qt::PointingHandCursor);
		}
		else {
			setCursor(Qt::ArrowCursor);
		}
	}

	
	if (state==NOT_DRAWING) {
		if (trans_state==TRANS_BEGIN) {
			bufCanvas = myCanvas;
			buf_flag = true;
			bufCanvas.translate(trans_ID, x - trans_ix, y - trans_iy);
			update();
		}
	}
	else if (state == DRAW_LINE) {
		if (line_state == LINE_POINTA) {
			bufCanvas = myCanvas;
			buf_flag = true;
			bufCanvas.drawLine(-1, line_Ax, line_Ay, x, y, algorithm);
			update();
		}
	}
	else if (state == DRAW_POLYGON) {
		if (poly_state == POLY_PAINTING) {
			autoPoly(x, y);//自动贴合修正
			bufpoly_points = poly_points;
			bufpoly_points.push_back(Point(x, y));
			bufCanvas.drawPolygon(poly_id, bufpoly_points, algorithm);
			buf_flag = true;
			update();
		}
	}
	else if (state == DRAW_ELLIPSE) {
		if (elli_state == ELLI_PAINTING && (event->buttons() & Qt::LeftButton)) {
			changing_dx = qAbs(x - init_x);
			changing_dy = qAbs(y - init_y);
			changing_dx = ((init_x - cx)*(x - init_x) >= 0) ? changing_dx : (-changing_dx);
			changing_dy = ((init_y - cy)*(y - init_y) >= 0) ? changing_dy : (-changing_dy);
			bufCanvas = myCanvas;
			bufCanvas.drawEllipse(elli_id, cx, cy, rx + changing_dx, ry + changing_dy);
			buf_flag = true;
			update();
		}
	}
	else if (state == DRAW_ROTATE) {
		if (rotate_state == ROTATE_BEGIN && (event->buttons() & Qt::LeftButton)) {
			int r = getRotateR(init_x, init_y, rotate_rx, rotate_ry, x, y);
			bufCanvas = myCanvas;
			bufCanvas.drawDotPoint(-1, rotate_rx, rotate_ry);
			bufCanvas.rotate(selected_ID, rotate_rx, rotate_ry, r);
			buf_flag = true;
			update();
		}
	}
	else if (state == DRAW_SCALE) {
		if (scale_state == SCALE_BEGIN && (event->buttons() & Qt::LeftButton)) {
			double s = getScaleS(init_x, init_y, scale_rx, scale_ry, x, y);
			bufCanvas = myCanvas;
			bufCanvas.drawDotPoint(-1, scale_rx, scale_ry);
			bufCanvas.scale(selected_ID, scale_rx, scale_ry, s);
			buf_flag = true;
			update();
		}
	}
	

	refreshStateLabel();
}


void CG_Painter::mouseReleaseEvent(QMouseEvent * event)
{
	//鼠标当前位置
	int x = event->pos().x();
	int y = event->pos().y();
	mouse_x = x;
	mouse_y = y;

	if (state != NOT_DRAWING) {
		if (event->button() == Qt::RightButton) {
			setState(NOT_DRAWING);
			return;
		}
	}
	
	if (state == NOT_DRAWING) {
		
		if (event->button() == Qt::LeftButton) {
			if (trans_state == TRANS_BEGIN) {
				trans_state = TRANS_NON;
				myCanvas = bufCanvas;
				buf_flag = false;
				update();
			}
		}
		else if(event->button() == Qt::RightButton){
			//正常状态下，右键单击，弹出菜单
			selected_ID = myCanvas.getID(x, y);
			if (selected_ID != -1) {
				//删除图元Action
				QAction* actionDelete = new QAction(tr(u8"删除"));
				actionDelete->setStatusTip(tr(u8"删除此图元"));
				connect(actionDelete, &QAction::triggered, this, &CG_Painter::action_to_delete);
				
				QMenu menu;
				//添加菜单项
				menu.addAction(actionDelete);
				
				//在鼠标位置显示
				menu.exec(QCursor::pos());
			}
		}
	}
	
	if (state == DRAW_LINE) {
		if (event->button() == Qt::LeftButton) {
			if (line_state == LINE_NON_POINT) {
				line_Ax = event->pos().x();
				line_Ay = event->pos().y();
				line_state = LINE_POINTA;
			}
			else if (line_state == LINE_POINTA) {
				line_Bx = event->pos().x();
				line_By = event->pos().y();
				myCanvas.drawLine(getNewID(), line_Ax, line_Ay, line_Bx, line_By, algorithm);
				setState(NOT_DRAWING);
			}
			
		}
	}
	else if (state == DRAW_POLYGON) {
		if (event->button() == Qt::LeftButton) {
			if (poly_state == POLY_NON_POINT) {
				startX = event->pos().x();
				startY = event->pos().y();
				bufCanvas = myCanvas;
				poly_points.push_back(Point(startX, startY));
				poly_state = POLY_PAINTING;
			}
			else if (poly_state == POLY_PAINTING) {
				if (autoPoly(x, y)) {//是否自动贴合判断
					poly_points.push_back(Point(x, y));
					myCanvas.drawPolygon(poly_id, poly_points, algorithm);
					setState(NOT_DRAWING);
				}
				else {
					poly_points.push_back(Point(x, y));
				}
				
			}
		}
		
	}
	else if (state == DRAW_ELLIPSE) {
		if (event->button() == Qt::LeftButton) {
			if (elli_state == ELLI_NON_POINT) {
				cx = x; cy = y;
				rx = DEFAULT_RX, ry = DEFAULT_RY;//长短轴
				bufCanvas = myCanvas;
				bufCanvas.drawEllipse(elli_id, cx, cy, rx, ry);
				buf_flag = true;
				update();
				elli_state = ELLI_PAINTING;
			}
			else if (elli_state == ELLI_PAINTING) {
				rx += changing_dx;
				ry += changing_dy;
			}
		}
	}
	else if (state == DRAW_ROTATE) {
		if (event->button() == Qt::LeftButton) {
			if (rotate_state == ROTATE_NON) {
				rotate_rx = x; rotate_ry = y;
				rotate_state = ROTATE_READY;
				bufCanvas = myCanvas;
				bufCanvas.drawDotPoint(-1, rotate_rx, rotate_ry);
				buf_flag = true;
				update();
			}else if (rotate_state == ROTATE_BEGIN) {
				rotate_state = ROTATE_READY;
				myCanvas = bufCanvas;
			}
		}
	}
	else if (state == DRAW_SCALE) {
		if (event->button() == Qt::LeftButton) {
			if (scale_state == SCALE_NON) {
				scale_rx = x; scale_ry = y;
				scale_state = SCALE_READY;
				bufCanvas = myCanvas;
				bufCanvas.drawDotPoint(-1, scale_rx, scale_ry);
				buf_flag = true;
				update();
			}
			else if (scale_state == SCALE_BEGIN) {
				scale_state = SCALE_READY;
				myCanvas = bufCanvas;
			}
		}
	}
	
	refreshStateLabel();
}

void CG_Painter::mouseDoubleClickEvent(QMouseEvent * event)
{
	//鼠标当前位置
	int x = event->pos().x();
	int y = event->pos().y();

	if (state == DRAW_ELLIPSE) {
		if (event->button() == Qt::LeftButton) {
			if (elli_state == ELLI_PAINTING) {
				myCanvas = bufCanvas;
				setState(NOT_DRAWING);
				update();
			}
		}
	}
}

void CG_Painter::paintEvent(QPaintEvent * event)
{
	QPainter paint(this);
	QImage *image;
	if (!buf_flag) {
		image = new QImage(geometry().width(), geometry().height(), QImage::Format_RGB888);
		myCanvas.getIamge(image);
	}
	else {
		image = new QImage(geometry().width(), geometry().height(), QImage::Format_RGB888);
		bufCanvas.getIamge(image);
	}
	paint.drawImage(0, 0, *image);
	delete image;
}

bool CG_Painter::autoPoly(int & nowx, int & nowy)
{
	if ((nowx - startX)*(nowx - startX) + (nowy - startY)*(nowy - startY) < AUTO_BIAS*AUTO_BIAS) {
		nowx = startX; nowy = startY;
		return true;
	}
	return false;
}

int CG_Painter::getNewID()
{
	return ID_Counter++;
}
