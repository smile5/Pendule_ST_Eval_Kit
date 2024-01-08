#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define OS_LINUX
#define NB_PT_SCROLLING 20000
#include "chartview.h"
#include "clickableqlabel.h"
#include "lineseries.h"
#include <QMainWindow>
#include <QLabel>
#include <QSerialPortInfo>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QCategoryAxis>
#include <QDebug>

#ifdef QT5_VERSION
QT_CHARTS_USE_NAMESPACE
#endif


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

#define COLOR_CONSIGNE QColor(193,125,17)
#define COLOR_ERREUR_PSY QColor(255,0,0)
#define COLOR_ERREUR_TETA QColor(255,0,0)
#define COLOR_SPID_PSY QColor(255,255,0)
#define COLOR_SPID_TETA QColor(255, 166, 0)
#define COLOR_PSY QColor(115,210,22)
#define COLOR_TETA QColor(114,159,207)

 // typedef struct{
 //        unsigned short Tech; // Temps echantillonnage en multiple de 1ms
 //        char nb_boucles; // nb boucles '0' boucle ouverte, '1' une boucle, '2' 2 boucles imbriquÃ©es , '3' boucle ouvert avec signal du PC
 //        unsigned short Kp1_num; // numerateur KP 1 - Proportionnel  - si 1 boucle alors seule la boucle 1 est active
 //        unsigned short Kp1_den; // Denominateur KP 1
 //        unsigned short Kp2_num; // numerateur KP 2
 //        unsigned short Kp2_den; // Denominateur KP 2
 //        unsigned short Kd1_num; // numerateur Kd 1 - DÃ©rivÃ©e
 //        unsigned short Kd1_den; // Denominateur Kd 1
 //        unsigned short Kd2_num; // numerateur Kd 2
 //        unsigned short Kd2_den; // Denominateur Kd 2
 //        unsigned short Ki1_num; // numerateur Ki 1 - IntÃ©gral
 //        unsigned short Ki1_den; // Denominateur Ki 1
 //        unsigned short Ki2_num; // numerateur Ki 2
 //        unsigned short Ki2_den; // Denominateur Ki 2
 //        char type_consigne; // A revoir pas correct 'E' Echellon, 'R' Rampe, 'S' Sinus, 'T' triangle, C'carrÃ©e, 'W' dent de scie
 //        short cons_initiale; //100 * pourcentage initial
 //        short cons_finale; //100 * pourcentage final
 //        unsigned short cons_per_num; // numerateur Kd 1 - DÃ©rivÃ©e
 //        unsigned short cons_per_den; // Denominateur Kd 1
 //        unsigned short CRC;
 //    } config;

    typedef struct{
        float mes_teta;
        float mes_psy;
        float consigne;
        float scorr_teta;
        float scorr_psy;
        bool rcv_ok;
        unsigned char nb_char;
        QByteArray buffer;
    } rcv_message;
public:
    explicit MainWindow(QWidget *parent = 0);
     QSerialPortInfo serialPortInfo;
     ClickableLabel *dev_label;
     QLabel *serial_port_label;
     QLabel *etat_label,*deltax_label;
     LineSeries *serie_consigne,*serie_erreur_psy,*serie_erreur_teta,*serie_spid_psy,*serie_spid_teta,*serie_teta,*serie_psy;
     QLineSeries *curseur_g,*curseur_d;
     QValueAxis *axis_consigne,*axis_erreur_teta,*axis_erreur_psy,*axis_spid_teta,*axis_spid_psy,*axis_teta,*axis_psy;

     QChart *chart ;
     ChartView *chartview;
     double compteur=0;
     ~MainWindow();
    // config config_ui;
     rcv_message rcv_serie;
     QGraphicsSimpleTextItem txt_curs1,txt_curs2;
     QGraphicsLineItem line_curs1;
     QPointF pos_curs1,pos_curs2;
     QTimer *timer;

private slots:
    void replot_rubber(QRect rect, QPointF pos1, QPointF pos2);
    void replot_area(QRectF Frect);
    void replot_zoom();
    void replot_zoomreset();
    void replot_scroll();
    void serial_timeout();
    // void pt_added(int index);
    // void pt_consigne_added(int index);
    // void pt_erreur_added(int index);
    // void pt_spid_psy_added(int index);
    // void pt_spid_teta_added(int index);
    // void pt_psy_added(int index);
    // void pt_teta_added(int index);
    void clear_series();
    void cliick(int);
    void test(QPointF);
    void serialreceived();
//    void on_serial_cb_currentIndexChanged(const QString &arg1);
    void on_combobox_mode_fct_currentIndexChanged(int index);
    // void on_cons_cb_type_currentIndexChanged(int index);
    // void on_cons_sb_per_editingFinished();
    // void on_cons_sb_vinit_editingFinished();
    // void on_cons_sd_vfin_editingFinished();
    // void on_pid1_sb_kp_editingFinished();
    // void on_pid2_sb_kp_editingFinished();
    // void on_pid1_sb_ki_editingFinished();
    // void on_pid2_sb_ki_editingFinished();
    // void on_pid1_sb_kd_editingFinished();
    // void on_pid2_sb_kd_editingFinished();
    void on_aff_cb_consigne_stateChanged(int arg1);
    void on_aff_cb_erreur_psy_stateChanged(int arg1);
    void on_aff_cb_psy_stateChanged(int arg1);
    void on_aff_cb_teta_stateChanged(int arg1);
    void on_pushbutton_matlab_clicked();
    void on_pushbutton_csv_clicked();
    void on_dev_label_clicked();
    void append_point();
    void on_btn_start_acq_toggled(bool checked);

    void on_aff_cb_pid_teta_stateChanged(int arg1);

    void on_aff_cb_pid_psy_stateChanged(int arg1);

private:
    QSerialPort *serial;
    Ui::MainWindow *ui;
         QValueAxis *axe_temps;
    QPointF extrapol(qreal x, QLineSeries *serie);
    QPointF pt_clicked;
    qreal ymin_consigne,ymin_erreur_psy,ymin_erreur_teta,ymin_spid_psy,ymin_spid_teta,ymin_psy,ymin_teta;
    qreal ymax_consigne,ymax_erreur_psy,ymax_erreur_teta,ymax_spid_psy,ymax_spid_teta,ymax_psy,ymax_teta;
    void envoie_trame_config(void);
    void replot_cursor(void);
    void calc_num_den_float(double value,unsigned short &value_num,unsigned short &value_den);
    void calc_num_den_float_per(double value,unsigned short &value_num,unsigned short &value_den);
    bool curseur_bt_g,curseur_bt_d;
    QByteArray serbuf;
    void grise_commande(bool on_off);
    QVector<rcv_message> tab_data;
    QTimer *chart_update;
    long nbr;
    int tech;
    unsigned char marche_bool;
    void envoie_cmd_float(unsigned char cmd,unsigned char option,float data);
    void envoie_cmd_char(unsigned char cmd,unsigned char option,char data);
};

#endif // MAINWINDOW_H
