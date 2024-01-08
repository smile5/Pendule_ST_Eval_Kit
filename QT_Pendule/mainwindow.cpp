#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "chartview.h"
#include "lineseries.h"
#include <QtDebug>

#include <QtSerialPort>
#include <QSerialPortInfo>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QCategoryAxis>
#include <QFileDialog>
#include <QMessageBox>

#ifdef QT5_VERSION
    QT_CHARTS_USE_NAMESPACE
#endif

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    dev_label(new ClickableLabel),
    serial_port_label(new QLabel),
    etat_label(new QLabel),
    deltax_label(new QLabel),
    serial(new QSerialPort),
    ui(new Ui::MainWindow),
  axe_temps(new QValueAxis)
{
    timer = new QTimer();
    tech=2;
    nbr=0;
    marche_bool=0;
    chart_update = new QTimer();
    connect(timer, SIGNAL(timeout()), this,SLOT(serial_timeout()));
    timer->stop();
    connect(chart_update,SIGNAL(timeout()),this,SLOT(append_point()));
    chart_update->stop();

    rcv_serie.nb_char=0;
    rcv_serie.rcv_ok=false;
    curseur_bt_d=false;
    curseur_bt_g=false;
    ui->setupUi(this);

    dev_label->setAlignment(Qt::AlignLeft);
    dev_label->setFixedWidth(150);
    connect(dev_label,SIGNAL(clicked()),this,SLOT(on_dev_label_clicked()));

    serial_port_label->setAlignment(Qt::AlignCenter);
    serial_port_label->setIndent(25);

    etat_label->setAlignment(Qt::AlignCenter);
    etat_label->setIndent(25);

    deltax_label->setAlignment(Qt::AlignRight);
    deltax_label->setIndent(25);
    deltax_label->setFixedWidth(150);

    ui->statusBar->addWidget(dev_label);
    ui->statusBar->addWidget(serial_port_label, 1);
    ui->statusBar->addWidget(etat_label,2);
    ui->statusBar->addWidget(deltax_label,3);
    etat_label->setText("Système Arrêté");
    dev_label->setText("Réalisé par Enise - XG");
    serial_port_label->setText("Port série: "+serial->portName());
    deltax_label->setText("Delta X:");

    ui->cu_lb_y1->show();
    ui->cu_lb_y2->hide();
    ui->cu_lb_y3->hide();
    ui->cu_lb_y4->hide();
    ui->cu_lb_y5->hide();
    ui->cu_lb_y6->hide();
    ui->cu1_lb_y1->show();
    ui->cu1_lb_y2->hide();
    ui->cu1_lb_y3->hide();
    ui->cu1_lb_y4->hide();
    ui->cu1_lb_y5->hide();
    ui->cu1_lb_y6->hide();
    // initialisation composant pour le graphique et le graphique
    serie_consigne = new LineSeries();
    serie_erreur_psy = new LineSeries();
    serie_erreur_teta = new LineSeries();
    serie_spid_psy = new LineSeries();
    serie_spid_teta = new LineSeries();
    serie_psy = new LineSeries();
    serie_teta = new LineSeries();
    serie_consigne->setPen(QPen(COLOR_CONSIGNE,2)); //,2, Qt::DotLine,Qt::SquareCap,Qt::BevelJoin));
    serie_erreur_psy->setPen(QPen(COLOR_ERREUR_PSY,2));
    serie_erreur_teta->setPen(QPen(COLOR_ERREUR_TETA,2));
    serie_spid_psy->setPen(QPen(COLOR_SPID_PSY,2));
    serie_spid_teta->setPen(QPen(COLOR_SPID_TETA,2));
    serie_psy->setPen(QPen(COLOR_PSY,2));
    serie_teta->setPen(QPen(COLOR_TETA,2));
    serie_consigne->setUseOpenGL(true);
    serie_erreur_psy->setUseOpenGL(true);
    serie_erreur_teta->setUseOpenGL(true);
    serie_spid_psy->setUseOpenGL(true);
    serie_spid_teta->setUseOpenGL(true);
    serie_psy->setUseOpenGL(true);
    serie_teta->setUseOpenGL(true);

  /*  serie_consigne->connect(serie_consigne, SIGNAL(pointAdded(int)),this,SLOT(pt_consigne_added(int)));
    serie_erreur->connect(serie_erreur, SIGNAL(pointAdded(int)),this,SLOT(pt_erreur_added(int)));
    serie_spid_psy->connect(serie_spid_psy, SIGNAL(pointAdded(int)),this,SLOT(pt_spid_psy_added(int)));
    serie_spid_teta->connect(serie_spid_teta, SIGNAL(pointAdded(int)),this,SLOT(pt_spid_teta_added(int)));
    serie_psy->connect(serie_psy, SIGNAL(pointAdded(int)),this,SLOT(pt_psy_added(int)));
    serie_psy->connect(serie_psy, SIGNAL(pointAdded(int)),this,SLOT(pt_added(int)));*/


    axis_consigne = new QValueAxis;
    axis_consigne->setLinePenColor(COLOR_CONSIGNE);
    axis_consigne->setProperty("alignment",Qt::AlignLeft);

    axis_erreur_psy = new QValueAxis;
    axis_erreur_psy->setLinePenColor(COLOR_ERREUR_PSY);
    axis_erreur_psy->setProperty("alignment",Qt::AlignLeft);

    axis_erreur_teta = new QValueAxis;
    axis_erreur_teta->setLinePenColor(COLOR_ERREUR_TETA);
    axis_erreur_teta->setProperty("alignment",Qt::AlignLeft);

    axis_spid_psy = new QValueAxis;
    axis_spid_psy->setLinePenColor(COLOR_SPID_PSY);
    axis_spid_psy->setProperty("alignment",Qt::AlignLeft);

    axis_spid_teta = new QValueAxis;
    axis_spid_teta->setLinePenColor(COLOR_SPID_TETA);
    axis_spid_teta->setProperty("alignment",Qt::AlignLeft);

    axis_psy = new QValueAxis;
    axis_psy->setLinePenColor(COLOR_PSY);
    axis_psy->setProperty("alignment",Qt::AlignLeft);

    axis_teta = new QValueAxis;
    axis_teta->setLinePenColor(COLOR_TETA);
    axis_teta->setProperty("alignment",Qt::AlignLeft);
   // axe_temps = new QValueAxis;
    axe_temps->setRange(0,100);
    axe_temps->setLabelFormat("%.3G");
    //
    axe_temps->setProperty("alignment",Qt::AlignBottom);

    chart = new QChart();
    chart->legend()->hide();
    chart->createDefaultAxes();

    chart->addSeries(serie_consigne);
    chart->addSeries(serie_erreur_psy);
    chart->addSeries(serie_erreur_teta);
    chart->addSeries(serie_spid_psy);
    chart->addSeries(serie_spid_teta);
    chart->addSeries(serie_psy);
    chart->addSeries(serie_teta);

    chart->addAxis(axis_consigne, Qt::AlignLeft);
    chart->addAxis(axis_erreur_psy, Qt::AlignLeft);
    chart->addAxis(axis_erreur_teta, Qt::AlignLeft);
    chart->addAxis(axis_spid_psy, Qt::AlignLeft);
    chart->addAxis(axis_spid_teta, Qt::AlignLeft);
    chart->addAxis(axis_psy, Qt::AlignLeft);
    chart->addAxis(axis_teta, Qt::AlignLeft);
    chart->addAxis(axe_temps, Qt::AlignBottom);
    axe_temps->show();
    serie_consigne->attachAxis(axis_consigne);
    serie_consigne->attachAxis(axe_temps);
    serie_erreur_psy->attachAxis(axis_erreur_psy);
    serie_erreur_psy->attachAxis(axe_temps);
    serie_erreur_teta->attachAxis(axis_erreur_teta);
    serie_erreur_teta->attachAxis(axe_temps);
    serie_spid_psy->attachAxis(axis_spid_psy);
    serie_spid_psy->attachAxis(axe_temps);
    serie_spid_teta->attachAxis(axis_spid_teta);
    serie_spid_teta->attachAxis(axe_temps);
    serie_psy->attachAxis(axis_psy);
    serie_psy->attachAxis(axe_temps);
    serie_teta->attachAxis(axis_teta);
    serie_teta->attachAxis(axe_temps);

    serie_consigne->hide();
    axis_consigne->hide();
    serie_erreur_psy->hide();
    axis_erreur_psy->hide();
    serie_erreur_teta->hide();
    axis_erreur_teta->hide();
    serie_spid_psy->hide();
    axis_spid_psy->hide();
    serie_spid_teta->hide();
    axis_spid_teta->hide();
    serie_psy->hide();
    axis_psy->hide();
    serie_teta->show();
    axis_teta->show();
  //  chart->addAxis(axe_temps,Qt::AlignBottom);
   /* chart->setAxisX(axe_temps,serie_consigne);
    chart->setAxisX(axe_temps,serie_erreur);
    chart->setAxisX(axe_temps,serie_spid_psy);
    chart->setAxisX(axe_temps,serie_spid_teta);
    chart->setAxisX(axe_temps,serie_psy);
    chart->setAxisX(axe_temps,serie_teta);
*/
    axis_consigne->setMinorTickCount(2);
    axis_erreur_psy->setMinorTickCount(2);
    axis_erreur_teta->setMinorTickCount(2);
    axis_spid_psy->setMinorTickCount(2);
    axis_spid_teta->setMinorTickCount(2);
    axis_psy->setMinorTickCount(2);
    axis_teta->setMinorTickCount(2);

    axe_temps->setTickCount(10);
    axe_temps->setMinorTickCount(2);

    chart->setObjectName("Chart_xg");
    chart->setMargins(QMargins(0,0,0,0));
    chartview = new ChartView(chart,this);

    chartview->connect(chartview,SIGNAL(zoom_change()),this,SLOT(replot_zoom()));
    chartview->connect(chartview,SIGNAL(scroll_sig()),this,SLOT(replot_scroll()));
    chartview->connect(chartview,SIGNAL(rubberBandChanged(QRect,QPointF,QPointF)),this,SLOT(replot_rubber(QRect,QPointF,QPointF)));
    chartview->connect(chart,SIGNAL(plotAreaChanged(QRectF)),this,SLOT(replot_area(QRectF)));
    chartview->connect(chartview,SIGNAL(zoom_reset()),this,SLOT(replot_zoomreset()));
    chartview->connect(chartview,SIGNAL(efface()),this,SLOT(clear_series()));
    chartview->connect(chartview,SIGNAL(souris_click_gauche(int)),this,SLOT(cliick(int)));

    ui->Chart_layout->addWidget(chartview);

    curseur_g = new QLineSeries();
    curseur_g->setPen(QPen(QColor(173, 127, 168), 1, Qt::DashLine));
    curseur_g->setUseOpenGL(true);
    chart->addSeries(curseur_g);
    curseur_g->attachAxis(axis_consigne);
    curseur_g->setObjectName("curseur_g");
    curseur_g->attachAxis(axe_temps);
    //chart->setAxisX(axe_temps,curseur_g);

    //click courbe peu precis par rapport curseur j'enleve
    /*   serie_consigne->connect(serie_consigne,SIGNAL(pressed(QPointF)),this,SLOT(test(QPointF)));
    serie_erreur->connect(serie_erreur,SIGNAL(pressed(QPointF)),this,SLOT(test(QPointF)));
    serie_spid_psy->connect(serie_spid_psy,SIGNAL(pressed(QPointF)),this,SLOT(test(QPointF)));
    serie_spid_teta->connect(serie_spid_teta,SIGNAL(pressed(QPointF)),this,SLOT(test(QPointF)));
    serie_psy->connect(serie_psy,SIGNAL(pressed(QPointF)),this,SLOT(test(QPointF)));
    serie_teta->connect(serie_teta,SIGNAL(pressed(QPointF)),this,SLOT(test(QPointF)));
*/
    curseur_d = new QLineSeries();
    curseur_d->setPen(QPen(QColor(0, 0, 0), 1, Qt::DashLine));
    curseur_d->setUseOpenGL(true);
    chart->addSeries(curseur_d);
    curseur_d->attachAxis(axis_consigne);
    curseur_d->attachAxis(axe_temps);
    //chart->setAxisX(axe_temps,curseur_d);

    on_combobox_mode_fct_currentIndexChanged(3);
    ui->aff_cb_consigne->setChecked(false);
    ui->aff_cb_consigne->hide();
    ui->aff_cb_teta->setChecked(true);
    ui->aff_cb_teta->show();
    clear_series();
    on_combobox_mode_fct_currentIndexChanged(3);
}

