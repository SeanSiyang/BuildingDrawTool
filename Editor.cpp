#include "Editor.h"

#include <QPainter>
#include <QMouseEvent>
#include <QtMath>
#include <QDebug>

CEditor::CEditor(QWidget* parent) : QWidget(parent)
{
	selectDotVisible = true;
	dotRadius = 4;
	lineWidth = 2;

	dotColor = QColor(14, 153, 160);
	lineColor = QColor(34, 163, 169);
	polygonColor = QColor(162, 121, 197);
	selectColor = QColor(214, 77, 84);

	pressed = false;
	selectedEllipseIndex = -1;
	selectedIndex = -1;

	// isEdit = false;
	isShift = false;

	setMouseTracking(true);
}

void CEditor::mousePressEvent(QMouseEvent* e)
{
	if (isSelect)
	{
		if (e->button() & Qt::LeftButton)
		{
			isPressed = true;
			selectStartPos = e->pos();
		
			return;
		}
	}
	
	QPoint p = e->pos();
	pressed = true;
	lastPoint = this->mapToGlobal(p);

	// ����ģʽ�²�ѡ��
	if (tempPoints.isEmpty())
	{
		// ���ѡ���ˣ�����Ƿ�㵽�˵���
		bool selectedPot = false;
		selectedEllipseIndex = -1;

		if (selectedIndex != -1)
		{
			for (int i = tempPolygons.at(selectedIndex).pos.size() - 1; i >= 0; --i)
			{
				if (length(p, tempPolygons.at(selectedIndex).pos[i]) <= 100)
				{
					selectedPot = true;
					selectedEllipseIndex = i;
					ellipsePos = tempPolygons.at(selectedIndex).pos[i];
					break;
				}
			}
		}

		// ��ǰѡ���˵㣬�����ػ�
		if (selectedPot)
			return;

		// �ж��Ƿ�ѡ����һ��ͼԪ
		selectedIndex = -1;
		for (int i = tempPolygons.size() - 1; i >= 0; --i)
		{
			// qDebug() << "tempPolygons.at(i).pos:" << tempPolygons.at(i).pos;
			tempPolygons[i].selected = checkPoint(tempPolygons.at(i).pos, p.x(), p.y());

			if (tempPolygons.at(i).selected)
			{
				// ��ֹ�ص�����
				if (selectedIndex == -1)
				{
					selectedIndex = i;
					pressedPolygon = tempPolygons.at(i);
				}
				else
				{
					tempPolygons[i].selected = false;
				}
			}
		}
		this->update();	// ���»���
	}
}

void CEditor::mouseMoveEvent(QMouseEvent* e)
{
	if (isSelect)
	{
		selectEndPos = e->pos();
		update();
		return;
	}

	tempPoint = e->pos();
	addPt = e->pos();

	if (pressed && selectedIndex != -1)
	{
		QPointF delta = this->mapToGlobal(tempPoint) - lastPoint;
		int len = tempPolygons.at(selectedIndex).pos.size();

		if (selectedEllipseIndex != -1)		// �ƶ���
		{
			tempPolygons[selectedIndex].pos[selectedEllipseIndex] = ellipsePos + delta;
			setCursor(Qt::ClosedHandCursor);
			/*if (!selectedPoints.isEmpty())
			{
				selectedPoints.clear();
				update();
			}*/
			qreal len = length(tempPolygons[selectedIndex].pos[selectedEllipseIndex], auxPt);
			qDebug() << "len: " << len;
			if (len < 50)
			{
				tempPolygons[selectedIndex].pos[selectedEllipseIndex] = auxPt;
			}

		}
		else if (selectedIndex != -1)		// �ƶ���
		{
			for (int i = 0; i < len; ++i)
			{
				tempPolygons[selectedIndex].pos[i] = pressedPolygon.pos.at(i) + delta;
			}
			// ����������ƶ�����ѡ��ĵ����
			setCursor(Qt::ArrowCursor);
			
			/*if (!selectedPoints.isEmpty())
			{
				selectedPoints.clear();
				update();
			}*/
		}
		/*else if (ctrlPressed == true)
		{
			setCursor(Qt::CrossCursor);
			
		}*/
		else 
		{
			setCursor(Qt::ArrowCursor);
			
		}
	}

	if (ctrlPressed == true && selectedIndex != -1)
	{
		setCursor(Qt::CrossCursor);
	}

	if (tempPoints.size() >= 1) // ��ʱ����dashPoint
	{
		if (!isShift) // ��dashPoint�������һ����
		{
			QPointF p1 = tempPoints.last();
			QLineF line1(p1, dashPoint);
			QLineF line2(p1, tempPoint);
			qreal angle = line1.angleTo(line2);
			qDebug() << "angle is : " << angle;
			
			if (angle < 10)
			{
				setCursor(Qt::CrossCursor);
			}
				
			else if (angle > 350)
			{
				setCursor(Qt::CrossCursor);
			}
				
			else
			{
				setCursor(Qt::ArrowCursor);
			}
				
		}
		//else if (ctrlPressed == true)
		//{
		//	setCursor(Qt::CrossCursor);
		//	// update();
		//}
		else
		{
			setCursor(Qt::ArrowCursor);
		}
		
	}
	this->update();
}

