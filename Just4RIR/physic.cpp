/*/////////////////////////////// INFORMATIONS ///////////////////////////////////////
Software name : Just4RIR
Creation date : November 30th 2018
Last modification : November 30th 2018
Author : Robin Gueguen
License : GPL 3.0 2018
Property : Institut des Sciences du Calcul et des Données - Sorbonne Université
Version : 1.0 beta
Function : Absorption from materials and atmosphere
*/////////////////////////////////////////////////////////////////////////////////////

#include "physic.h"
#include "QFile"
#include "QIODevice"
#include "QTextStream"
#include "QCoreApplication"
#include "math.h"
#include <QMessageBox>
#include <QDebug>
#include "plotwindow.h"


Material::Material()
{
    QFile fichier(QCoreApplication::applicationDirPath() + "/Material.txt"); //fichier modifiable
    if (!fichier.exists()) fichier.setFileName(":/mat/Material.Li8"); // fichier de materiaux odeon compilé

    if(fichier.open(QIODevice::ReadOnly | QIODevice::Text)) // Si on peut ouvrir le fichier
    {
        QTextStream flux(&fichier);
        while(!flux.atEnd())
        {
            QString ligne = flux.readLine(); // Lecture ligne par ligne du fichier
            QStringList donnee = ligne.split("\t");

            if (donnee[0] == "") // Si la ligne commence par "" et qu'elle comporte plusieurs données
            {
                if (donnee.size() > 1)
                {
                    //remplissage des coeff d'absorption
                    for (int i = 1; i<donnee.size() ; i++)
                    {
                        m_indMat.push_back(donnee[i].toFloat());
                    }
                }
            }
            else // Sinon on est sur la ligne du nom du materiau
            {
                // replissage du numero de materiaux
                m_nomMat.push_back(donnee[0]);
            }
        }
    }
    else
    {
        QMessageBox::critical(NULL,"Error","File Material.Li8 not found. \nThe default absoption value for all faces is 0.5");
        m_nomMat.push_back("materiauNonReference");
        for (int i = 0 ; i<64 ; i++) // la ligne du fichier Odéon avec des coefficient à 50% est la 7-eme, on inscrit donc les (7+1)*8 permiers terme du tableau
        {
            m_indMat.push_back(0.5);
        }
    }
}

Material::~Material()
{
}

// Accesseurs
float Material::getIndMat(int num)
{
    return m_indMat[num];
}

std::vector<QString> &Material::getNomMat()
{
    return m_nomMat;
}


//Ref : http://www.sengpielaudio.com/calculator-air.htm
// view-source:http://resource.npl.co.uk/acoustics/techguides/absorption/
std::vector<float> absair(int temp, int relh) {

    std::vector<float> freq, resultat;
    int i;

    freq.resize(8,0);
    resultat.resize(8,0);
    freq[0] = 62.5;
    for (i = 0 ; i<7 ; i++)
    {
        freq[i+1] = freq[i]*2;
    }

    float pres=101325;
    pres/=101325; // convert to relative pressure

    temp+=273.15; // convert to kelvin

    float C_humid = 4.6151 - 6.8346*pow((273.15/temp),1.261);
    float tempr   = temp/293.15;// convert to relative air temp (re 20 deg C)


    float hum     = relh*pow(10,C_humid)*pres;
    float frO     = pres*(24+4.04e4*hum*(0.02+hum)/(0.391+hum));
    float frN     = pres*pow(tempr,-0.5)*(9+280*hum*exp(-4.17*(pow(tempr,-1/3)-1)));

    // boucle sur la frequence
    for (i = 0 ; i<8 ; i++)
    {
        resultat[i] = 8.686*freq[i]*freq[i]*(1.84e-11*(1/pres)*sqrt(tempr)+pow(tempr,-2.5)*(0.01275*(exp(-2239.1/temp)/(frO+freq[i]*freq[i]/frO))+0.1068*(exp(-3352/temp)/(frN+freq[i]*freq[i]/frN))));
        resultat[i] = pow(10, resultat[i]/10)-1; //on passe de dB/m au facteur de puissance perdu/m
    }

    return resultat;
}