void MainWindow::test(QPointF pt)
{
    pt_clicked=pt;
    QMessageBox::about(this,"Point Clické:",
                       "X: " + QString().number(pt.x()) + " <br>"
                       "Y: " + QString().number(pt.y()) + " <br>");
}

QPointF MainWindow::extrapol(qreal x, QLineSeries *serie)
{
    qreal tmpx,tmpx1,tmpy,tmpy1,y;
    int i=0;

    for(tmpx=0;(tmpx<x)&&(i<serie->count());)
    {
        tmpx=serie->at(i).x();
        i++;
    }
    tmpx=serie->at(i-2).x();
    tmpy=serie->at(i-2).y();
    tmpx1=serie->at(i-1).x();
    tmpy1=serie->at(i-1).y();
    y=((tmpy1-tmpy)/(tmpx1-tmpx))*(x-tmpx)+tmpy;
    return QPointF(x,y);
}
void MainWindow::clear_series()
{
    ymax_consigne=-180;
    ymin_consigne=180;
    compteur=0;
    nbr=0;
    serie_consigne->clear();
    serie_erreur_psy->clear();
    serie_erreur_teta->clear();
    serie_spid_psy->clear();
    serie_spid_teta->clear();
    serie_psy->clear();
    serie_teta->clear();
    axe_temps->setRange(0,3);
    if(ui->combobox_mode_fct->currentIndex()==0)
        axis_consigne->setRange(150,210);
    else
        axis_consigne->setRange(-270,270);
    axis_erreur_psy->setRange(-1000,1000);
    axis_erreur_teta->setRange(-1000,1000);
    axis_spid_psy->setRange(-360,360);
    axis_spid_teta->setRange(-1000,1000);
    axis_psy->setRange(-270,270);
    if(ui->combobox_mode_fct->currentIndex()==3)
        axis_teta->setRange(-270,270);
    else
        axis_teta->setRange(160,200);
    curseur_d->clear();
    curseur_g->clear();
    txt_curs1.hide();
    txt_curs2.hide();
}

void MainWindow::replot_zoom()
{
    QPointF zm,z;
    QRectF zoom_area,plot_area;

    chart_update->stop();
    plot_area=chart->plotArea();
    z=chartview->mouse_point;
    zm=chartview->mouse_reel;
    if(chartview->ymouse==1)  //Zomm IN
    {
        zoom_area.setX(zm.x()-plot_area.width()/4);
        zoom_area.setY(zm.y()-plot_area.height()/4);
        zoom_area.setWidth(plot_area.width()/2);
        zoom_area.setHeight(plot_area.height()/2);
        if(zoom_area.x()<0)
        {
            zoom_area.setX(chart->mapToPosition(QPointF(0,0)).x());
        }
        if(z.x()<(3/1000))
        {
            zoom_area.setX(chart->mapToPosition(QPointF(0,0)).x());
        }
        if((chart->mapToValue(zoom_area.topRight()).x()-chart->mapToValue(zoom_area.topLeft()).x())>(3/1000))
        {
            chart->zoomIn(zoom_area);
            chartview->nb_zoom++;
            //chart->zoomIn();
        }
    }
    else if(chartview->ymouse==0) // Zoom OUT
    {
        zoom_area.setX(zm.x()-1*plot_area.width());
        zoom_area.setY(zm.y()-1*plot_area.height());
        zoom_area.setWidth(plot_area.width()*2);
        zoom_area.setHeight(plot_area.height()*2);

        if((chart->mapToValue(zoom_area.topRight()).x()-chart->mapToValue(zoom_area.topLeft()).x())>(3/1000))
        {
            if(chartview->nb_zoom>-2)
            {
                chart->zoomIn(zoom_area);
                chartview->nb_zoom--;
            }
            else
            {
                chart->zoomReset();
                chart_update->start();
            }
        }
    }
    else
    {}
    if(axe_temps->min()<0)
    {
        zoom_area.setX(chart->mapToPosition(QPointF(0,0)).x());
        chart->zoomIn(zoom_area);
    }
    replot_cursor();
}