void CEditor::mouseReleaseEvent(QMouseEvent* e)
{
	if (isSelect)
	{
		if (e->button() == Qt::LeftButton)
		{
			selectEndPos = e->pos();

			qDebug() << "selectStartPos: " << selectStartPos;
			qDebug() << "selectEndPos: " << selectEndPos;


			isPressed = false;
			// �����ѡ����Щ��
			getPtsFromSelect();
			update();
			return;
		}
	}


	// ����Ҽ������ʱ��
	if (e->button() == Qt::RightButton)
	{
		clearTemp();
		selectedIndex = -1;
		selectedEllipseIndex = -1;
		return;
	}

	pressed = false;
	if (selectedIndex != -1)
		return;
	
	QPoint point = e->pos();
	/*foreach(const QPointF & item, tempPoints)
	{
		qDebug() << "x: " << item.x() << " y: " << item.y();
	}*/

	if (tempPoints.count() > 0)
	{
		qreal len = (qPow(tempPoints.first().x() - point.x(), 2.0) + qPow(tempPoints.first().y() - point.y(), 2.0));
		
		if (len < 100)
		{
			// ���һ�������
			if (tempPoints.size() >= 3)
			{
				Polygon pol;
				// pol.pos = tempPoints.toVector();
				pol.pos = tempPoints;
				pol.selected = false;
				tempPolygons.append(pol);
			}

			tempPoints.clear();
			this->update();
			return;
		}
	}

	if (tempPoints.size() >= 1)	// ��ʱ����dashPoint ���� newPoint
	{
		if (!isShift)
		{
			tempPoints.append(dashPoint);
		}
		else 
		{
			tempPoints.append(point);
		}
	}
	else
	{
		tempPoints.append(point);
	}

	this->update();
}

void CEditor::paintEvent(QPaintEvent* e)
{
	/*if (tempPolygons.isEmpty())
		return;*/
	/*if (tempPolygons.count() == 1)
		return;*/
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing, true);

	// ���ƶ����
	foreach(const Polygon &p, tempPolygons)
	{
		drawPolygon(&painter, p);
	}

	// ���Ƶ㼯��
	drawLines(&painter, tempPoints, false);

	if (isPressed) // ���ƹ����в���Ҫ�����ڵ������˳�򣬻�ȡ���ʱ����Ҫ������Χ
	{
		QRectF selectionRect = QRectF(selectStartPos, selectEndPos).normalized();
		
		painter.drawRect(selectionRect);
		painter.fillRect(selectionRect, QColor(0, 120, 215, 50));
	}
	
	// ���Ʊ�ѡ�еĵ�
	if (!selectedPoints.isEmpty())
	{
		painter.setPen(Qt::red);
		painter.setBrush(Qt::white);
		for (const QPointF& point : selectedPoints) 
		{
			painter.drawEllipse(point, dotRadius, dotRadius);
		}	

	}
}

void CEditor::drawPolygon(QPainter* p, const Polygon& v)
{
	p->save();

	// ���ƶ����
	p->setPen(QPen(lineColor, lineWidth));
	v.selected ? p->setBrush(selectColor) : p->setBrush(polygonColor);
	p->drawPolygon(v.pos.data(), v.pos.size());

	// ����Բ��
	if (selectDotVisible && v.selected)
	{
		p->setPen(Qt::NoPen);
		p->setBrush(dotColor);

		foreach(const QPointF& point, v.pos)
		{
			p->drawEllipse(point, dotRadius, dotRadius);
			if (selectedEllipseIndex != -1)
			{
				// setCursor(Qt::ClosedHandCursor);
				QPen pen(Qt::green);
				pen.setStyle(Qt::DashLine);
				p->setPen(pen);
				
				p->drawLine(tempPolygons[selectedIndex].pos[selectedEllipseIndex].x(), tempPolygons[selectedIndex].pos[selectedEllipseIndex].y(),
					tempPolygons[selectedIndex].pos[selectedEllipseIndex].x(), tempPolygons[selectedIndex].pos[selectedEllipseIndex].y() + 200);
				p->drawLine(tempPolygons[selectedIndex].pos[selectedEllipseIndex].x(), tempPolygons[selectedIndex].pos[selectedEllipseIndex].y(),
					tempPolygons[selectedIndex].pos[selectedEllipseIndex].x(), tempPolygons[selectedIndex].pos[selectedEllipseIndex].y() - 200);

				p->drawLine(tempPolygons[selectedIndex].pos[selectedEllipseIndex].x(), tempPolygons[selectedIndex].pos[selectedEllipseIndex].y(),
					tempPolygons[selectedIndex].pos[selectedEllipseIndex].x() + 200, tempPolygons[selectedIndex].pos[selectedEllipseIndex].y());
				p->drawLine(tempPolygons[selectedIndex].pos[selectedEllipseIndex].x(), tempPolygons[selectedIndex].pos[selectedEllipseIndex].y(),
					tempPolygons[selectedIndex].pos[selectedEllipseIndex].x() - 200, tempPolygons[selectedIndex].pos[selectedEllipseIndex].y());

				p->setPen(Qt::NoPen);

				// �϶�ʱ��ʾ����������
				QPen auxPen(Qt::darkGray);
				auxPen.setStyle(Qt::DashLine);
				p->setPen(auxPen);
				p->setBrush(QBrush(Qt::transparent));
				movingShowDot();	// ���¸����������
				
				// ���Ƹ�����
				int sideLength = 10;
				int topLeftX = auxPt.x() - sideLength / 2;
				int topLeftY = auxPt.y() - sideLength / 2;
				p->drawRect(topLeftX, topLeftY, sideLength, sideLength);
				p->setPen(Qt::NoPen);
				p->setBrush(Qt::NoBrush);

				// �������Ѿ�֪���ˣ������������������ֱ��
				// ���ѡȡ�Ķ���εĵ���Ϣ
				QVector<QPointF>& points = tempPolygons[selectedIndex].pos;
				// ͳ�Ƶ���
				int count = points.count();

				// �϶�ʱ�ĸ�����
				QLineF firstLine;
				QLineF secondLine;
				// ��Ҫ�жϵ�ǰ���Ƿ������һ���㣬�Ƿ��ǵ�һ����
				if (selectedEllipseIndex == (count - 1))	// �����һ�����ʱ��Ҫ������ж�
				{
					int before = selectedEllipseIndex - 1;
					int after = 0;

					QPointF beforePt = points[before];
					QPointF afterPt = points[after];

					firstLine = QLineF(beforePt, auxPt);
					secondLine = QLineF(afterPt, auxPt);

				}
				else if (selectedEllipseIndex == 0)			// ��һ����
				{
					int before = count - 1;
					int after = selectedEllipseIndex + 1;

					QPointF beforePt = points[before];
					QPointF afterPt = points[after];

					firstLine = QLineF(beforePt, auxPt);
					secondLine = QLineF(afterPt, auxPt);
				}
				else    // ����������һ���㣬Ҫ��õ�ǰ���ǰ�������������
				{
					int before = selectedEllipseIndex - 1;
					int after = selectedEllipseIndex + 1;

					// ��ȡ����Ϣ
					QPointF beforePt = points[before];
					QPointF afterPt = points[after];

					firstLine = QLineF(beforePt, auxPt);
					secondLine = QLineF(afterPt, auxPt);
				}
				// ����ֱ��
				QPen linePen(Qt::red);
				linePen.setStyle(Qt::DashLine);
				p->setPen(linePen);

				p->drawLine(firstLine);
				p->drawLine(secondLine);
				p->setPen(Qt::NoPen);
				p->setBrush(Qt::NoBrush);
			}
			else if (ctrlPressed == true)
			{
				setCursor(Qt::CrossCursor);
				// update();
			}
			else
			{
				setCursor(Qt::ArrowCursor);
			}
		}
	}
	p->restore();
}

