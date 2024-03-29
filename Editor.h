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
	bool selectDotVisible;			// ѡ�е�ɼ�
	int dotRadius;					// ��İ뾶
	int lineWidth;					// �������

	QColor dotColor;				// �����ɫ
	QColor lineColor;				// ������ɫ
	QColor polygonColor;			// �������ɫ
	QColor selectColor;				// ѡ����ɫ

	QPointF dashPoint;				// ���ߵĵ�
	QPointF newPoint;				// ���Զ���б�ߵ�ʱ�������µ������

	QPoint tempPoint;				// ��ʱ��
	QPoint addPt;				// ��ӵ�ʱ����¼��ǰ�ĵ�
	QVector<QPointF> tempPoints;
	QVector<Polygon> tempPolygons;
	// QList<QPointF> tempPoints;		// �㼯��
	// QList<Polygon> tempPolygons;	// ����μ���

	bool pressed;					// ����Ƿ���
	QPointF lastPoint;				// ��갴�´�������
	QPointF ellipsePos;				// ���ְ��µ������
	int selectedEllipseIndex;		// ѡ�е�� index
	Polygon pressedPolygon;			// ���ְ���ʱ����ε�ԭʼ����
	int selectedIndex;				// ѡ�ж���ε� index

	bool isEdit = false;
	bool isShift = false;
	bool isSelect = false;
	bool isPressed = false;
	bool ctrlPressed = false;
	
	

	qreal lastSlope;				// ��һ�ε�б��

	// ��ѡ������
	QPointF selectStartPos;
	QPointF selectEndPos;

	// �洢��ѡ��ĵ�
	QVector<QPointF> selectedPoints;
	QVector<QPointF> selectedPointsIndex;

	// �ƶ�ʱ�ĸ�����
	QPointF auxPt;
	
	
	
private:
	// ��������֮��ľ���
	double length(const QPointF& p1, const QPointF& p2);

	// ����Ƿ�ѡ�ж����
	bool checkPoint(const QVector<QPointF>& points, int x, int y);

	// ����б��
	qreal computeSlope(QPointF start, QPointF end);

	// ���������������� ��̫����
	QPointF computeThirdPt(QPointF first, QPointF second, qreal slope);

	// ����һ�ּ����������ķ���
	qreal distanceBetweenPoints(const QPointF& p1, const QPointF& p2);

	// �ҵ�������λ��������߶�����
	int findNearestLine(const QVector<QPointF>& points, const QPointF& pos);
	
	// �ҵ�������λ��������߶εľ���
	qreal findNearestLineLength(const QVector<QPointF>& points, const QPointF& pos);

	// ����㵽ֱ�ߵľ���
	qreal distanceFromPointToLine(const QPointF& point, const QPointF& linePoint1, const QPointF& linePoint2);

	// ������ȷ���ƶϵ�λ��
	void calRightAnglePt(const QPointF& beforePt, const QPointF& afterPt);

	// ����ͶӰ��
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


	// �����ʱ����
	void clearTemp();

	// �������
	void clearAll();

	// ����ǰѡ�е�ͼ�α���ΪSVG
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