void MainWindow::replot_scroll()
{
    if(chartview->scroll=='U')
    {
        chart->scroll(0,(ymax_consigne-ymin_consigne)/100);
    }
    if(chartview->scroll=='D')
    {
        chart->scroll(0,-(ymax_consigne-ymin_consigne)/100);
    }
    if(chartview->scroll=='L')
    {
        qreal axe_max;
        axe_max=axe_temps->max();
        chart->scroll(-(axe_temps->max()-axe_temps->min())*200,0);
        if(axe_temps->min()<0)
        {
            axe_temps->setMin(0);
            axe_temps->setMax(axe_max);
        }
    }
    if(chartview->scroll=='R')
    {
        chart->scroll((axe_temps->max()-axe_temps->min())*200,0);
    }
    replot_cursor();
}

void MainWindow::replot_cursor()
{
    if ((curseur_g->count()<1)&&(curseur_d->count()<1))
    {}
    else
    {
        pos_curs1=chart->mapToPosition(QPointF(curseur_g->at(1).x(),axis_consigne->max()),curseur_g);
        txt_curs1.setPos(pos_curs1);

        curseur_g->replace(curseur_g->at(1).x(),curseur_g->at(1).y(),curseur_g->at(1).x(),axis_consigne->min());
        curseur_g->replace(curseur_g->at(0).x(),curseur_g->at(0).y(),curseur_g->at(0).x(),axis_consigne->max());

        pos_curs2=chart->mapToPosition(QPointF(curseur_d->at(1).x(),axis_consigne->max()),curseur_d);
        txt_curs2.setPos(pos_curs2);

        curseur_d->replace(curseur_d->at(1).x(),curseur_d->at(1).y(),curseur_d->at(1).x(),axis_consigne->min());
        curseur_d->replace(curseur_d->at(0).x(),curseur_d->at(0).y(),curseur_d->at(0).x(),axis_consigne->max());
    }
}

void MainWindow::replot_zoomreset()
{
    chart_update->start();
    replot_cursor();
    chartview->nb_zoom=0;
}

void MainWindow::replot_area(QRectF rect)
{
    replot_cursor();
    rect.isValid();
}

void MainWindow::replot_rubber(QRect rect, QPointF pos1, QPointF pos2)
{
    QPointF posc1,posc2;
    if(rect.isNull())
    {
        posc1=chart->mapToValue(chartview->pos_sel1);
        posc2=chart->mapToValue(chartview->pos_sel2);
        if((abs(posc1.x()-posc2.x())>0.003) && (posc1.x()>0) && (posc2.x()>0))
        {
            chart->zoomIn(chartview->selection);
        }
        replot_cursor();
    }
    chartview->selection=rect;
    chartview->pos_sel1=pos1;
    chartview->pos_sel2=pos2;
    chart_update->stop();
}