void CEditor::drawLines(QPainter* p, const QVector<QPointF>& list, bool isFirst)
{
	p->save();

	int count = list.count();
	if (count > 0)
	{
		// ���Ƶ㼯��
		p->setPen(Qt::NoPen);
		p->setBrush(dotColor);

		// ����ÿ����
		for (int i = 0; i < count; ++i)
		{
			p->drawEllipse(list.at(i), dotRadius, dotRadius);
		}

		// ������������
		p->setPen(QPen(lineColor, lineWidth));
		p->setBrush(Qt::NoBrush);

		for (int i = 0; i < count - 1; ++i)
		{
			p->drawLine(list.at(i), list.at(i + 1));
		}

		p->drawLine(list.last(), isFirst ? list.first() : tempPoint);
		
		// qDebug() << "isShift condition: " << isShift;

		if (!isShift)
		{
			// ��������
			QPen pen(Qt::blue);
			pen.setStyle(Qt::DashLine);
			p->setPen(pen);
			qreal slope = computeSlope(list.last(), tempPoint);
			
			if (slope > 0) 
			{
				if (slope < 1)	// x��������
				{
					dashPoint = QPointF(tempPoint.x(), list.last().y());
					p->drawLine(list.last(), dashPoint);

					p->setPen(Qt::black);
					p->setBrush(dotColor);
					p->drawEllipse(dashPoint, dotRadius, dotRadius);
				}
				else            // y��������
				{
					dashPoint = QPointF(list.last().x(), tempPoint.y());
					p->drawLine(list.last(), dashPoint);

					p->setPen(Qt::black);
					p->setBrush(dotColor);
					p->drawEllipse(dashPoint, dotRadius, dotRadius);
				}
			}
			else if (slope < 0) 
			{
				if (slope > -1) // x�Ḻ����
				{
					dashPoint = QPointF(tempPoint.x(), list.last().y());
					p->drawLine(list.last(), dashPoint);
					
					p->setPen(Qt::black);
					p->setBrush(dotColor);
					p->drawEllipse(dashPoint, dotRadius, dotRadius);
				}
				else			// y�Ḻ����
				{
					dashPoint = QPointF(list.last().x(), tempPoint.y());
					p->drawLine(list.last(), dashPoint);

					p->setPen(Qt::black);
					p->setBrush(dotColor);
					p->drawEllipse(dashPoint, dotRadius, dotRadius);
				}
			}
		}
		
		if (isShift && count > 1)	// ����shift ��������Ҫ���¼���
		{	
			// second way is that we rotate the line and paint cross lines
			QPointF last2Point;
			if (count == 2)
			{
				last2Point = list.first();
			}
			else if (count > 2)
			{
				last2Point = list.at(count - 2);
			}
			
			/*qDebug() << "last2Point: " << last2Point.x() << "  " << last2Point.y();
			qDebug() << "list.last(): " << list.last().x() << "  " << list.last().y();*/

			qreal slope = computeSlope(last2Point, list.last());	// ǰһ��б��

			float length = 100;
			float angle = atan(slope);		// �����ߵ���б�Ƕ�

			float pSlope = -1.0 / slope;

			QPen pen(Qt::red);
			pen.setStyle(Qt::DashLine);
			p->setPen(pen);

			// ������
			/*p->drawLine(list.last().x() - length * cos(angle), list.last().y() - length * sin(angle),
				list.last().x() + length * cos(angle), list.last().y() + length * sin(angle));
			
			p->drawLine(list.last().x() - length, list.last().y() - length * pSlope,
				list.last().x() + length, list.last().y() + length * pSlope);*/
			
		
			/*qreal newSlope = computeSlope(list.last(), tempPoint);
			qreal intersectX = (pSlope * list.last().x() - newSlope * list.last().x() + list.last().y() - tempPoint.y()) /
				(pSlope - newSlope);
			qreal intersectY = pSlope * (intersectX - list.last().x()) + list.last().y();*/

			// ��������ֱ�ߵ�б��
			QLineF line1 = QLineF(last2Point, list.last());
			QLineF line2 = QLineF(list.last(), tempPoint);

			qreal angleDegrees = line1.angleTo(line2);

			// ������һ��ֱ����x��ļн�
			/*qreal angle2x = qAtan2(last2Point.y() - list.last().y(), last2Point.x() - list.last().x());
			qreal rotationAngle = qRadiansToDegrees(-angle2x);
			QTransform transform;
			transform.rotate(rotationAngle);

			QPointF rotatedP1 = transform.map(last2Point);
			QPointF rotatedP2 = transform.map(list.last());

			qDebug() << "rotatedP1" << rotatedP1;
			qDebug() << "rotatedP2" << rotatedP2;

			p->setPen(Qt::black);
			p->setBrush(dotColor);

			p->drawEllipse(rotatedP1, dotRadius, dotRadius);
			p->drawEllipse(rotatedP2, dotRadius, dotRadius);*/

			
			QLineF auxLine;
			// ���ݼнǴ�С���ж����ĸ���
			if (angleDegrees >= 0 && angleDegrees <= 45)		// x���� 
			{
				qDebug() << "x++++++++++++++++++";
				// ��ת��xy�Ƕ�
				// transform
				p->drawLine(list.last().x(), list.last().y(),
					list.last().x() + length * cos(angle), list.last().y() + length * sin(angle));

				auxLine.setLine(list.last().x(), list.last().y(),
					list.last().x() + length * cos(angle), list.last().y() + length * sin(angle));
				
				
				calculateProjection(auxLine.p1(), auxLine.p2(), tempPoint);
				p->setPen(Qt::black);
				p->setBrush(Qt::blue);
				p->drawEllipse(newPoint, dotRadius, dotRadius);
			}
			else if (angleDegrees > 45 && angleDegrees <= 90)	// y����
			{
				qDebug() << "y++++++++++++++++++";
				p->drawLine(list.last().x(), list.last().y(),
					list.last().x() + length, list.last().y() + length * pSlope);

				auxLine.setLine(list.last().x(), list.last().y(),
					list.last().x() + length, list.last().y() + length * pSlope);

				calculateProjection(auxLine.p1(), auxLine.p2(), tempPoint);
				p->setPen(Qt::black);
				p->setBrush(Qt::blue);
				p->drawEllipse(newPoint, dotRadius, dotRadius);
			}
			else if (angleDegrees > 90 && angleDegrees <= 135)	// y����
			{
				qDebug() << "y++++++++++++++++++";
				p->drawLine(list.last().x(), list.last().y(),
					list.last().x() + length, list.last().y() + length * pSlope);

				auxLine.setLine(list.last().x(), list.last().y(),
					list.last().x() + length, list.last().y() + length * pSlope);

				calculateProjection(auxLine.p1(), auxLine.p2(), tempPoint);
				p->setPen(Qt::black);
				p->setBrush(Qt::blue);
				p->drawEllipse(newPoint, dotRadius, dotRadius);
			}
			else if (angleDegrees >= 135 && angleDegrees < 180)	// x�Ḻ
			{
				qDebug() << "x------------------";
				p->drawLine(list.last().x() - length * cos(angle), list.last().y() - length * sin(angle),
					list.last().x(), list.last().y());

				auxLine.setLine(list.last().x() - length * cos(angle), list.last().y() - length * sin(angle),
					list.last().x(), list.last().y());

				calculateProjection(auxLine.p1(), auxLine.p2(), tempPoint);
				p->setPen(Qt::black);
				p->setBrush(Qt::blue);
				p->drawEllipse(newPoint, dotRadius, dotRadius);
			}
			else if (angleDegrees >= 180 && angleDegrees < 225)
			{
				qDebug() << "x------------------";
				p->drawLine(list.last().x() - length * cos(angle), list.last().y() - length * sin(angle),
					list.last().x(), list.last().y());

				auxLine.setLine(list.last().x() - length * cos(angle), list.last().y() - length * sin(angle),
					list.last().x(), list.last().y());

				calculateProjection(auxLine.p1(), auxLine.p2(), tempPoint);
				p->setPen(Qt::black);
				p->setBrush(Qt::blue);
				p->drawEllipse(newPoint, dotRadius, dotRadius);
			}
			else if (angleDegrees >= 225 && angleDegrees < 270)
			{
				qDebug() << "y------------------";
				p->drawLine(list.last().x() - length, list.last().y() - length * pSlope,
					list.last().x(), list.last().y()); 

				auxLine.setLine(list.last().x() - length, list.last().y() - length * pSlope,
					list.last().x(), list.last().y());

				calculateProjection(auxLine.p1(), auxLine.p2(), tempPoint);
				p->setPen(Qt::black);
				p->setBrush(Qt::blue);
				p->drawEllipse(newPoint, dotRadius, dotRadius);
			}
			else if (angleDegrees >= 270 && angleDegrees < 315)
			{
				qDebug() << "y------------------";
				p->drawLine(list.last().x() - length, list.last().y() - length * pSlope,
					list.last().x(), list.last().y());

				auxLine.setLine(list.last().x() - length, list.last().y() - length * pSlope,
					list.last().x(), list.last().y());

				calculateProjection(auxLine.p1(), auxLine.p2(), tempPoint);
				p->setPen(Qt::black);
				p->setBrush(Qt::blue);
				p->drawEllipse(newPoint, dotRadius, dotRadius);
			}
			else if (angleDegrees >= 315 && angleDegrees < 360)
			{
				qDebug() << "x++++++++++++++++++";
				p->drawLine(list.last().x(), list.last().y(),
					list.last().x() + length * cos(angle), list.last().y() + length * sin(angle));

				auxLine.setLine(list.last().x(), list.last().y(),
					list.last().x() + length * cos(angle), list.last().y() + length * sin(angle));

				calculateProjection(auxLine.p1(), auxLine.p2(), tempPoint);
				p->setPen(Qt::black);
				p->setBrush(Qt::blue);
				p->drawEllipse(newPoint, dotRadius, dotRadius);
			}

			// p->drawEllipse(QPointF(intersectX, intersectY), dotRadius, dotRadius);


			/*qreal x = (slope * list.last().x() - tempPoint.y() - pSlope * tempPoint.x()) / (slope - pSlope);
			qreal y = pSlope * (x - list.last().x()) + tempPoint.y();
			QPointF intersection(x, y);
			
			p->setPen(Qt::black);
			p->setBrush(dotColor);

			p->drawEllipse(intersection, dotRadius, dotRadius);*/

			/*float projectionX = (pSlope * list.last().x() - slope * tempPoint.x() + tempPoint.y() - list.last().y() / (pSlope - slope));
			float projectionY = slope * (projectionX - tempPoint.x()) + tempPoint.y();
			qDebug() << "projectionX: " << projectionX;
			qDebug() << "projectionY: " << projectionY;

			p->setPen(Qt::black);
			p->setBrush(dotColor);

			p->drawEllipse(projectionX, projectionY, dotRadius, dotRadius);
			*/
			this->update();
		}
		else if (count < 1)
		{
			return;
		}
	}

	p->restore();
}

