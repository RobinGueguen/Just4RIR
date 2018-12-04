/*/////////////////////////////// INFORMATIONS ///////////////////////////////////////
Software name : Just4RIR
Creation date : November 30th 2018
Last modification : November 30th 2018
Author : Robin Gueguen
License : GPL 3.0 2018
Property : Institut des Sciences du Calcul et des Données - Sorbonne Université
Version : 1.0 beta
Function : Display plot
*/////////////////////////////////////////////////////////////////////////////////////

#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include <QDialog>
#include"qcustomplot.h"

namespace Ui {
class plotWindow;
}

class plotWindow : public QDialog
{
    Q_OBJECT

public:
    explicit plotWindow(QWidget *parent = 0);
    ~plotWindow();

    void makePlot();
    void XY(std::vector<float> &x, std::vector<std::vector<double> > &y, float seuil);
    void XY(std::vector<float> &x, std::vector<std::vector<double> > &y);
    void XY(std::vector<float> &x, std::vector<float> &y); // courbe quelconque
    void XY(std::vector<float> &x, std::vector<int> &y);
    void setYLabel(QString nom);
    void setTitle(QString nom);
    void hideLegend();
    void setX(int min, int max);

private slots:
  void mousePress();
  void mouseWheel();
  void selectionChanged();
  void hideSelectedGraph();
  void hideAllGraphs();
  void displaySelectedGraph();
  void displayAllGraphs();
  void contextMenuRequest(QPoint pos);
  void linScale();
  void logScale();

private:
    Ui::plotWindow *ui;
    QCPItemText *textLabel;
    bool m_echelleLog;
    QVector<double> vectX, vectY;
    double xMin, xMax, yMax, yMin;
    QVector<QVector<double> > courbe;
};

#endif // PLOTWINDOW_H