void MainWindow::cliick(int clk)
{
    qreal x;
    QPointF pt;
    QString a;
    x=chartview->mouse_point.x();

    if(clk==0)
    {
        if(curseur_bt_g==0)
        {
            ui->cu_lb_x->setText("X1: "+a.number(x,'G'));
            if(serie_consigne->count()==0)
            {}
            else
            {
                curseur_bt_g=1;
                curseur_g->append(x,ymin_consigne);
                curseur_g->append(x,ymax_consigne);
                pos_curs1=chart->mapToPosition(QPointF(curseur_g->at(1).x(),axis_consigne->max()),curseur_g);
                txt_curs1.setPos(pos_curs1);
                txt_curs1.setPen(QPen(QColor(173, 127, 168)));
                txt_curs1.setText("1");
                txt_curs1.show();
                chartview->scene()->addItem(&txt_curs1);
            }
            if(serie_consigne->count()>2)
            {
                pt=this->extrapol(x,serie_consigne);
                ui->cu_lb_y1->setText(a.number(pt.y(),'G',4));
                if(ui->aff_cb_consigne->isChecked())
                    ui->cu_lb_y1->show();
            }
            else
            {
                ui->cu_lb_y1->hide();
            }
            if((serie_erreur_psy->count()>2)&&(ui->aff_cb_erreur_psy->isChecked()))
            {
                pt=this->extrapol(x,serie_erreur_psy);
                ui->cu_lb_y2->setText(a.number(pt.y(),'G',4));
                ui->cu_lb_y2->show();
            }
            else
            {
                ui->cu_lb_y2->hide();
            }
            if((serie_spid_teta->count()>2)&&(ui->aff_cb_pid_teta->isChecked()))
            {
                pt=this->extrapol(x,serie_spid_teta);
                ui->cu_lb_y3->setText(a.number(pt.y(),'G',4));
                ui->cu_lb_y3->show();
            }
            else
            {
                ui->cu_lb_y3->hide();
            }
            if((serie_spid_psy->count()>2)&&(ui->aff_cb_pid_psy->isChecked()))
            {
                pt=this->extrapol(x,serie_spid_psy);
                ui->cu_lb_y4->setText(a.number(pt.y(),'G',4));
                ui->cu_lb_y4->show();
            }
            else
            {
                ui->cu_lb_y4->hide();
            }
            if((serie_psy->count()>2)&&(ui->aff_cb_psy->isChecked()))
            {
                pt=this->extrapol(x,serie_psy);
                ui->cu_lb_y6->setText(a.number(pt.y(),'G',4));
                ui->cu_lb_y6->show();
            }
            else
            {
                ui->cu_lb_y6->hide();
            }
            if((serie_teta->count()>2)&&(ui->aff_cb_teta->isChecked()))
            {
                pt=this->extrapol(x,serie_teta);
                ui->cu_lb_y5->setText(a.number(pt.y(),'G',4));
                ui->cu_lb_y5->show();
            }
            else
            {
                ui->cu_lb_y5->hide();
            }
        }
        else
        {
            chartview->scene()->removeItem(&txt_curs1);
            ui->cu_lb_x->setText("X1: ");
            curseur_bt_g=0;
            curseur_g->clear();
            ui->cu_lb_y1->hide();
            ui->cu_lb_y2->hide();
            ui->cu_lb_y3->hide();
            ui->cu_lb_y4->hide();
            ui->cu_lb_y5->hide();
            ui->cu_lb_y6->hide();
        }
    }
    else if (clk==1)
    {

        if(curseur_bt_d==0)
        {
            ui->cu1_lb_x->setText("X2 : "+a.number(x,'G'));
            if(serie_consigne->count()==0)
            {}
            else
            {
                curseur_bt_d=1;
                curseur_d->append(x,ymin_consigne);
                curseur_d->append(x,ymax_consigne);
                pos_curs2=chart->mapToPosition(curseur_d->at(1),curseur_d);
                txt_curs2.setPos(pos_curs2);
                txt_curs2.setPen(QPen(QColor(173, 127, 168)));
                txt_curs2.setText("2");
                txt_curs2.show();
                chartview->scene()->addItem(&txt_curs2);
            }
            if(serie_consigne->count()>2)
            {
                pt=this->extrapol(x,serie_consigne);
                ui->cu1_lb_y1->setText(a.number(pt.y(),'G',4));
                if(ui->aff_cb_consigne->isChecked())
                    ui->cu1_lb_y1->show();
            }
            else
            {
                ui->cu1_lb_y1->hide();
            }
            if((serie_erreur_psy->count()>2)&&(ui->aff_cb_erreur_psy->isChecked()))
            {
                pt=this->extrapol(x,serie_erreur_psy);
                ui->cu1_lb_y2->setText(a.number(pt.y(),'G',4));
                ui->cu1_lb_y2->show();
            }
            else
            {
                ui->cu1_lb_y2->hide();
            }
            if((serie_spid_teta->count()>2)&&(ui->aff_cb_pid_teta->isChecked()))
            {
                pt=this->extrapol(x,serie_spid_teta);
                ui->cu1_lb_y3->setText(a.number(pt.y(),'G',4));
                ui->cu1_lb_y3->show();
            }
            else
            {
                ui->cu1_lb_y3->hide();
            }
            if((serie_spid_psy->count()>2)&&(ui->aff_cb_pid_psy->isChecked()))
            {
                pt=this->extrapol(x,serie_spid_psy);
                ui->cu1_lb_y4->setText(a.number(pt.y(),'G',4));
                ui->cu1_lb_y4->show();
            }
            else
            {
                ui->cu1_lb_y4->hide();
            }
            if((serie_psy->count()>2)&&(ui->aff_cb_psy->isChecked()))
            {
                pt=this->extrapol(x,serie_psy);
                ui->cu1_lb_y6->setText(a.number(pt.y(),'G',4));
                ui->cu1_lb_y6->show();
            }
            else
            {
                ui->cu1_lb_y6->hide();
            }
            if((serie_teta->count()>2)&&(ui->aff_cb_teta->isChecked()))
            {
                pt=this->extrapol(x,serie_teta);
                ui->cu1_lb_y5->setText(a.number(pt.y(),'G',4));
                ui->cu1_lb_y5->show();
            }
            else
            {
                ui->cu1_lb_y5->hide();
            }
        }
        else
        {
            chartview->scene()->removeItem(&txt_curs2);
            ui->cu1_lb_x->setText("X2: ");
            curseur_bt_d=0;
            curseur_d->clear();
            ui->cu1_lb_y1->hide();
            ui->cu1_lb_y2->hide();
            ui->cu1_lb_y3->hide();
            ui->cu1_lb_y4->hide();
            ui->cu1_lb_y5->hide();
            ui->cu1_lb_y6->hide();
        }
    }
    replot_cursor();
    if((curseur_d->count()>1)&&(curseur_g->count()>1))
    {
        deltax_label->setText("Delta X:"+QString().number(abs(curseur_d->at(0).x()-curseur_g->at(0).x()),'G'));
    }
    else
    {
        deltax_label->setText("Delta X:");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    serial->close();
}

void MainWindow::serial_timeout()
{
    rcv_serie.rcv_ok=false;
    timer->stop();
}

void MainWindow::serialreceived()
{
    long i;
    timer->start(2000);
    serbuf=serial->readAll();

    for(i=0;i<serbuf.size();i++)
    {
        if ((serbuf.at(i)=='=')&&(rcv_serie.nb_char==0))
        {
            rcv_serie.nb_char=1;
            rcv_serie.buffer.clear();
        }
        else if((rcv_serie.nb_char>=1)&&(rcv_serie.nb_char<20))
        {
            rcv_serie.nb_char++;
            rcv_serie.buffer.append((unsigned char)serbuf.at(i));

            if(rcv_serie.nb_char==5)
            {
                //rcv_serie.mes_teta=rcv_serie.buffer.toFloat();
                rcv_serie.mes_teta=*(reinterpret_cast<const float*>(rcv_serie.buffer.constData()));
                rcv_serie.mes_teta=rcv_serie.mes_teta*360.0/24000.0;
                rcv_serie.buffer.clear();
            }
            else if(rcv_serie.nb_char==8)
            {
                if((unsigned char)serbuf.at(i)>0x80)
                {
                    rcv_serie.mes_psy=(float)(long)(((unsigned char)rcv_serie.buffer[0])+((unsigned char)rcv_serie.buffer[1]<<8)+((unsigned char)serbuf.at(i)<<16)+(0xFF<<24));
                }
                else
                {
                    rcv_serie.mes_psy=(float)(long)(((unsigned char)rcv_serie.buffer[0])+((unsigned char)rcv_serie.buffer[1]<<8)+((unsigned char)serbuf.at(i)<<16)+(0x00<<24));
                }
                rcv_serie.mes_psy=360.0*rcv_serie.mes_psy/(200.0*16.0);
                rcv_serie.buffer.clear();
            }
            else if(rcv_serie.nb_char==10)
            {
                short tmp;
                tmp=(short)((unsigned char)rcv_serie.buffer[0]+((unsigned char)rcv_serie.buffer[1]<<8));
                rcv_serie.consigne=((float)tmp)*360.0/(200.0*16.0);
                rcv_serie.buffer.clear();
            }
            else if(rcv_serie.nb_char==14)
            {
                rcv_serie.scorr_teta=*(reinterpret_cast<const float*>(rcv_serie.buffer.constData()));
                rcv_serie.scorr_teta=rcv_serie.scorr_teta;
                rcv_serie.buffer.clear();
            }
            else if(rcv_serie.nb_char==18)
            {
                rcv_serie.scorr_psy=*(reinterpret_cast<const float*>(rcv_serie.buffer.constData()));
                rcv_serie.scorr_psy=rcv_serie.scorr_psy;
                rcv_serie.buffer.clear();
            }
            else if(rcv_serie.nb_char==19)
            {
                timer->stop();
                rcv_serie.nb_char=0;
                rcv_serie.rcv_ok=true;
            }
        }
        else
        {
            rcv_serie.nb_char=0;
            rcv_serie.buffer.clear();
            rcv_serie.rcv_ok=false;
        }
        if(rcv_serie.rcv_ok==true)
        {
            rcv_serie.rcv_ok=false;
            tab_data.append(rcv_serie);
        }
    }
}

// modif d'une valeur => envoie trame complète :
void MainWindow::calc_num_den_float(double value,unsigned short &value_num,unsigned short &value_den)
{
    if (value >=1000)
    {
        value_num=(unsigned short)(value);
        value_den=1;
    }
    else if (value >= 100)
    {
        value_num=(unsigned short)(value*10.0);
        value_den=10;
    }
    else if (value >=10)
    {
        value_num=(unsigned short)(value*100.0);
        value_den=100;
    }
    else if (value >=1)
    {
        value_num=(unsigned short)(value*1000.0);
        value_den=1000;
    }
    else
    {
        value_num=(unsigned short)(value*10000.0);
        value_den=10000;
    }
}

// modif d'une valeur => envoie trame complète :
void MainWindow::calc_num_den_float_per(double value,unsigned short &value_num,unsigned short &value_den)
{
    if (value >=1000)
    {
        value_num=(unsigned short)(value);
        value_den=1;
    }
    else if (value >= 100)
    {
        value_num=(unsigned short)(value*10.0);
        value_den=10;
    }
    else if (value >=10)
    {
        value_num=(unsigned short)(value*100.0);
        value_den=100;
    }
    else
    {
        value_num=(unsigned short)(value*1000.0);
        value_den=1000;
    }
}

void MainWindow::on_combobox_mode_fct_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
        ui->groupBox_consigne->hide();
        ui->groupbox_pid1->setTitle("PID Vertical θ");
        ui->label_6->setText("Kp coeff. Proportionnel");
        ui->label_4->show();
        ui->pid1_sb_kp->show();
        ui->aff_cb_erreur_psy->show();
        ui->label_4->setText("Kp coeff. Proportionnel");
        ui->aff_cb_pid_teta->setText("Sortie PID Vertical θ");
        ui->label_5->setText("Ti: coeff. Intégral (s)");
        ui->label_3->setText("Td: coeff. Dérivé (s)");
        ui->label_7->setText("Ti: coeff. Intégral (s)");
        ui->label_8->setText("Kd: coeff. Dérivé (s)");
        ui->aff_cb_erreur_psy->setText("Vitesse (tr/min)");
        ui->aff_cb_pid_teta->setText("Sortie PID Vertical θ (tr/min/s)");
        ui->groupbox_pid1->show();
        ui->groupbox_pid2->hide();
        ui->aff_cb_pid_teta->show();
        ui->aff_cb_pid_psy->setChecked(false);
        ui->aff_cb_pid_psy->hide();
        ui->aff_cb_consigne->show();
        ui->aff_cb_consigne->setChecked(true);
        axis_consigne->setRange(160,200);
        break;
    case 1:
        ui->groupBox_consigne->show();
        ui->label_11->setText("Amplitude (°)");
        //ui->cons_sb_vinit->setRange(-180,180);
        ui->label_4->show();
        ui->pid1_sb_kp->show();
        ui->label_4->setText("Kp coeff. Proportionnel");
        ui->label_6->setText("Kp coeff. Proportionnel");
        ui->groupbox_pid2->setTitle("PID Horizontal ψ");
        ui->groupbox_pid1->setTitle("PID Vertical θ");
        ui->aff_cb_consigne->setText("Consigne (°)");
        ui->aff_cb_erreur_psy->show();
        ui->aff_cb_pid_teta->setText("Sortie PID Vertical θ");
        ui->aff_cb_pid_psy->setText("Sortie PID Horizontal ψ");
        ui->label_5->setText("Ti: coeff. Intégral (s)");
        ui->label_3->setText("Td: coeff. Dérivé (s)");
        ui->label_7->setText("Ti: coeff. Intégral (s)");
        ui->label_8->setText("Td: coeff. Dérivé (s)");
        ui->aff_cb_erreur_psy->setText("Erreur Angle Hor. ψ (°)");
        ui->groupbox_pid1->show();
        ui->groupbox_pid2->show();
        ui->aff_cb_pid_teta->show();
        ui->aff_cb_pid_psy->show();
        ui->aff_cb_consigne->show();
        ui->aff_cb_consigne->setChecked(true);
        break;
    case 2:
        ui->groupBox_consigne->show();
        ui->label_11->setText("Amplitude(°)");
        ui->label_4->show();
        ui->pid1_sb_kp->show();
        ui->label_4->setText("Kp coeff. Proportionnel");
        ui->label_6->setText("Kp coeff. Proportionnel");
        ui->groupbox_pid2->setTitle("PID Horizontal ψ");
        ui->groupbox_pid1->setTitle("PID Vertical θ");
        ui->aff_cb_consigne->setText("Consigne (°)");
        ui->aff_cb_erreur_psy->show();
        ui->aff_cb_pid_teta->setText("Sortie PID Vertical θ");
        ui->aff_cb_pid_psy->setText("Sortie RST Horizontal ψ");
        ui->label_5->setText("Ti: coeff. Intégral (s)");
        ui->label_3->setText("Td: coeff. Dérivé (s)");
        ui->label_7->setText("Ti: coeff. Intégral (s)");
        ui->label_8->setText("Td: coeff. Dérivé (s)");
         ui->aff_cb_erreur_psy->setText("Erreur Angle Hor. ψ (°)");
        ui->groupbox_pid1->show();
        ui->groupbox_pid2->show();
        ui->aff_cb_pid_teta->show();
        ui->aff_cb_pid_psy->show();
        ui->aff_cb_consigne->show();
        ui->aff_cb_consigne->setChecked(true);
        break;
    default:
        ui->groupBox_consigne->show();
        ui->label_11->setText("PWM Initiale(%)");
        ui->groupbox_pid1->setTitle("Coefficients intégral et dérivé");
        ui->aff_cb_consigne->setText("Consigne(°)");
        ui->aff_cb_pid_teta->setText("Sortie intégration");
        ui->aff_cb_pid_psy->setText("Sortie dérivation");
        ui->pid1_sb_kp->hide();
        ui->label_4->hide();
        ui->label_3->setText("Coeff. Dérivé");
        ui->pid1_sb_ki->setValue(1);
        ui->label_5->setText("Coeff. Intégral");
        ui->pid1_sb_kd->setValue(1);
        //ui->aff_cb_erreur->hide();
        ui->aff_cb_erreur_psy->setText("Valeur Registre PWM");
        ui->groupbox_pid1->hide();
        ui->groupbox_pid2->hide();
        ui->aff_cb_pid_teta->show();
        ui->aff_cb_pid_psy->show();
        ui->aff_cb_erreur_psy->hide();
        ui->aff_cb_pid_teta->hide();
        ui->aff_cb_pid_psy->show();
        ui->aff_cb_consigne->hide();
        ui->aff_cb_consigne->setChecked(false);
        break;
    }
    clear_series();
    //    this->envoie_trame_config();
}

