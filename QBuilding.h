#ifndef Q_BUILDING_H
#define Q_BUILDING_H

#include <QtWidgets/QMainWindow>
#include <QMenu>
#include <QAction>
#include <QKeyEvent>
#include <QDebug>
#include <QShortcut>

#include "ui_QBuilding.h"

class QBuilding : public QMainWindow
{
    Q_OBJECT

public:
    QBuilding(QWidget* parent = nullptr);
    ~QBuilding();

    typedef enum
    {
        KEY_STATE_Release = 0,
        KEY_STATE_Press = 1,
    } E_KEY_STATE;

    E_KEY_STATE altKey;
    E_KEY_STATE ctrlKey;
    E_KEY_STATE shiftKey;


protected:
    // ÓÒ¼ü²Ëµ¥
    // void contextMenuEvent(QContextMenuEvent* event) override;
    void keyPressEvent(QKeyEvent* e);
    void keyReleaseEvent(QKeyEvent* e);

private:
    Ui::QBuildingClass ui;

    QMenu* m_pMenu;
    // QAction* m_pActEndDraw;
    QAction* m_pActClear;

    QAction* m_pActSave2SVG;
    QAction* m_pStartSelecting;
    QAction* m_pEndSelecting;
    QAction* m_pStartDraw;
    QAction* m_pEndDraw;
    // QAction* m_pActEdit;
    // QAction* m_pActEndEdit;

    // ¿ì½Ý¼ü
    QShortcut* m_pShortcut;

    QImage m_map;   
    bool ctrlPressed = false;
};

#endif // Q_BUILDING_H