double CEditor::length(const QPointF& p1, const QPointF& p2)
{
	return qPow(p1.x() - p2.x(), 2.0) + qPow(p1.y() - p2.y(), 2.0);	// ƽ����
}

bool CEditor::checkPoint(const QVector<QPointF>& points, int testx, int testy)
{
	// ���ٱ�֤3����
	const int count = points.size();
	if (count < 3)
	{
		return false;
	}

	QList<float> vertx, verty;
	for (int i = 0; i < count; ++i)
	{
		vertx << points.at(i).x();
		verty << points.at(i).y();
	}

	// �����㷨�����������Ƿ��ڶ�����ڲ�
	int i = 0, j, c = 0;
	for (i = 0, j = count - 1; i < count; j = i++)
	{
		bool b1 = (verty.at(i) > testy) != (verty.at(j) > testy);
		bool b2 = (testx < (vertx.at(j) - vertx.at(i))* (testy - verty.at(i)) / (verty.at(j) - verty.at(i)) + vertx.at(i));
		if (b1 && b2)
		{
			c = !c;
		}
	}

	return c;
}

qreal CEditor::computeSlope(QPointF start, QPointF end)
{
	qreal slope = (end.y() - start.y()) / (end.x() - start.x());

	return slope;
}

// �޷������ĸ�����
QPointF CEditor::computeThirdPt(QPointF first, QPointF second, qreal slope)
{

	qreal x1 = first.x(), y1 = first.y();
	qreal x2 = second.x(), y2 = second.y();

	qreal resultX = (1.0 / slope) * x1 + (slope * x2) + y1 - y2;
	qreal resultY = slope * (resultX - x2) + y2;

	return QPointF(resultX, resultY);
}