// Gestion Zoom et afficahge

void MainWindow::on_aff_cb_consigne_stateChanged(int arg1)
{
    QPointF pt;
    QString a;
    if(arg1)
    {
        serie_consigne->show();
        axis_consigne->show();
        if(curseur_g->count()>1)
            ui->cu_lb_y1->show();
        if(curseur_d->count()>1)
            ui->cu1_lb_y1->show();
        if(serie_consigne->count()>2)
        {
            pt=this->extrapol(curseur_g->at(1).x(),serie_consigne);
            ui->cu_lb_y1->setText(a.number(pt.y(),'G',4));
            pt=this->extrapol(curseur_d->at(1).x(),serie_consigne);
            ui->cu1_lb_y1->setText(a.number(pt.y(),'G',4));
        }
    }
    else
    {
        serie_consigne->hide();
        axis_consigne->hide();
        ui->cu_lb_y1->hide();
        ui->cu1_lb_y1->hide();
    }
}

void MainWindow::on_aff_cb_erreur_psy_stateChanged(int arg1)
{
    QPointF pt;
    QString a;
    if(arg1)
    {
        serie_erreur_psy->show();
        axis_erreur_psy->show();
        if(curseur_g->count()>1)
            ui->cu_lb_y2->show();
        if(curseur_d->count()>1)
            ui->cu1_lb_y2->show();
        if(serie_erreur_psy->count()>2)
        {
            pt=this->extrapol(curseur_g->at(1).x(),serie_erreur_psy);
            ui->cu_lb_y2->setText(a.number(pt.y(),'G',4));
            pt=this->extrapol(curseur_d->at(1).x(),serie_erreur_psy);
            ui->cu1_lb_y2->setText(a.number(pt.y(),'G',4));
        }
    }
    else
    {
        serie_erreur_psy->hide();
        axis_erreur_psy->hide();
        ui->cu_lb_y2->hide();
        ui->cu1_lb_y2->hide();
    }
}

void MainWindow::on_aff_cb_psy_stateChanged(int arg1)
{
    QPointF pt;
    QString a;
    if(arg1)
    {
        serie_psy->show();
        axis_psy->show();
        if(curseur_g->count()>1)
            ui->cu_lb_y6->show();
        if(curseur_d->count()>1)
            ui->cu1_lb_y6->show();
        if(serie_psy->count()>2)
        {
            pt=this->extrapol(curseur_g->at(1).x(),serie_psy);
            ui->cu_lb_y6->setText(a.number(pt.y(),'G',4));
            pt=this->extrapol(curseur_d->at(1).x(),serie_psy);
            ui->cu1_lb_y6->setText(a.number(pt.y(),'G',4));
        }
    }
    else
    {
        serie_psy->hide();
        axis_psy->hide();
        ui->cu_lb_y6->hide();
        ui->cu1_lb_y6->hide();
    }
}

void MainWindow::on_aff_cb_teta_stateChanged(int arg1)
{
    QPointF pt;
    QString a;
    if(arg1)
    {
        serie_teta->show();
        axis_teta->show();
        if(curseur_g->count()>1)
            ui->cu_lb_y5->show();
        if(curseur_d->count()>1)
            ui->cu1_lb_y5->show();
        if(serie_teta->count()>2)
        {
            pt=this->extrapol(curseur_g->at(1).x(),serie_teta);
            ui->cu_lb_y5->setText(a.number(pt.y(),'G',4));
            pt=this->extrapol(curseur_d->at(1).x(),serie_teta);
            ui->cu1_lb_y5->setText(a.number(pt.y(),'G',4));
        }
    }
    else
    {
        serie_teta->hide();
        axis_teta->hide();
        ui->cu_lb_y5->hide();
        ui->cu1_lb_y5->hide();
    }
}

void MainWindow::envoie_cmd_float(unsigned char cmd,unsigned char option,float data)
{
    QByteArray data_to_send('0',8);
    void *tmp;
    unsigned char *ctmp;

    tmp= &data;
    ctmp=(unsigned char*)tmp;
    data_to_send[0]='?'; // #
    data_to_send[1]=cmd;
    data_to_send[2]=option;
    data_to_send[3]=*(ctmp);
    data_to_send[4]=*(ctmp+1);
    data_to_send[5]=*(ctmp+2);
    data_to_send[6]=*(ctmp+3);
    data_to_send[7]='!'; // $
    if(serial->isOpen())
    {
        serial->write(data_to_send);
    }
    data_to_send.clear();
}

void MainWindow::envoie_cmd_char(unsigned char cmd,unsigned char option,char data)
{
    QByteArray data_to_send('0',8);
    data_to_send[0]='?'; // #
    data_to_send[1]=cmd;
    data_to_send[2]=option;
    data_to_send[3]=data;
    data_to_send[4]='0';
    data_to_send[5]='0';
    data_to_send[6]='0';
    data_to_send[7]='!'; // $
    if(serial->isOpen())
    {
        serial->write(data_to_send);
    }
    data_to_send.clear();
}

