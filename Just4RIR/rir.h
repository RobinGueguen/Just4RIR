/*/////////////////////////////// INFORMATIONS ///////////////////////////////////////
Software name : Just4RIR
Creation date : November 30th 2018
Last modification : November 30th 2018
Author : Robin Gueguen
License : GPL 3.0 2018
Property : Institut des Sciences du Calcul et des Données - Sorbonne Université
Version : 1.0 beta
Function : Creation of image-sources
*/////////////////////////////////////////////////////////////////////////////////////

#ifndef RIR_H
#define RIR_H

#include "fonction.h"
#include "objreader.h"
#include "raytracing.h"

// les méthodes
std::vector<float> toucheListener(Ray &rayon, Listener &listener);
void partitionnage(std::vector<std::vector<float> > &fir, std::vector<std::vector<float> > &firPart, int taille);
void unite(std::vector< CoordVector> &si_in, std::vector< CoordVector> &si_out, std::vector<float> &nrg_in, std::vector<float> &nrg_out, float distance);


// Les classes

class SourceImage
{
    public:
        SourceImage();
        ~SourceImage();
        std::vector<CoordVector> &getSourcesImages();
        std::vector<float> &getNrgSI();
        std::vector<float> &getSI_Tps();
        std::vector<float> &getX();
        std::vector<std::vector<double> > &getY();
        std::vector<std::vector<double> > &getCurve();
        bool addSourcesImages(Ray &rayon, Listener &listener, float longueurMax, const std::vector<float> &absAir, float seuil);
        bool calculerRIR(int f_ech, std::vector<float> &absR, float gain, bool curve, float seuil);
        std::vector< std::vector<float> >&getFIR();
        std::vector< std::vector<float> >&getFirPart();
        std::vector<CoordVector> &getRaySI();
        std::vector<CoordVector> &getRaySIvec();
        std::vector<float> &getRaySIlong();
        std::vector<float> &getRaySITps();

    private:
        std::vector<CoordVector> m_sourcesImages;
        std::vector<float> m_nrgSI;
        std::vector<int> m_nbSI;
        std::vector<float> m_sourcesImages_Filtrees;
        std::vector<float> m_nrgSI_Filtrees;
        std::vector<float> m_x;
        std::vector<std::vector<double> > m_y;
        std::vector<std::vector<double> > m_curve;
        std::vector<float> m_sourcesImages_Tps;
        float m_xMax;
        std::vector< std::vector<float> > m_FIR;
        std::vector< std::vector<float> > m_firPart;
        std::vector<CoordVector> m_raySI;
        std::vector<CoordVector> m_raySIvec;
        std::vector<float> m_raySIlong;
        std::vector<float> m_raySITps;
        int m_nbIteration;
};

#endif // RIR_H
