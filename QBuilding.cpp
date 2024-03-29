#include "QBuilding.h"

QBuilding::QBuilding(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    setWindowTitle(u8"Shape Regularization");

    /*QImage image = QImage(this->size(), QImage::Format_RGB32);
    image.fill(Qt::white);*/

    m_map.load(qApp->applicationDirPath() + "/map.png");
    QPainter painter(ui.drawWidget);
    painter.drawImage(QPoint(0, 0), m_map);

    // m_pMenu = new QMenu(this);

    /*m_pActEndDraw = new QAction(QString::fromLocal8Bit("结束绘制"), this);
    m_pActEndDraw->setShortcut(QKeySequence("Ctrl+E"));*/
    m_pActClear = new QAction(QString::fromLocal8Bit("清除当前所有绘制"), this);
    // m_pActClear->setShortcut(QKeySequence("Ctrl+D"));
    m_pActSave2SVG = new QAction(QString::fromLocal8Bit("存为矢量"), this);
    m_pStartSelecting = new QAction(QString::fromLocal8Bit("开始框选"), this);
    m_pEndSelecting = new QAction(QString::fromLocal8Bit("结束框选"), this);
    m_pStartDraw = new QAction(QString::fromLocal8Bit("Start"), this);
    m_pEndDraw = new QAction(QString::fromLocal8Bit("End"), this);

    // m_pActEdit = new QAction(QString::fromLocal8Bit("修改形状"), this);
    // m_pActEndEdit = new QAction(QString::fromLocal8Bit("结束修改"), this);

    ui.mainToolBar->addAction(m_pActSave2SVG);
    ui.mainToolBar->addAction(m_pActClear);
    ui.mainToolBar->addAction(m_pStartSelecting);
    ui.mainToolBar->addAction(m_pEndSelecting);
    
    /* m_pMenu->addAction(m_pActEndDraw);
    m_pMenu->addAction(m_pActClear);
    m_pMenu->addAction(m_pActDP);
    m_pMenu->addAction(m_pActSR);
    m_pMenu->addAction(m_pActSave2SVG);
    m_pMenu->addAction(m_pActEdit);
    m_pMenu->addAction(m_pActEndEdit);
    m_pMenu->addAction(m_pActQuit);

    m_pMenu->setStyleSheet("QMenu{font:18px}");*/

    // 快捷键
    m_pShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_D), this);

    connect(m_pStartDraw, &QAction::triggered, this, [=]() {
        ui.drawWidget->setEditable();
        });

    connect(m_pEndDraw, &QAction::triggered, this, [=]() {
        ui.drawWidget->setEditable();
        });

    // 删除选中的点
    connect(m_pShortcut, &QShortcut::activated, this, [=]() {
        qDebug() << "D + Ctrl";
        ui.drawWidget->clearSelectedPts();
        });

    connect(m_pActClear, &QAction::triggered, this, [=]() {
        ui.drawWidget->clearAll();
        });

    connect(m_pStartSelecting, &QAction::triggered, this, [=]() {
        ui.drawWidget->selectPts(true);
        });

    connect(m_pEndSelecting, &QAction::triggered, this, [=]() {
        ui.drawWidget->selectPts(false);
        
        });

    /*
    connect(m_pActEndDraw, &QAction::triggered, this, [=]() {
        ui.drawWidget->endDraw();
        });
    connect(m_pActDP, &QAction::triggered, this, [=]() {
        ui.drawWidget->testDP();
        });
    connect(m_pActSR, &QAction::triggered, this, [=]() {
        ui.drawWidget->testRegularization();
        });
    connect(m_pActSave2SVG, &QAction::triggered, this, [=]() {
        ui.drawWidget->saveAsSVG();
        });
    connect(m_pActEdit, &QAction::triggered, this, [=]() {
        ui.drawWidget->edit();
        });
    connect(m_pActEndEdit, &QAction::triggered, this, [=]() {
        ui.drawWidget->endEdit();
        });*/

}

QBuilding::~QBuilding()
{

}

void QBuilding::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_D)
    {
        ui.drawWidget->deleteSelectedDot();
    }

    if (e->key() == Qt::Key_Delete)
    {
        ui.drawWidget->deleteSelectedPolygon();
    }

    if (e->key() == Qt::Key_A)
    {
        /*if (e->key() == Qt::Key_Control)
            ui.drawWidget->setCursor(Qt::CrossCursor);*/
        ui.drawWidget->addPoint();
    }

    if (e->key() == Qt::Key_Control)
    {
        ui.drawWidget->setCursor(Qt::CrossCursor);
        ctrlPressed = true;
        ui.drawWidget->setCtrlPressed(true);
        if (e->key() == Qt::Key_A && ctrlPressed)
        {
            ui.drawWidget->addPoint();
        }
    }
    /*else if (e->key() == Qt::Key_A && ctrlPressed)
    {
        
        
    }*/
    
   /* if (e->key() == Qt::Key_I)
    {
        ui.drawWidget->insertDot();
    }*/
    
    switch (e->key())
    {
    case Qt::Key_Alt:
        altKey = KEY_STATE_Press;
        break;
    case Qt::Key_Control:
        ctrlKey = KEY_STATE_Press;
        break;

    case Qt::Key_Shift:
        shiftKey = KEY_STATE_Press;
        break;
    }

    if (shiftKey == KEY_STATE_Press)
        ui.drawWidget->setShift(true);
   
}

void QBuilding::keyReleaseEvent(QKeyEvent* e)
{
    switch (e->key())
    {
    case Qt::Key_Alt:
        altKey = KEY_STATE_Release;
        break;
    case Qt::Key_Control:
        ctrlKey = KEY_STATE_Release;
        break;
    case Qt::Key_Shift:
        shiftKey = KEY_STATE_Release;
        break;
    }

    if (shiftKey == KEY_STATE_Release)
        ui.drawWidget->setShift(false);    

    if (e->key() == Qt::Key_Control)
    {
        ctrlPressed = false;
        ui.drawWidget->setCtrlPressed(false);
        ui.drawWidget->unsetCursor();
    }
    
}