void MainWindow::envoie_trame_config()
{
    if(ui->combobox_mode_fct->currentIndex()==0) // PID Simple pas de consigne car vertical
    {
        //envoie Valeur PID Teta
        envoie_cmd_float('p','0',(float)ui->pid1_sb_kp->value());
        envoie_cmd_float('i','0',(float)ui->pid1_sb_ki->value());
        envoie_cmd_float('d','0',(float)ui->pid1_sb_kd->value());
        //Start
        envoie_cmd_char('S','1','1');
    }
    else if(ui->combobox_mode_fct->currentIndex()==1) // PID double envoie de consigne
    {
        //envoie consigne
        envoie_cmd_char('C','1',ui->cons_cb_type->currentIndex());
        envoie_cmd_float('C','2',(float)ui->cons_sb_per->value());
        envoie_cmd_float('C','3',(float)ui->cons_sb_vinit->value()/360.0);
        //envoie Valeur PID Teta
        envoie_cmd_float('p','0',(float)ui->pid1_sb_kp->value());
        envoie_cmd_float('i','0',(float)ui->pid1_sb_ki->value());
        envoie_cmd_float('d','0',(float)ui->pid1_sb_kd->value());
        //envoie Valeur PID Psy
        envoie_cmd_float('p','1',(float)ui->pid2_sb_kp->value());
        envoie_cmd_float('i','1',(float)ui->pid2_sb_ki->value());
        envoie_cmd_float('d','1',(float)ui->pid2_sb_kd->value());
        //Start
        envoie_cmd_char('S','1','2');
    }
    else if(ui->combobox_mode_fct->currentIndex()==2) // PID + RST envoie de consigne
    {
        //Start
        envoie_cmd_char('S','0','3');
    }
    else // boucle ouverte on n'envoie rien
    {
        //envoie consigne
        envoie_cmd_char('C','1',ui->cons_cb_type->currentIndex());
        envoie_cmd_float('C','2',(float)ui->cons_sb_per->value());
        envoie_cmd_float('C','3',(float)ui->cons_sb_vinit->value());
        //Start
        envoie_cmd_char('S','1','4');
    }
}

void MainWindow::on_pushbutton_matlab_clicked()
{
    int i;
    QFile fichiercsv;
    QString tmp,filename, stemps, sconsigne, serreur,sspid_psy,sspid_teta,spsy,steta;
    filename=QFileDialog::getSaveFileName();
    fichiercsv.setFileName(filename);

    if (fichiercsv.open(QFile::WriteOnly|QFile::Truncate))
    {
        QTextStream stream(&fichiercsv);
        stream.setLocale(QLocale("fr"));
        stemps.append("temps =[");
        sconsigne.append("consigne = [");
        serreur.append("erreur =[");
        sspid_psy.append("spid_psy =[");
        sspid_teta.append("spid_teta =[");
        spsy.append("psy =[");
        steta.append("teta =[");
        for(i=0;i<serie_consigne->count();i++)
        {
            stemps.append(tmp.number(serie_consigne->at(i).x())+";");
            sconsigne.append(tmp.number(serie_consigne->at(i).y())+";");
            serreur.append(tmp.number(serie_erreur_psy->at(i).y())+";");
            sspid_psy.append(tmp.number(serie_spid_psy->at(i).y())+";");
            sspid_teta.append(tmp.number(serie_spid_teta->at(i).y())+";");
            spsy.append(tmp.number(serie_psy->at(i).y())+";");
            steta.append(tmp.number(serie_teta->at(i).y())+";");
        }
        stemps.append("]; \r\n");
        sconsigne.append("]; \r\n");
        serreur.append("]; \r\n");
        sspid_psy.append("]; \r\n");
        sspid_teta.append("]; \r\n");
        spsy.append("]; \r\n");
        steta.append("]; \r\n");
        stream << stemps;
        stream << sconsigne;
        stream << serreur;
        stream << sspid_psy;
        stream << sspid_teta;
        stream << spsy;
        stream << steta;
        fichiercsv.close();
    }

}

void MainWindow::on_dev_label_clicked()
{
   QMessageBox::about(this,"IHM TP Automatique - Pendule",
                       "<h4>TP Automatique 4° Année</h4>\n\n"
                       "TP Automatique <br>"
                       "Copyright 2023 Centrale Lyon-Enise.<br>"
                       "Support: Galtier Bhiri Granjon - ENISE <br>"
                       "<a href=\"http://www.enise.fr\">http://www.enise.fr</a>" );

}
void MainWindow::on_pushbutton_csv_clicked()
{
    int i;
    QFile fichiercsv;
    QString filename;
    filename=QFileDialog::getSaveFileName();// fd->directory().path()+"/fichiercsv.csv");
    fichiercsv.setFileName(filename);

    if (fichiercsv.open(QFile::WriteOnly|QFile::Truncate))
    {
        QTextStream stream(&fichiercsv);
        stream.setLocale(QLocale("fr"));
        for(i=0;i<serie_consigne->count();i++)
        {
            stream << serie_consigne->at(i).x()<<";"<< serie_consigne->at(i).y()<<";"<< serie_erreur_psy->at(i).y()<<";" \
                   << serie_spid_psy->at(i).y()<<";"<< serie_spid_teta->at(i).y()<<";"<< serie_psy->at(i).y()<<";" \
                   << serie_teta->at(i).y()<<"\r\n" ;
        }
        fichiercsv.close();
    }

}
void MainWindow::grise_commande(bool on_off)
{
    ui->groupbox_pid1->setEnabled(on_off);
    ui->groupbox_pid2->setEnabled(on_off);
    ui->groupBox_mode_fct->setEnabled(on_off);
    ui->groupBox_consigne->setEnabled(on_off);
}