qreal CEditor::distanceBetweenPoints(const QPointF& p1, const QPointF& p2)
{
	return qSqrt(qPow(p2.x() - p1.x(), 2) + qPow(p2.y() - p1.y(), 2));
}

int CEditor::findNearestLine(const QVector<QPointF>& points, const QPointF& pos)
{
	qreal minDistance = std::numeric_limits<qreal>::max();
	int index = -1;

	for (int i = 0; i < points.size(); i++)
	{
		qreal dist;
		//if (i == points.size() - 1)		// ĩβ������ʼ��
		//{
		//	dist = distanceBetweenPoints(pos, points[i]) + distanceBetweenPoints(pos, points[0]);
		//}
		//else 
		//{
		//	dist = distanceBetweenPoints(pos, points[i]) + distanceBetweenPoints(pos, points[i + 1]);
		//}
		//
		// �������굽ֱ�ߵľ���
		const QPointF& p1 = points[i];
		
		const QPointF& p2 = (i == (points.size() - 1)) ? points[0] : points[i + 1];

		qDebug() << "p1: " << p1;
		qDebug() << "p2: " << p2;
			// points[(i + 1) % points.size()];	// ѭ��������β

		dist = distanceFromPointToLine(pos, p1, p2);

		if (dist < minDistance)
		{
			minDistance = dist;
			index = i;
		}
	}

	return index;
}

