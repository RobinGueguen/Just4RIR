/*/////////////////////////////// INFORMATIONS ///////////////////////////////////////
Software name : Just4RIR
Creation date : November 30th 2018
Last modification : November 30th 2018
Author : Robin Gueguen
License : GPL 3.0 2018
Property : Institut des Sciences du Calcul et des Données - Sorbonne Université
Version : 1.0 beta
Function : Analytic test case of a shoes box
*/////////////////////////////////////////////////////////////////////////////////////

#include "analytique.h"
#include <math.h>
#include "QDebug"
#include <QMessageBox>

void cubeAnalytique(int temp, int hum, Source source, std::vector<Listener> listener, int nbRebond, std::vector<SourceImage> sourceImage, int numListener)
{

    /// RIR de cube analytique

    // Dimension du cube
    CoordVector L(4,4,4); //longueur, largeur, hauteur

    std::vector<float> absR = absair(temp, hum);//absorptionAir(20);
    float f[] = {60,50,40,30,20,10};//x, -x, y, -y, z, -z en % d'attenaution
    std::vector<float> att(f, f+sizeof(f)/sizeof(float));

    // Position src et listener
    CoordVector src = source.getCentre();
    CoordVector mic = listener[numListener].getCentre();

    debugStdVect(att);
    src.debug();

    // Positions relatives et énergie des sources images
    std::vector<CoordVector> src_im;
    std::vector<float> src_im_tps, nrgbis;

    float d, attenuation, i0, i1, i2, i3, i4, i5;
    float i,j, k, l;
    float ordre = nbRebond;
    float ordrebuf;
    CoordVector si;

    //correspondance
    att[0]=1-(f[1]/100);
    att[1]=1-(f[0]/100);
    att[2]=1-(f[5]/100);
    att[3]=1-(f[4]/100);
    att[4]=1-(f[2]/100);
    att[5]=1-(f[3]/100);
    debugStdVect(att);

    bool sitrouvee = false;
    for (i = -ordre ; i <=ordre ; i++)
    {
        si.x = i*L.x + src.x*pow(-1,i);
        i0 = abs(0.5*i - 0.25 + 0.25*pow(-1, i));
        i1 = abs(0.5*i + 0.25 - 0.25*pow(-1, i));
        for (j = -(ordre-abs(i)) ; j<= ordre-abs(i); j++)
        {
            if (ordre-abs(i)-abs(j)< 0) ordrebuf = 0;
            else ordrebuf = ordre-abs(i)-abs(j);
            si.y = j*L.y + src.y*pow(-1,j);
            i2 = abs(0.5*j - 0.25 + 0.25*pow(-1, j));
            i3 = abs(0.5*j + 0.25 - 0.25*pow(-1, j));
            for (k = -ordrebuf ; k<= ordrebuf; k++)
            {
                si.z = k*L.z + src.z*pow(-1,k);
                i4 = abs(0.5*k - 0.25 + 0.25*pow(-1, k));
                i5 = abs(0.5*k + 0.25 - 0.25*pow(-1, k));


                for (l=0 ; l< src_im.size() ; l++)
                {
                    if (proche(si,src_im[l])) {sitrouvee = true; qDebug() <<"source trouvée !";}
                }
                if(!sitrouvee)
                    {
                        attenuation =    pow(att[0], i0)
                                        *pow(att[1], i1)
                                        *pow(att[2], i2)
                                        *pow(att[3], i3)
                                        *pow(att[4], i4)
                                        *pow(att[5], i5);

                        if(attenuation > 0 && attenuation <=1)
                        {
                            src_im.push_back(si); // ajout position
                            src_im_tps.push_back(1000*d/VITESSE_SON); // distance temporelle ms

                            d = norme(vecteur(si,mic)); // distance

                            if (d > 1e-6) // pour eviter les divisions par 0 (cas source et mic confondus)
                            {
                                for (l = 0; l< 8 ; l++){
                                    nrgbis.push_back(attenuation/pow(d,2)*exp(-absR[l]*d));
                                }
                            }
                            else {
                                QMessageBox::warning(NULL,"Attention","Source et micro confondus");
                            }
                        }
                    }
                else sitrouvee=false;
            }
        }
    }


    std::vector<float> x;
    std::vector<std::vector<double> > y;
    y.resize(8);
    std::vector<float> nrgExp = sourceImage[numListener].getNrgSI();
    std::vector<CoordVector> src_im_exp = sourceImage[numListener].getSourcesImages();

    if(src_im_exp.size() < 1)
    {
        QMessageBox::warning(NULL,"Attention","Pas de source image expérimentale");
        return;
    }

    float max0 = *std::max_element(nrgExp.begin(), nrgExp.end());
    float max1 = *std::max_element(nrgbis.begin(), nrgbis.end());

    if(src_im.size()<1 || max1 < 0.000001)
    {
        QMessageBox::warning(NULL,"Attention","Pas de sources images théoriques");
        return;
    }

    for(auto&a : nrgbis) a/=(max1/max0);
    max1 = *std::max_element(nrgbis.begin(), nrgbis.end());

    for (j=0; j< src_im_exp.size(); j++)
    {
        for (i=0; i< src_im.size(); i++)
        {
            if (proche(src_im_exp[j],src_im[i]) && nrgExp[8*j] > 100)
            {
                for (l=0 ; l < 8 ; l++)
                {
                    y[l].push_back((double)(nrgbis[8*i+l]-nrgExp[8*j+l])/nrgbis[8*i+l]);
                }
                if(proche(src_im_exp[j],src))
                {qDebug() << "source" << "i = " << i << " ; j = " << j;}
            }
        }
    }
}