void MainWindow::append_point()
{
    long cpt;
    double compteur_sec=0;
    QVector<rcv_message> tmp;
    tmp=tab_data;
    tab_data.clear();
    if(tmp.size()==0)
    {return;}
    serial_port_label->setText("Port Série: "+serial->portName()+ "   Tech = 2ms");
  //   qDebug()<< "Tech: "<<tmp.at(0).Tech <<"\r\n";
    for(cpt=0;cpt<(tmp.size());cpt++)
    {
        compteur=compteur+2;
        compteur_sec=compteur/1000;
        if(ui->combobox_mode_fct->currentIndex()==3)
        {
            serie_consigne->append(compteur_sec,tmp.at(cpt).consigne/10);
        }
        else
        {
            serie_consigne->append(compteur_sec,tmp.at(cpt).consigne);
        }
        serie_psy->append(compteur_sec,tmp.at(cpt).mes_psy);
        serie_erreur_psy->append(compteur_sec,tmp.at(cpt).scorr_psy);
        serie_spid_psy->append(compteur_sec,tmp.at(cpt).scorr_psy);
        serie_spid_teta->append(compteur_sec,tmp.at(cpt).scorr_teta);
        serie_teta->append(compteur_sec,tmp.at(cpt).mes_teta);
        // Pour le mot de controle sur 8 bits:
        // 7    6   5   4   3   2   1   0
        // ??   ??  ??  ??  ??  Pb  ATU AT
 /*       etat_label->setText("Système :");
        if(tmp.at(cpt).control&0x02)
        {
            etat_label->setText("Système: ATU");
        }
        else
        {
            etat_label->setText("Système: Marche");
        }
        if(tmp.at(cpt).control&0x01)
        {
            etat_label->setText("Système: Arrêt");
        }
        else
        {
            etat_label->setText("Système: Marche");
        }
        if(tmp.at(cpt).control&0x04)
        {
            etat_label->setText(etat_label->text()+" Défaut");
        }
*/
        // if(tech<20)
        // {
        //     if (nbr>NB_PT_SCROLLING)
        //     {
        //       /*  if(((nbr)%NB_PT_SCROLLING)==0)
        //         {
        //             ymax_consigne=-100000;
        //             ymin_consigne=100000;
        //             ymax_erreur=-100000;
        //             ymin_erreur=100000;
        //             ymax_spid_psy=-100000;
        //             ymin_spid_psy=100000;
        //             ymax_spid_teta=-100000;
        //             ymin_spid_teta=100000;
        //             ymax_psy=-100000;
        //             ymin_psy=100000;
        //             ymax_teta=-100000;
        //             ymin_teta=100000;
        //         }*/
        //         serie_consigne->removePoints(0,1);
        //         serie_erreur_psy->removePoints(0,1);
        //         serie_spid_psy->removePoints(0,1);
        //         serie_spid_teta->removePoints(0,1);
        //         serie_psy->removePoints(0,1);
        //         serie_teta->removePoints(0,1);
        //     }
        //     nbr++;
        // }
    }
    //  scrolling horizontal
if(tmp.size()>100)
{}
else
{
   if((compteur-NB_PT_SCROLLING*tech)>0)
    {
        //chart->axes(Qt::Horizontal).back()->setRange((compteur-NB_PT_SCROLLING*tech)/1000,compteur_sec+0.1);
       // chart->axisX()->setRange((compteur-NB_PT_SCROLLING*tech)/1000,compteur_sec+0.1);
       axe_temps->setRange((compteur-NB_PT_SCROLLING*tech)/1000,compteur_sec+0.1);
     /*   if(ui->combobox_mode_fct->currentIndex()==0)
        {
            if(ymin_consigne<ymin_psy)
            {
                ymin_psy=ymin_consigne;
            }
            else if(ymin_consigne>ymin_psy)
            {
                ymin_consigne=ymin_psy;
            }
            if(ymax_consigne>ymax_psy)
            {
                ymax_psy=ymax_consigne;
            }
            else if(ymax_consigne<ymax_psy)
            {
                ymax_consigne=ymax_psy;
            }
            axis_consigne->setRange(ymin_psy,ymax_psy);
        }
        else if((ui->combobox_mode_fct->currentIndex()==1)||(ui->combobox_mode_fct->currentIndex()==2))
        {
            if(ymin_consigne<ymin_teta)
            {
                ymin_teta=ymin_consigne;
            }
            else if(ymin_consigne>ymin_teta)
            {
                ymin_consigne=ymin_teta;
            }
            if(ymax_consigne>ymax_teta)
            {
                ymax_teta=ymax_consigne;
            }
            else if(ymax_consigne<ymax_teta)
            {
                ymax_consigne=ymax_teta;
            }
            axis_consigne->setRange(ymin_teta,ymax_teta);
        }
        else
        {
            axis_consigne->setRange(ymin_consigne,ymax_consigne);
        }
        axis_erreur_psy->setRange(ymin_erreur_psy,ymax_erreur_psy);
        axis_spid_psy->setRange(ymin_spid_psy,ymax_spid_psy);
        axis_spid_teta->setRange(ymin_spid_teta,ymax_spid_teta);
        axis_psy->setRange(ymin_psy,ymax_psy);
        axis_teta->setRange(ymin_teta,ymax_teta);*/
    }
    else
    {
       //chart->axes(Qt::Horizontal).back()->setRange(0,compteur_sec+0.1);
        //chart->axisX()->setRange(0,compteur_sec+0.1);
        axe_temps->setRange(0, (0.1 + compteur_sec));
        chart->axes(Qt::Horizontal).back()->setRange(0, (0.1 + compteur_sec));
     /*  if(ui->combobox_mode_fct->currentIndex()==0)
        {
            if(ymin_consigne<ymin_psy)
            {
                ymin_psy=ymin_consigne;
            }
            else if(ymin_consigne>ymin_psy)
            {
                ymin_consigne=ymin_psy;
            }
            if(ymax_consigne>ymax_psy)
            {
                ymax_psy=ymax_consigne;
            }
            else if(ymax_consigne<ymax_psy)
            {
                ymax_consigne=ymax_psy;
            }
            axis_consigne->setRange(ymin_psy,ymax_psy);
        }
        else if((ui->combobox_mode_fct->currentIndex()==1)||(ui->combobox_mode_fct->currentIndex()==2))
        {
            if(ymin_consigne<ymin_teta)
            {
                ymin_teta=ymin_consigne;
            }
            else if(ymin_consigne>ymin_teta)
            {
                ymin_consigne=ymin_teta;
            }
            if(ymax_consigne>ymax_teta)
            {
                ymax_teta=ymax_consigne;
            }
            else if(ymax_consigne<ymax_teta)
            {
                ymax_consigne=ymax_teta;
            }
            axis_consigne->setRange(ymin_teta,ymax_teta);
        }
        else
        {
            axis_consigne->setRange(ymin_consigne,ymax_consigne);
        }
        axis_erreur_psy->setRange(ymin_erreur_psy,ymax_erreur_psy);
        axis_spid_psy->setRange(ymin_spid_psy,ymax_spid_psy);
        axis_spid_teta->setRange(ymin_spid_teta,ymax_spid_teta);
        axis_psy->setRange(ymin_psy,ymax_psy);
        axis_teta->setRange(ymin_teta,ymax_teta);  */
    }

    replot_cursor();
    }
}

void MainWindow::on_btn_start_acq_toggled(bool checked)
{
    if (checked)
    {
        QString port_name;
        foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        {
        /*  qDebug() << "Name : " << info.portName();
                qDebug() << "Description : " << info.description();
                qDebug() << "Manufacturer: " << info.manufacturer();
                qDebug() << "VID " << info.vendorIdentifier();
                qDebug() << "PID" << info.productIdentifier();
                qDebug() << index << "\r\n"; */
            if ((info.vendorIdentifier() == 1204) && (info.productIdentifier() == 2))
            {
                port_name = info.portName();
            }
            else if ((info.vendorIdentifier() == 0x04b4) && (info.productIdentifier() == 0xf139))
            {
                port_name = info.portName();
            }
            else if ((info.vendorIdentifier() == 0x0483) && (info.productIdentifier() == 0x374b))
            {
                port_name = info.portName();
            }
        }
        if (serial->isOpen())
        {
            serial->disconnect(serial, SIGNAL(readyRead()), this,SLOT(serialreceived()));
            serial->flush();
            serbuf.clear();
            serial->close();
        }
        rcv_serie.nb_char = 0;
        rcv_serie.rcv_ok = false;
#ifdef OS_LINUX
        serial->setPortName("/dev/" + port_name);
        //  serial->setPortName("/dev/pts/1"); // only for debug
#elif
        serial->setPortName(portname);
#endif
        if (port_name == "")
        {
            ui->btn_start_acq->setChecked(false);
            serial_port_label->setText("Port Série: Clos");
        }
        else
        {
            tab_data.clear();
            serbuf.clear();
            this->grise_commande(false);
            serial->setBaudRate(115200); // 921600);
            serial->setDataBits(QSerialPort::Data8);
            serial->setParity(QSerialPort::NoParity);
            serial->setStopBits(QSerialPort::OneStop);
            serial->setFlowControl(QSerialPort::NoFlowControl);
            serial->setReadBufferSize(0);
            serial->open(QIODevice::ReadWrite);
            serial->connect(serial, SIGNAL(readyRead()), this,SLOT(serialreceived()), Qt::UniqueConnection);
            serial_port_label->setText("Port série: " + serial->portName());
            chart_update->start(100);
            marche_bool = 1;
            envoie_trame_config();
            ui->btn_start_acq->setText("Stop Acquisition");
        }
    }
    else
    {
        serial->disconnect(serial, SIGNAL(readyRead()), this,SLOT(serialreceived()));
        chart_update->stop();
        marche_bool = 0;
        this->grise_commande(true);
        envoie_cmd_char('S','0','0');
        serial_port_label->setText("Port Série: Clos");
        //serial->putChar('S');
        while (!serial->flush());
       // serial->clear(QSerialPort::AllDirections);
        serbuf.clear();
        //serial->close();
        ui->btn_start_acq->setText("Start Acquisition");
    }
}

void MainWindow::on_aff_cb_pid_psy_stateChanged(int arg1)
{
    QPointF pt;
    QString a;
    if(arg1)
    {
        serie_spid_psy->show();
        axis_spid_psy->show();
        if(curseur_g->count()>1)
            ui->cu_lb_y4->show();
        if(curseur_d->count()>1)
            ui->cu1_lb_y4->show();
        if(serie_spid_psy->count()>2)
        {
            pt=this->extrapol(curseur_g->at(1).x(),serie_spid_psy);
            ui->cu_lb_y4->setText(a.number(pt.y(),'G',4));
            pt=this->extrapol(curseur_d->at(1).x(),serie_spid_psy);
            ui->cu1_lb_y4->setText(a.number(pt.y(),'G',4));
        }
    }
    else
    {
        serie_spid_psy->hide();
        axis_spid_psy->hide();
        ui->cu_lb_y4->hide();
        ui->cu1_lb_y4->hide();
    }
}