qreal CEditor::findNearestLineLength(const QVector<QPointF>& points, const QPointF& pos)
{
	qreal minDistance = std::numeric_limits<qreal>::max();
	int index = -1;

	for (int i = 0; i < points.size(); i++)
	{
		qreal dist;
		// �������굽ֱ�ߵľ���
		const QPointF& p1 = points[i];

		const QPointF& p2 = (i == (points.size() - 1)) ? points[0] : points[i + 1];

		/*qDebug() << "p1: " << p1;
		qDebug() << "p2: " << p2;*/
		
		dist = distanceFromPointToLine(pos, p1, p2);

		if (dist < minDistance)
		{
			minDistance = dist;
			index = i;
		}
	}

	return minDistance;
}

qreal CEditor::distanceFromPointToLine(const QPointF& point, const QPointF& linePoint1, const QPointF& linePoint2)
{
	double A = linePoint2.y() - linePoint1.y();
	double B = linePoint1.x() - linePoint2.x();
	double C = linePoint2.x() * linePoint1.y() - linePoint1.x() * linePoint2.y();

	qreal distance = std::abs(A * point.x() + B * point.y() + C) / std::sqrt(A * A + B * B);

	return distance;
}

void CEditor::calRightAnglePt(const QPointF& beforePt, const QPointF& afterPt)
{
	if (beforePt.x() <= afterPt.x() && beforePt.y() <= afterPt.y())
	{
		auxPt.setX(beforePt.x());
		auxPt.setY(afterPt.y());
	}
	else if (beforePt.x() <= afterPt.x() && beforePt.y() > afterPt.y())
	{
		auxPt.setX(afterPt.x());
		auxPt.setY(beforePt.y());
	}
	else if (beforePt.x() > afterPt.x() && beforePt.y() > afterPt.y())
	{
		auxPt.setX(beforePt.x());
		auxPt.setY(afterPt.y());
	}
	else if (beforePt.x() > afterPt.x() && beforePt.y() < afterPt.y())
	{
		auxPt.setX(afterPt.x());
		auxPt.setY(beforePt.y());
	}

	return;
}

void CEditor::calculateProjection(const QPointF& p1, const QPointF& p2, const QPointF& point)
{
	// ����ֱ�ߵ�б��
	qreal m = (p2.y() - p1.y()) / (p2.x() - p1.x());

	// ����ֱ�ߵĽؾ�
	qreal b = p1.y() - m * p1.x();

	// ����ͶӰ��� x ����
	qreal projectedX = (point.x() + m * (point.y() - b)) / (m * m + 1);

	// ����ͶӰ��� y ����
	qreal projectedY = m * projectedX + b;

	newPoint.setX(projectedX);
	newPoint.setY(projectedY);

}
	

//QPointF CEditor::calRightAnglePt(const QPointF& a, const QPointF& b, const QPointF& c)
//{
//	QPointF ab = b - a;
//
//	
//	QPointF bc = c - b;
//
//
//	QPointF abp(-ab.y(), ab.x());
//	QPointF bcp(-bc.y(), bc.x());
//
//	
//	QPointF cp = b + 0.5 * (abp + bcp);
//
//	return cp;
//}

bool CEditor::getSelectDotVisible() const
{
	return this->selectDotVisible;
}

int CEditor::getDotRadius() const
{
	return this->dotRadius;
}

int CEditor::getLineWidth() const
{
	return this->lineWidth;
}

QColor CEditor::getDotColor() const
{
	return this->dotColor;
}

QColor CEditor::getLineColor() const
{
	return this->lineColor;
}

QColor CEditor::getPolygonColor() const
{
	return this->polygonColor;
}

QColor CEditor::getSelectColor() const
{
	return this->selectColor;
}

QSize CEditor::sizeHint() const
{
	return QSize(200, 100);
}

QSize CEditor::minimumSizeHint() const
{
	return QSize(20, 10);
}

int CEditor::getSelectedIndex() const
{
	return selectedIndex;
}

// �޸Ŀɱ༭��״̬
void CEditor::setEditable()
{
	if (isEdit == true)
		isEdit = false;
	else
		isEdit = true;
}

// �����������ϽǺ����½ǵ��������Ϣ
QRectF CEditor::normalizedRect()
{
	float topLeftX = qMin(selectStartPos.x(), selectEndPos.x());
	float topLeftY = qMin(selectStartPos.y(), selectEndPos.y());

	float bottomRightX = qMax(selectStartPos.x(), selectEndPos.x());
	float bottomRightY = qMax(selectStartPos.y(), selectEndPos.y());
	
	selectStartPos.setX(topLeftX);
	selectStartPos.setY(topLeftY);
	selectEndPos.setX(bottomRightX);
	selectEndPos.setY(bottomRightY);

	/*QPointF topLeft(topLeftX, topLeftY);
	QPointF bottomRight(bottomRightX, bottomRightY);*/
	
	//qDebug() << "selectStartPos: " << selectStartPos;
	//qDebug() << "selectEndPos: " << selectEndPos;

	QRectF rect = QRectF(selectStartPos, selectEndPos).normalized();
	
	return rect;
}

