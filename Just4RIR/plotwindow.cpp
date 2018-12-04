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

#include "plotwindow.h"
#include "ui_plotwindow.h"

plotWindow::plotWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::plotWindow)
{
    ui->setupUi(this);

    ui->customPlot->legend->setVisible(true);
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                      QCP::iSelectLegend | QCP::iSelectPlottables | QCP::iMultiSelect);
    ui->customPlot->legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items

      // connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
      connect(ui->customPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
      connect(ui->customPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));

      // connect slot that ties some axis selections together (especially opposite axes):
      connect(ui->customPlot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));

      // setup policy and connect slot for context menu popup:
      ui->customPlot->setContextMenuPolicy(Qt::CustomContextMenu);
      connect(ui->customPlot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));



}

plotWindow::~plotWindow()
{
    delete ui;
}

void plotWindow::setYLabel(QString nom)
{
    ui->customPlot->yAxis->setLabel(nom);
}

void plotWindow::setTitle(QString nom)
{
    // add the text label at the top:
    QCPItemText *textLabel = new QCPItemText(ui->customPlot);// = new QCPItemText(plotWindow);
    textLabel->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
    textLabel->position->setType(QCPItemPosition::ptAxisRectRatio);
    textLabel->position->setCoords(0.5, 0); // place position at center/top of axis rect
    textLabel->setText(nom);
    textLabel->setFont(QFont(font().family(), 16)); // make font a bit larger
    textLabel->setPen(QPen(Qt::black)); // show black border around text

}

void plotWindow::hideLegend()
{
    ui->customPlot->legend->setVisible(false);
}

void plotWindow::setX(int min, int max)
{
    ui->customPlot->xAxis->setRange(min, max);
}

void plotWindow::makePlot()
{
    int buf=0;
    // Création graphique des courbes
    for (int k = 0 ; k < courbe.size() ; k++) // pour chaque bande
    {
        ui->customPlot->addGraph();
        ui->customPlot->graph(k+buf)->setData(vectX, courbe[k]);
        QString nom = QString::number(62.5*pow(2,k)) + "Hz";
        ui->customPlot->graph(k+buf)->setName(nom);
        ui->customPlot->graph(k+buf)->setPen(QPen(QColor::fromHsv(360/courbe.size()*(k+buf),255,255)));

        if (courbe.size() == 16) // dacay curve
        {
            buf++;
            ui->customPlot->addGraph();
            ui->customPlot->graph(k+buf)->setData(vectX, courbe[k+8]);
            QString nom = QString::number(62.5*pow(2,k)) + "Hz - Integrated";
            ui->customPlot->graph(k+buf)->setName(nom);
            ui->customPlot->graph(k+buf)->setPen(QPen(QColor::fromHsv(360/courbe.size()*(k+buf),255,255)));
            if (k==7) break;
        }
    }


    // Noms des axes
    ui->customPlot->xAxis->setLabel("Time (ms)");
    if(m_echelleLog)
        ui->customPlot->yAxis->setLabel("Normalized energy (log)");
    else
        ui->customPlot->yAxis->setLabel("Noormalized energy (linear)");

    // Regalges des plages des axes
    ui->customPlot->xAxis->setRange(xMin, xMax);
    ui->customPlot->yAxis->setRange(yMin,yMax);
    ui->customPlot->replot();
}

void plotWindow::XY(std::vector<float> &x, std::vector<std::vector<double> > &y, float seuil)
{
    // Conversion en double
    std::vector<double> vX(x.begin(),x.end());

    // Conversion en QVector
    vectX = QVector<double>::fromStdVector(vX);

    int k;
    // Recupération des extremums
    xMin = vectX[0];
    xMax = vectX[vectX.size() - 1];

    courbe.resize(y.size());
    yMin = 10*log10(seuil);
    yMax = 0;

    if (yMin >=yMax) yMin=-60; // Valeur par défault si le seuil est mal réglé.

    for (k=0 ; k < y.size() ; k++) // créaction de y.size courbes
    {
        std::vector<double> vY(y[k].begin(),y[k].end());
        courbe[k]  = QVector<double>::fromStdVector(vY);
        for(double &a : courbe[k]) {
            if (a<seuil) a=yMin;
            else a=10*log10(a);}
    }
    m_echelleLog = true;
}

void plotWindow::XY(std::vector<float> &x, std::vector<std::vector<double> > &y)
{
    // Conversion en double
    std::vector<double> vX(x.begin(),x.end());

    // Conversion en QVector
    vectX = QVector<double>::fromStdVector(vX);

    int k;
    // Recupération des extremums
    xMin = vectX[0];
    xMax = vectX[vectX.size() - 1];

    courbe.resize(y.size());


    yMin = (double)y[0][0];
    yMax = yMin;

    for (k=0 ; k < y.size() ; k++) // créaction de y.size courbes
    {
        std::vector<double> vY(y[k].begin(),y[k].end());
        courbe[k]  = QVector<double>::fromStdVector(vY);
        for(double &a : courbe[k]) {
            if (a<yMin) yMin=a;
            if (a>yMax) yMax=a;}
    }
    m_echelleLog = false;
}

void plotWindow::XY(std::vector<float> &x, std::vector<float> &y)
{
    // Conversion en double
    std::vector<double> vX(x.begin(),x.end());
    std::vector<double> vY(y.begin(),y.end());

    // Conversion en QVector
    vectX = QVector<double>::fromStdVector(vX);
    courbe.resize(1);
    courbe[0]  = QVector<double>::fromStdVector(vY);

    // Recupération des extremums
    xMin = vectX[0];
    xMax = vectX[vectX.size() - 1];
    yMin = *std::min_element(y.begin(), y.end());
    yMax = *std::max_element(y.begin(), y.end());

    m_echelleLog = false;
}

