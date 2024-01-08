#include "chartview.h"
#include <QList>
#include <QString>
#include <QLineSeries>
#include <QtGui/QMouseEvent>

ChartView::ChartView(QChart *chart, QWidget *parent) :
    QChartView(chart, parent)
{
    setDragMode(QChartView::RubberBandDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse );
    setRenderHint(QPainter::Antialiasing);
    selection.setWidth(0);
    selection.setHeight(0);
    nb_zoom=0;
}

void ChartView::wheelEvent(QWheelEvent *event)
{
    this->mouse_reel=event->position();
    //this->mouse_reel=event->posF();
    this->mouse_point=chart()->mapToValue(event->position());
    //this->mouse_point=chart()->mapToValue(event->posF());
    if(event->angleDelta().y()>0)
    {
        ymouse=1;
    }
    else
    {
        ymouse=0;
    }
    emit zoom_change();
    QChartView::wheelEvent(event);
}

void ChartView::mouseDoubleClickEvent(QMouseEvent *event)
{
    this->mouse_point=chart()->mapToValue(event->pos());
    if(event->button() == Qt::LeftButton)
    {
        chart()->zoomReset();
        emit zoom_reset();
    }
    // QChartView::mouseDoubleClickEvent(*event);
}

void ChartView::mousePressEvent(QMouseEvent *event)
{
    this->mouse_point=chart()->mapToValue(event->pos());
    if(event->button() == Qt::LeftButton)
    {
        //std::cout << "my overriden event" << std::endl;
        //return; //event doesn't go further
    }

    if(event->button() == Qt::MiddleButton)
    {
        emit souris_click_gauche(0);
        //std::cout << "my overriden event" << std::endl;
        //return; //event doesn't go further
    }
    if(event->button() == Qt::RightButton)
    {
        emit souris_click_gauche(1);
        //std::cout << "my overriden event" << std::endl;
        //return; //event doesn't go further
    }
    QChartView::mousePressEvent(event);//any other event
}

void ChartView::mouseReleaseEvent(QMouseEvent *event)
{
    this->mouse_point=chart()->mapToValue(event->pos());
    if(event->button() == Qt::RightButton)
    {
        return; //event doesn't go further
    }
    QChartView::mouseReleaseEvent(event);//any other event
}

void ChartView::mouseMoveEvent(QMouseEvent *event)
{
    this->mouse_point=chart()->mapToValue(event->pos());
    QChartView::mouseMoveEvent(event);
}

void ChartView::keyPressEvent(QKeyEvent *event)
{
    QRectF test;
    switch (event->key()) {
    case Qt::Key_Plus:
        ymouse=1;
        emit zoom_change();
        break;
    case Qt::Key_Minus:
        ymouse=0;
        emit zoom_change();
        break;
    case Qt::Key_Delete:
        emit ChartView::efface();
        break;
    case Qt::Key_Escape:
        emit ChartView::efface();
        break;
    case Qt::Key_Left:
        scroll='L';
        emit scroll_sig();
        break;
    case Qt::Key_Right:
        scroll='R';
        emit scroll_sig();
        break;
    case Qt::Key_Up:
        scroll='U';
        emit scroll_sig();
        break;
    case Qt::Key_Down:
        scroll='D';
        emit scroll_sig();
        break;
    default:
        QGraphicsView::keyPressEvent(event);
        break;
    }
}