void CEditor::setSelectDotVisible(bool selectDotVisible)
{
	if (this->selectDotVisible != selectDotVisible)
	{
		this->selectDotVisible = selectDotVisible;
		this->update();
	}
}

void CEditor::setDotRadius(int dotRadius)
{
	if (this->dotRadius != dotRadius)
	{
		this->dotRadius = dotRadius;
		this->update();
	}
}

void CEditor::setLineWidth(int lineWidth)
{
	if (this->lineWidth != lineWidth)
	{
		this->lineWidth = lineWidth;
		this->update();
	}
}

void CEditor::setDotColor(const QColor& dotColor)
{
	if (this->dotColor != dotColor)
	{
		this->dotColor = dotColor;
		this->update();
	}
}

void CEditor::setLineColor(const QColor& lineColor)
{
	if (this->lineColor != lineColor)
	{
		this->lineColor = lineColor;
		this->update();
	}
}

void CEditor::setPolygonColor(const QColor& polygonColor)
{
	if (this->polygonColor != polygonColor)
	{
		this->polygonColor = polygonColor;
		this->update();
	}
}

void CEditor::setSelectColor(const QColor& selectColor)
{
	if (this->selectColor != selectColor)
	{
		this->selectColor = selectColor;
		this->update();
	}
}

void CEditor::setShift(const bool checked)
{
	isShift = checked;
}

void CEditor::setCtrlPressed(const bool checked)
{
	ctrlPressed = checked;
}

void CEditor::clearTemp()
{
	tempPoints.clear();
	update();
}

void CEditor::clearAll()
{
	//qDeleteAll(tempPolygons.begin(), tempPolygons.end());
	tempPolygons.clear();
	
	/*int count = tempPolygons.count();
	qDebug() << "count: " << count;
	for (int i = 0; i < count; i++)
	{
		tempPolygons.removeAt(i);
	}
	tempPolygons.clear();*/
}

void CEditor::saveSvg()
{
	if (selectedIndex == -1)
	{
		return;
	}

	QString filePath = QFileDialog::getSaveFileName(this, "Save SVG", "", "SVG files (*.svg)");
	if (filePath.isEmpty())
	{
		qDebug() << "The file path is ERROR!!";
		return;
	}

	QSvgGenerator generator;
	generator.setFileName(filePath);
	generator.setSize(QSize(this->width(), this->height()));
	generator.setViewBox(QRect(0, 0, this->width(), this->height()));
	generator.setTitle("SVG");
	generator.setDescription("This SVG file is generated by Qt.");

	CEditor::Polygon polygon = tempPolygons.at(selectedIndex);

	QVector<QPointF> points = polygon.pos;

	QPainter painter;
	painter.begin(&generator);

	// ���ƶ����
	painter.setRenderHints(QPainter::Antialiasing);
	painter.setPen(Qt::red);
	painter.setBrush(Qt::green);
	painter.drawPolygon(points.data(), points.size());

	painter.end();
}

// ɾ���ڵ�
void CEditor::deleteSelectedDot()
{
	if (selectedIndex != -1 && selectedEllipseIndex != -1)
	{
		tempPolygons[selectedIndex].pos.remove(selectedEllipseIndex);
		this->update();
	}
}

// ɾ��ѡ��Ķ����
void CEditor::deleteSelectedPolygon()
{
	if (selectedIndex != -1)
	{
		tempPolygons.remove(selectedIndex);
		selectedIndex = -1;

		if (!selectedPoints.isEmpty())	// ����Ѿ�ѡ���˵㣬������Ҫɾ�����������ʱ��Ҫ��ѡ��ĵ���գ�����滭ͣ��������
		{
			selectedPoints.clear();
			update();
		}
	}
}

// �������߶�������µĵ�
void CEditor::addPoint()
{
	if (selectedIndex != -1)	// ȷ��Ҫ�Ѿ�ѡ���˶����
	{
		// QPointF pos = QCursor::pos();	// ��ȡ��ǰ���ĵ�
		// pos = this->mapToGlobal(pos);
		qDebug() << "add point pos is " << addPt;	
		QVector<QPointF>& points = tempPolygons[selectedIndex].pos;

		int nearestIndex = findNearestLine(points, addPt);

		if (nearestIndex != -1)
		{
			if (nearestIndex < 0 || nearestIndex > points.size() - 1)
			{
				qDebug() << "Invalid index";
				return;
			}

			for (int i = 0; i < points.size(); i++)
			{
				qDebug() << "Point " << i << ": " << points[i];
			}

			points.insert(nearestIndex + 1, addPt);
			update();
		}
		else
		{
			qDebug() << "No valid line found for the cursor position";
		}
	}
}