void MainWindow::on_aff_cb_pid_teta_stateChanged(int arg1)
{
    QPointF pt;
    QString a;
    if(arg1)
    {
        serie_spid_teta->show();
        axis_spid_teta->show();
        if(curseur_g->count()>1)
            ui->cu_lb_y3->show();
        if(curseur_d->count()>1)
            ui->cu1_lb_y3->show();
        if(serie_spid_teta->count()>2)
        {
            pt=this->extrapol(curseur_g->at(1).x(),serie_spid_teta);
            ui->cu_lb_y3->setText(a.number(pt.y(),'G',4));
            pt=this->extrapol(curseur_d->at(1).x(),serie_spid_teta);
            ui->cu1_lb_y3->setText(a.number(pt.y(),'G',4));
        }
    }
    else
    {
        serie_spid_teta->hide();
        axis_spid_teta->hide();
        ui->cu_lb_y3->hide();
        ui->cu1_lb_y3->hide();
    }

}




// void MainWindow::on_cons_cb_type_currentIndexChanged(int index)
// {
//     //    this->envoie_trame_config();
// }

// void MainWindow::on_cons_sb_per_editingFinished()
// {
//     //    this->envoie_trame_config();
// }

// void MainWindow::on_cons_sb_vinit_editingFinished()
// {
//     //    this->envoie_trame_config();
// }

// void MainWindow::on_cons_sd_vfin_editingFinished()
// {
//     //    this->envoie_trame_config();
// }

// void MainWindow::on_pid1_sb_kp_editingFinished()
// {
//     //    this->envoie_trame_config();
// }

// void MainWindow::on_pid2_sb_kp_editingFinished()
// {
//     //    this->envoie_trame_config();
// }

// void MainWindow::on_pid1_sb_ki_editingFinished()
// {
//     //    this->envoie_trame_config();
// }

// void MainWindow::on_pid2_sb_ki_editingFinished()
// {
//     //    this->envoie_trame_config();
// }

// void MainWindow::on_pid1_sb_kd_editingFinished()
// {
//     //    this->envoie_trame_config();
// }

// void MainWindow::on_pid2_sb_kd_editingFinished()
// {
//     //    this->envoie_trame_config();
// }
// void MainWindow::pt_added(int index)
// {
//     qreal y = serie_consigne->at(index).y();
//     if(y< ymin_consigne || y > ymax_consigne)
//     {
//         if(y < ymin_consigne)
//             ymin_consigne = y;
//         if(y> ymax_consigne)
//             ymax_consigne = y;
//     }
//     if(ymax_consigne-ymin_consigne<1)
//     {
//         ymax_consigne=ymin_consigne+1;
//     }

//     y = serie_erreur_psy->at(index).y();
//     if(y< ymin_erreur_psy || y > ymax_erreur_psy)
//     {
//         if(y < ymin_erreur_psy)
//             ymin_erreur_psy = y;
//         if(y> ymax_erreur_psy)
//             ymax_erreur_psy = y;
//     }
//     if(ymax_erreur_psy-ymin_erreur_psy<1)
//     {
//         ymax_erreur_psy=ymin_erreur_psy+1;
//     }

//     y = serie_spid_psy->at(index).y();
//     if(y< ymin_spid_psy || y > ymax_spid_psy)
//     {
//         if(y < ymin_spid_psy)
//             ymin_spid_psy = y;
//         if(y> ymax_spid_psy)
//             ymax_spid_psy = y;
//     }
//     if(ymax_spid_psy-ymin_spid_psy<1)
//     {
//         ymax_spid_psy=ymin_spid_psy+1;
//     }

//     y = serie_spid_teta->at(index).y();
//     if(y< ymin_spid_teta || y > ymax_spid_teta)
//     {
//         if(y < ymin_spid_teta)
//             ymin_spid_teta = y;
//         if(y> ymax_spid_teta)
//             ymax_spid_teta = y;
//     }
//     if(ymax_spid_teta-ymin_spid_teta<1)
//     {
//         ymax_spid_teta=ymin_spid_teta+1;
//     }

//     y = serie_psy->at(index).y();
//     if(y< ymin_psy || y > ymax_psy)
//     {
//         if(y < ymin_psy)
//             ymin_psy = y;
//         if(y> ymax_psy)
//             ymax_psy = y;
//     }
//     if(ymax_psy-ymin_psy<1)
//     {
//         ymax_psy=ymin_psy+1;
//     }

//     y = serie_teta->at(index).y();
//     if(y< ymin_teta || y > ymax_teta)
//     {
//         if(y < ymin_teta)
//             ymin_teta = y;
//         if(y> ymax_teta)
//             ymax_teta = y;
//     }
//     if(ymax_teta-ymin_teta<1)
//     {
//         ymax_teta=ymin_teta+1;
//     }
// }

// void MainWindow::pt_consigne_added(int index)
// {
//     qreal y = serie_consigne->at(index).y();

//     if(y< ymin_consigne || y > ymax_consigne)
//     {
//         if(y < ymin_consigne)
//             ymin_consigne = y;
//         if(y> ymax_consigne)
//             ymax_consigne = y;
//     }
//     if(ymax_consigne-ymin_consigne<1)
//     {
//         ymax_consigne=ymin_consigne+1;
//     }

//     //    axis_consigne->setRange(ymin_consigne,ymax_consigne);
// }

// void MainWindow::pt_erreur_added(int index)
// {
//     qreal y = serie_erreur_psy->at(index).y();

//     if(y< ymin_erreur_psy || y > ymax_erreur_psy)
//     {
//         if(y < ymin_erreur_psy)
//             ymin_erreur_psy = y;
//         if(y> ymax_erreur_psy)
//             ymax_erreur_psy = y;
//     }
//     if(ymax_erreur_psy-ymin_erreur_psy<1)
//     {
//         ymax_erreur_psy=ymin_erreur_psy+1;
//     }
//     //  axis_erreur->setRange(ymin_erreur,ymax_erreur);
// }

// void MainWindow::pt_spid_psy_added(int index)
// {
//     qreal y = serie_spid_psy->at(index).y();

//     if(y< ymin_spid_psy || y > ymax_spid_psy)
//     {
//         if(y < ymin_spid_psy)
//             ymin_spid_psy = y;
//         if(y> ymax_spid_psy)
//             ymax_spid_psy = y;
//     }
//     if(ymax_spid_psy-ymin_spid_psy<1)
//     {
//         ymax_spid_psy=ymin_spid_psy+1;
//     }
//     //    axis_spid_psy->setRange(ymin_spid_psy,ymax_spid_psy);
// }

// void MainWindow::pt_spid_teta_added(int index)
// {
//     qreal y = serie_spid_teta->at(index).y();

//     if(y< ymin_spid_teta || y > ymax_spid_teta)
//     {
//         if(y < ymin_spid_teta)
//             ymin_spid_teta = y;
//         if(y> ymax_spid_teta)
//             ymax_spid_teta = y;
//     }
//     if(ymax_spid_teta-ymin_spid_teta<1)
//     {
//         ymax_spid_teta=ymin_spid_teta+1;
//     }
//     //  axis_spid_teta->setRange(ymin_spid_teta,ymax_spid_teta);
// }

// void MainWindow::pt_psy_added(int index)
// {
//     qreal y = serie_psy->at(index).y();

//     if(y< ymin_psy || y > ymax_psy)
//     {
//         if(y < ymin_psy)
//             ymin_psy = y;
//         if(y> ymax_psy)
//             ymax_psy = y;
//     }
//     if(ymax_psy-ymin_psy<1)
//     {
//         ymax_psy=ymin_psy+1;
//     }
//     //  axis_psy->setRange(ymin_psy,ymax_psy);
// }

// void MainWindow::pt_teta_added(int index)
// {
//     qreal y = serie_teta->at(index).y();

//     if(y< ymin_teta || y > ymax_teta)
//     {
//         if(y < ymin_teta)
//             ymin_teta = y;
//         if(y> ymax_teta)
//             ymax_teta = y;
//     }
//     if(ymax_teta-ymin_teta<1)
//     {
//         ymax_teta=ymin_teta+1;
//     }
//     //  axis_teta->setRange(ymin_teta,ymax_teta);
// }
