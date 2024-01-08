#ifndef LINESERIES_H
#define LINESERIES_H
#include <QObject>
#include <QLineSeries>
#include <QWidget>
#include <Qt>

#ifdef QT5_VERSION
QT_CHARTS_USE_NAMESPACE
#endif

class LineSeries : public QLineSeries
{
    Q_OBJECT
public:
    explicit LineSeries(QWidget* parent = Q_NULLPTR);
    ~LineSeries();
Q_SIGNALS:
        void pressed(const QPointF &point,const LineSeries* name= 0);
};

#endif // LINESERIES_H