void CEditor::useCtrlAddPoint()
{
	if (selectedIndex != -1)	// ȷ��Ҫ�Ѿ�ѡ���˶����
	{
		// �жϵ�ǰ�����ֱ�ߵľ���
		// ��������ʱ������ΪCross
		QVector<QPointF>& points = tempPolygons[selectedIndex].pos;

		int nearestIndex = findNearestLine(points, tempPoint);

	

		if (nearestIndex != -1)
		{
			if (nearestIndex < 0 || nearestIndex > points.size() - 1)
			{
				qDebug() << "Invalid index";
				return;
			}

			for (int i = 0; i < points.size(); i++)
			{
				qDebug() << "Point " << i << ": " << points[i];
			}

			points.insert(nearestIndex + 1, addPt);
		}
		else
		{
			qDebug() << "No valid line found for the cursor position";
		}
	}
}


/// <summary>
/// ���϶�����ʱ���ṩ�ĸ�������
/// ��Ҫ�ж��Ƿ������һ����
/// </summary>
void CEditor::movingShowDot()
{
	if (selectedIndex != -1 && selectedEllipseIndex != -1)
	{
		QVector<QPointF>& points = tempPolygons[selectedIndex].pos;
		int count = points.count();

		/*for (int i = 0; i < count; i ++)
		{
			qDebug() << "The point is" << i << " is " << points[i];
		}*/
		
		// ��Ҫ�ж��ƶ��ĵ��Ƿ������һ����
		if (selectedEllipseIndex == (count - 1))	// �����һ�����ʱ��Ҫ������ж�
		{
			int before = selectedEllipseIndex - 1;
			int after = 0;

			QPointF beforePt = points[before];
			QPointF afterPt = points[after];
			
			calRightAnglePt(beforePt, afterPt);

		}
		else if (selectedEllipseIndex == 0)			// ��һ����
		{
			int before = count - 1;
			int after = selectedEllipseIndex + 1;

			QPointF beforePt = points[before];
			QPointF afterPt = points[after];

			calRightAnglePt(beforePt, afterPt);
		}
		else    // ����������һ���㣬Ҫ��õ�ǰ���ǰ�������������
		{
			int before = selectedEllipseIndex - 1;
			int after = selectedEllipseIndex + 1;

			// ��ȡ����Ϣ
			QPointF beforePt = points[before];
			QPointF afterPt = points[after];

			calRightAnglePt(beforePt, afterPt);
			/*qDebug() << "beforePt: " << beforePt;
			qDebug() << "afterPt: " << afterPt;*/
			// �����������������Ϣ
			// �ж�λ��
			
			// auxFirstPt = calRightAnglePt(beforePt, afterPt, currentPt);
			/*if (beforePt.x() <= afterPt.x() && beforePt.y() <= afterPt.y())
			{
				auxPt.setX(beforePt.x());
				auxPt.setY(afterPt.y());
			}
			else if (beforePt.x() <= afterPt.x() && beforePt.y() > afterPt.y())
			{
				auxPt.setX(afterPt.x());
				auxPt.setY(beforePt.y());
			}
			else if (beforePt.x() > afterPt.x() && beforePt.y() > afterPt.y())
			{
				auxPt.setX(beforePt.x());
				auxPt.setY(afterPt.y());
			}
			else if (beforePt.x() > afterPt.x() && beforePt.y() < afterPt.y())
			{
				auxPt.setX(afterPt.x());
				auxPt.setY(beforePt.y());
			}*/
		
		}
		//tempPolygons[selectedIndex].pos.remove(selectedEllipseIndex);
		//this->update();
	}
}

/*��ѡģʽ*/
void CEditor::selectPts(const bool checked)
{
	isSelect = checked;
	qDebug() << "isSelct condition: " << isSelect;
	selectedPoints.clear();
	update();
}

/*�Ӿ����л�ȡ��*/
void CEditor::getPtsFromSelect()
{
	selectedPoints.clear();

	// ��Ҫ����һ�µ��˳��
	// ��ʱ���λ�ÿ��ܲ��Ǵ����ϵ����µ�
	QRectF selectionRect = normalizedRect();

	if (selectedIndex != -1)
	{
		QVector<QPointF>& points = tempPolygons[selectedIndex].pos;
		qDebug() << "Points  " << points.count();
		

		for (const QPointF& point : points)
		{
			if (ptInRect(point, selectStartPos, selectEndPos))
			{
				selectedPoints.append(point);
			}
		}
		qDebug() << "Point " << selectedPoints.count();
		for (int i = 0; i < selectedPoints.size(); i++)
		{
			qDebug() << "Point " << i << ": " << selectedPoints[i];
		}	
	}
}

/*�жϵ��Ƿ��ھ�����*/
bool CEditor::ptInRect(const QPointF& point, const QPointF& topLeft, const QPointF& bottomRight)
{
	return point.x() >= topLeft.x() && point.x() <= bottomRight.x()
		&& point.y() >= topLeft.y() && point.y() <= bottomRight.y();
}

/*ɾ��ѡ�еĵ�*/
void CEditor::clearSelectedPts()
{
	if (selectedIndex != -1)
	{
		QVector<QPointF>& points = tempPolygons[selectedIndex].pos;
		
		for (const QPointF& point : selectedPoints)
		{
			points.removeOne(point);
		}

		selectedPoints.clear();
		int count = points.count();
		
		if (count <= 2)
		{
			points.clear();
			selectedIndex = -1;
		}
			
		update();
		qDebug() << "Remove Finished";
	}
}
