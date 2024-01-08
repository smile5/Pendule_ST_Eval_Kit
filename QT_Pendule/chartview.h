#ifndef CHARTVIEW_H
#define CHARTVIEW_H
#include <QObject>
#include <QString>
#include <QLineSeries>
#include <QtCharts/QChartView>

#ifdef QT5_VERSION
QT_CHARTS_USE_NAMESPACE
#endif

class ChartView : public QChartView
{
    Q_OBJECT
public:
    ChartView(QChart *chart, QWidget *parent = 0);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mouseReleaseEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    qreal xmouse;
    int ymouse,scroll;
    QRect selection;
    QPointF pos_sel1,pos_sel2,mouse_point,mouse_reel;
    QList<QRectF> old_zoom_wheel_area;
    long nb_zoom;
  //  QGraphicsSimpleTextItem txt_curs1;
  /*  QLineSeries line_curs1;
    QPointF pos_curs1;*/

signals:
    void efface();
    void souris_click_gauche(int);
    void zoom_change();
    void zoom_reset();
    void scroll_sig();
};

#endif // CHARTVIEW_H
