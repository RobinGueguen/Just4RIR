/*/////////////////////////////// INFORMATIONS ///////////////////////////////////////
Software name : Just4RIR
Creation date : November 30th 2018
Last modification : November 30th 2018
Author : Robin Gueguen
License : GPL 3.0 2018
Property : Institut des Sciences du Calcul et des Données - Sorbonne Université
Version : 1.0 beta
Function : Display perceptive factors
*/////////////////////////////////////////////////////////////////////////////////////

#include "data.h"
#include "ui_data.h"
#include "QDebug"

Data::Data(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Data)
{
    ui->setupUi(this);

    float f= 62.5;
    for (int i=0; i<8; i++) // pour chaque bande
    {
        text = new QLabel;
        text->setText(QString::number(f) + "Hz");
        f*=2;
        text->setFrameShape(QFrame::WinPanel);
        text->setFrameShadow(QFrame::Raised);
        ui->layout->addWidget(text,0,i+1, Qt::AlignCenter);
    }
}

Data::~Data()
{
    delete ui;
}

void Data::addValue(QString val, int row, int col)
{
    text = new QLabel;
    text->setText(val);
    ui->layout->addWidget(text,row,col, Qt::AlignCenter);
}
