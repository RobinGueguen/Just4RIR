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

#ifndef DATA_H
#define DATA_H

#include <QDialog>
#include <QLabel>
#include <QGridLayout>

namespace Ui {
class Data;
}

class Data : public QDialog
{
    Q_OBJECT

public:
    explicit Data(QWidget *parent = 0);
    ~Data();
    void addValue(QString val, int row, int col);



private:
    Ui::Data *ui;
    QLabel *text;
    //QGridLayout *layout;
};

#endif // DATA_H
