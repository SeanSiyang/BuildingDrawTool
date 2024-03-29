#ifndef EDITOR_H
#define EDITOR_H

#include <QtWidgets/QMainWindow>
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QFileDialog>
#include <QtSvg/QSvgGenerator>
#include <QToolTip>
#include <QCursor>
#include <QTransform>

#include <iostream>

class CEditor : public QWidget
{
	Q_OBJECT

	Q_PROPERTY(bool selectDotVisible READ getSelectDotVisible WRITE setSelectDotVisible)
	Q_PROPERTY(int dotRadius READ getDotRadius WRITE setDotRadius)
	Q_PROPERTY(int lineWidth READ getLineWidth WRITE setLineWidth)

	Q_PROPERTY(QColor dotColor READ getDotColor WRITE setDotColor)
	Q_PROPERTY(QColor lineColor READ getLineColor WRITE setLineColor)
	Q_PROPERTY(QColor polygonColor READ getPolygonColor WRITE setPolygonColor)
	Q_PROPERTY(QColor selectColor READ getSelectColor WRITE setSelectColor)

public:
	typedef struct {
		QVector<QPointF> pos;
		bool selected;
	} Polygon;

	/*typedef enum
	{
		KEY_STATE_Release	= 0,
		KEY_STATE_Press		= 1,
	} E_KEY_STATE;

	E_KEY_STATE altKey;
	E_KEY_STATE ctrlKey;
	E_KEY_STATE shiftKey;*/

	explicit CEditor(QWidget* parent = nullptr);

protected:
	void mousePressEvent(QMouseEvent* e);
	void mouseMoveEvent(QMouseEvent* e);
	void mouseReleaseEvent(QMouseEvent* e);
	void paintEvent(QPaintEvent* e);
	void drawPolygon(QPainter* p, const Polygon& v);
	// void drawLines(QPainter* p, const QList<QPointF>& list, bool isFirst = true);
	void drawLines(QPainter* p, const QVector<QPointF>& list, bool isFirst = true);
	
	/*void keyPressEvent(QKeyEvent* e);
	void keyReleaseEvent(QKeyEvent* e);*/

private:
	bool selectDotVisible;			// 选中点可见
	int dotRadius;					// 点的半径
	int lineWidth;					// 线条宽度

	QColor dotColor;				// 点的颜色
	QColor lineColor;				// 线条颜色
	QColor polygonColor;			// 多边形颜色
	QColor selectColor;				// 选中颜色

	QPointF dashPoint;				// 虚线的点
	QPointF newPoint;				// 当自定义斜线的时候，生成新的坐标点

	QPoint tempPoint;				// 临时点
	QPoint addPt;				// 添加点时，记录当前的点
	QVector<QPointF> tempPoints;
	QVector<Polygon> tempPolygons;
	// QList<QPointF> tempPoints;		// 点集合
	// QList<Polygon> tempPolygons;	// 多边形集合

	bool pressed;					// 鼠标是否按下
	QPointF lastPoint;				// 鼠标按下处的坐标
	QPointF ellipsePos;				// 保持按下点的坐标
	int selectedEllipseIndex;		// 选中点的 index
	Polygon pressedPolygon;			// 保持按下时多边形的原始坐标
	int selectedIndex;				// 选中多边形的 index

	bool isEdit = false;
	bool isShift = false;
	bool isSelect = false;
	bool isPressed = false;
	bool ctrlPressed = false;
	
	

	qreal lastSlope;				// 上一次的斜率

	// 框选的坐标
	QPointF selectStartPos;
	QPointF selectEndPos;

	// 存储被选择的点
	QVector<QPointF> selectedPoints;
	QVector<QPointF> selectedPointsIndex;

	// 移动时的辅助点
	QPointF auxPt;
	
	
	
private:
	// 计算两点之间的距离
	double length(const QPointF& p1, const QPointF& p2);

	// 检测是否选中多边形
	bool checkPoint(const QVector<QPointF>& points, int x, int y);

	// 计算斜率
	qreal computeSlope(QPointF start, QPointF end);

	// 计算第三个点的坐标 不太管用
	QPointF computeThirdPt(QPointF first, QPointF second, qreal slope);

	// 另外一种计算两点距离的方法
	qreal distanceBetweenPoints(const QPointF& p1, const QPointF& p2);

	// 找到距离光标位置最近的线段索引
	int findNearestLine(const QVector<QPointF>& points, const QPointF& pos);
	
	// 找到距离光标位置最近的线段的距离
	qreal findNearestLineLength(const QVector<QPointF>& points, const QPointF& pos);

	// 计算点到直线的距离
	qreal distanceFromPointToLine(const QPointF& point, const QPointF& linePoint1, const QPointF& linePoint2);

	// 计算正确的推断点位置
	void calRightAnglePt(const QPointF& beforePt, const QPointF& afterPt);

	// 计算投影点
	void calculateProjection(const QPointF& p1, const QPointF& p2, const QPointF& point);

public:
	bool getSelectDotVisible()	const;
	int getDotRadius()			const;
	int getLineWidth()			const;

	QColor getDotColor()		const;
	QColor getLineColor()		const;
	QColor getPolygonColor()	const;
	QColor getSelectColor()		const;

	QSize sizeHint()			const;
	QSize minimumSizeHint()		const;

	int getSelectedIndex()		const;

	void setEditable();

	QRectF normalizedRect();

public slots:
	void setSelectDotVisible(bool selectDotVisible);
	void setDotRadius(int dotRadius);
	void setLineWidth(int lineWidth);

	void setDotColor(const QColor& dotColor);
	void setLineColor(const QColor& lineColor);
	void setPolygonColor(const QColor& polygonColor);
	void setSelectColor(const QColor& selectColor);

	void setShift(const bool checked);
	void setCtrlPressed(const bool checked);


	// 清除临时绘制
	void clearTemp();

	// 清除所有
	void clearAll();

	// 将当前选中的图形保存为SVG
	void saveSvg();

	void deleteSelectedDot();
	void deleteSelectedPolygon();
	void addPoint();
	void useCtrlAddPoint();
	void movingShowDot();


	void selectPts(const bool checked);
	void getPtsFromSelect();
	bool ptInRect(const QPointF& point, const QPointF& topLeft, const QPointF& bottomRight);
	void clearSelectedPts();
};

#endif // EDITOR_H