void plotWindow::XY(std::vector<float> &x, std::vector<int> &y)
{
    // Conversion en double
    std::vector<double> vX(x.begin(),x.end());
    std::vector<double> vY(y.begin(),y.end());

    // Conversion en QVector
    vectX = QVector<double>::fromStdVector(vX);
    courbe.resize(1);
    courbe[0]  = QVector<double>::fromStdVector(vY);

    // Recupération des extremums
    xMin = vectX[0];
    xMax = vectX[vectX.size() - 1];
    yMin = *std::min_element(y.begin(), y.end());
    yMax = *std::max_element(y.begin(), y.end());

    m_echelleLog = false;


}
// SLOTS :

void plotWindow::mousePress()
{
  // if an axis is selected, only allow the direction of that axis to be dragged
  // if no axis is selected, both directions may be dragged

  if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    ui->customPlot->axisRect()->setRangeDrag(ui->customPlot->xAxis->orientation());
  else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    ui->customPlot->axisRect()->setRangeDrag(ui->customPlot->yAxis->orientation());
  else
    ui->customPlot->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void plotWindow::mouseWheel()
{
  // if an axis is selected, only allow the direction of that axis to be zoomed
  // if no axis is selected, both directions may be zoomed

  if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->xAxis->orientation());
  else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->yAxis->orientation());
  else
    ui->customPlot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

void plotWindow::selectionChanged()
{
  /*
   normally, axis base line, axis tick labels and axis labels are selectable separately, but we want
   the user only to be able to select the axis as a whole, so we tie the selected states of the tick labels
   and the axis base line together. However, the axis label shall be selectable individually.

   The selection state of the left and right axes shall be synchronized as well as the state of the
   bottom and top axes.

   Further, we want to synchronize the selection of the graphs with the selection state of the respective
   legend item belonging to that graph. So the user can select a graph by either clicking on the graph itself
   or on its legend item.
  */

  // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      ui->customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    ui->customPlot->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    ui->customPlot->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      ui->customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    ui->customPlot->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    ui->customPlot->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }

  // synchronize selection of graphs with selection of corresponding legend items:
  for (int i=0; i<ui->customPlot->graphCount(); ++i)
  {
    QCPGraph *graph = ui->customPlot->graph(i);
    QCPPlottableLegendItem *item = ui->customPlot->legend->itemWithPlottable(graph);
    if (item->selected() || graph->selected())
    {
      item->setSelected(true);
      graph->setSelected(true);
    }
  }
}


void plotWindow::contextMenuRequest(QPoint pos)
{
  QMenu *menu = new QMenu(this);
  menu->setAttribute(Qt::WA_DeleteOnClose);

    if (ui->customPlot->selectedGraphs().size() > 0)
    {
        menu->addAction("Hide selected graph", this, SLOT(hideSelectedGraph()));
        menu->addAction("Display selected graph", this, SLOT(displaySelectedGraph()));
    }
    if (ui->customPlot->graphCount() > 0)
    {
        menu->addAction("Hide all graphs", this, SLOT(hideAllGraphs()));
        menu->addAction("Display all graphs", this, SLOT(displayAllGraphs()));
    }
    if (m_echelleLog)  menu->addAction("Linear scale", this, SLOT(linScale()));
    else           menu->addAction("Logarithme scale", this, SLOT(logScale()));

  menu->popup(ui->customPlot->mapToGlobal(pos));
}


void plotWindow::hideSelectedGraph()
{
  if (ui->customPlot->selectedGraphs().size() > 0)
  {
      foreach (QCPGraph* gr, ui->customPlot->selectedGraphs())
      {
          gr->setVisible(false);
      }
      ui->customPlot->replot();
  }
}

void plotWindow::displaySelectedGraph()
{
  if (ui->customPlot->selectedGraphs().size() > 0)
  {
      foreach (QCPGraph* gr, ui->customPlot->selectedGraphs())
      {
          gr->setVisible(true);
      }
      ui->customPlot->replot();
  }
}

void plotWindow::hideAllGraphs()
{
    for (int i = 0 ; i <ui->customPlot->graphCount() ; i++)
    {
        ui->customPlot->graph(i)->setVisible(false);
    }
    ui->customPlot->replot();
}

void plotWindow::displayAllGraphs()
{
    for (int i = 0 ; i <ui->customPlot->graphCount() ; i++)
    {
        ui->customPlot->graph(i)->setVisible(true);
    }
    ui->customPlot->replot();
}

void plotWindow::linScale()
{
    if (m_echelleLog)
    {
        for (int k=0; k < courbe.size() ; k++)
        {
            for(double &a : courbe[k]) {
                if(a <= -120) a=0;
                else a=pow(10, a/10);}
        }
        m_echelleLog = false;
        yMax = pow(10, yMax/10);
        yMin = pow(10, yMin/10);

        ui->customPlot->clearGraphs();
        makePlot();
    }
}

void plotWindow::logScale()
{
    if (!m_echelleLog)
    {
        for (int k=0; k < courbe.size() ; k++)
        {
            for(double &a : courbe[k]) {
                if(a<=0) a = -120;
                else a=10*log10(a);}
        }
        m_echelleLog = true;
        yMax = 10*log10(yMax);
        if (yMin<=0) yMin = -120;
        else yMin = 10*log10(yMin);

        ui->customPlot->clearGraphs();
        makePlot();
    }
}

