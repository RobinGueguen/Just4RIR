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

#include "rir.h"
#include "math.h"
#include <QMessageBox>
#include "QDebug"
#include "reglin.h"

// les méthodes

std::vector<float> toucheListener(Ray &rayon, Listener &listener)
{
    std::vector<float> resultat, ray_long;
    ray_long = rayon.getLong();

    std::vector<CoordVector> posOld, dir;
    posOld = rayon.getPos();
    dir = rayon.getDir();

    std::vector<bool> vivant = rayon.getRayVivant();

    float alpha, normeAL;

    CoordVector L(listener.getCentre()); // Point au centre du Listener
    float r(listener.getRayon()); // Rayon du Listener

    CoordVector AL;

    for(int i = 0; i<posOld.size() ; i++)
    {
        if (vivant[i]) // on ne prend pas les rayons morts
        {
            //AB = vecteur(posOld[i], posNew[i]);
           AL = vecteur(posOld[i], L);
           normeAL = norme(AL);

           // test si le point A n'est pas dans le listener
           if (normeAL > r)
               {
               alpha = angle(dir[i],AL);
               // test sur la direction
               if (cos(alpha) >= 0) // on peut mettre alpha tout court car acos (dans la fonction angle) renvoi la partie positive
               {
                   // test sur la distance
                   if (ray_long[i] >= normeAL)
                   {
                       if (alpha <= asin(r/normeAL)) resultat.push_back(normeAL);

                       else resultat.push_back(-1e-6);
                   }   else resultat.push_back(-1e-6);
               }       else resultat.push_back(-1e-6);
            }          else resultat.push_back(normeAL);
        }              else {resultat.push_back(-1e-6);}
    }
    return resultat;
}

// La classe

SourceImage::SourceImage()
{
    m_xMax = 0;
}

SourceImage::~SourceImage()
{
}

std::vector<CoordVector> &SourceImage::getSourcesImages()
{
    return m_sourcesImages;
}

std::vector<float> &SourceImage::getSI_Tps()
{
    return m_sourcesImages_Tps;
}

std::vector<float> &SourceImage::getNrgSI()
{
    return m_nrgSI;
}

std::vector<float> &SourceImage::getX()
{
    return m_x;
}
std::vector<std::vector<double> > &SourceImage::getY()
{
    return m_y;
}

std::vector<std::vector<double> > &SourceImage::getCurve()
{
    return m_curve;
}

std::vector< std::vector<float> >&SourceImage::getFIR(){
    return m_FIR;
}

std::vector< std::vector<float> >&SourceImage::getFirPart(){
    return m_firPart;
}

std::vector<CoordVector> &SourceImage::getRaySI(){
    return m_raySI;
}
std::vector<CoordVector> &SourceImage::getRaySIvec(){
    return m_raySIvec;
}
std::vector<float> &SourceImage::getRaySIlong(){
    return m_raySIlong;
}
std::vector<float> &SourceImage::getRaySITps(){
    return m_raySITps;
}

bool SourceImage::addSourcesImages(Ray &rayon, Listener &listener, float longueurMax, const std::vector<float>& absAir, float seuil)
{
    std::vector<float> touche = toucheListener(rayon,listener);
    CoordVector C; // la source image

    std::vector<float> longueurRayonTot = rayon.getDist(); // Distance parcourue avant le dernier rebond
    std::vector<CoordVector> point = rayon.getPos();
    std::vector<CoordVector> point2 = rayon.getRay();
    std::vector<float> longueurRayonLast = rayon.getLong();
    std::vector<CoordVector> vec = rayon.getDir();
    std::vector<float> nrg = rayon.getNRGbackup();
    std::vector<bool> vivant = rayon.getRayVivant();
    CoordVector A, vect;
    float temps;

    int i, k, j;
    bool SI_trouvee = false;
    int tailleSi = m_sourcesImages.size();


    if(!m_nrgSI.empty() && seuil >0) seuil*= *std::max_element(m_nrgSI.begin(), m_nrgSI.end()); // Normalisation du seuil

    for (i = 0 ; i< touche.size() ; i++) // rayon par rayon
    {
        if ((longueurRayonTot[i]+touche[i])>longueurMax && vivant[i]) {rayon.killRay(i);}
        else
        {
            if (touche[i]>=0) // si le rayon touche le listener
            {
                A = point[i];
                vect = vec[i];

               C = A - vect*longueurRayonTot[i];

               longueurRayonTot[i]+=touche[i]; // ajout de la distance au listener à la longueur totale

               /// Nouvelle méthode
               for (j= m_sourcesImages.size()-1 ; j>=0; j--) // balayage des source images deja existante depuis la fin
               {
                   if (proche(C, m_sourcesImages[j],0.0001)) // si la nouvelle SI est proche d'une ancienne
                   {
                       for (k = 0 ; k<8 ; k ++) // On ajoute les énérgies
                       {
                           m_nrgSI[8*j+k]+=(nrg[8*i+k] * exp(-absAir[k]*longueurRayonTot[i]));
                       }
                       SI_trouvee = true; // on a trouvé une SI en doublons
                       break; // et on sort de la boucle
                   }
               }
               if (SI_trouvee) SI_trouvee = false; // Si on a trouvé une SI on n'a rien à faire
               else /// Ancienne méthode
               {
                    // On ajoute les coordonnées au vecteur sources images
                    m_sourcesImages.push_back(C);

                    // Pour chaque nouvelle source image on enregistre les energies des 8 bandes
                    for (k = 0 ; k<8 ; k ++)
                    {
                        m_nrgSI.push_back(nrg[8*i+k] * exp(-absAir[k]*longueurRayonTot[i]));
                    }

                    temps = 1000 * longueurRayonTot[i] / VITESSE_SON; // en ms
                    // On créé le vecteur des valeurs en temps
                    m_sourcesImages_Tps.push_back(temps);

                    //Recupération trajectoire rayon
                    m_raySI.push_back(point2[i]);
                    m_raySIvec.push_back(-vec[i]);
                    m_raySIlong.push_back(longueurRayonTot[i]-touche[i]+longueurRayonLast[i]);
                    m_raySITps.push_back(temps);
                }
            }
        }
    }

    // suppression des si inférieurs au seuil
    if (longueurMax<1e6) // dans le cas ou on n'est pas en mode rebond fixe.
    {
        bool b;
        for(i= m_sourcesImages.size()-1; i>=0 ; i--)
        {
            b = true;
            for(k=0 ; k<8 ; k++) if(m_nrgSI[8*i+k] > seuil) b =false; // s'il y a une energie au dessus du seuil
            if(b)
            {
                m_sourcesImages.erase(m_sourcesImages.begin()+i);
                m_sourcesImages_Tps.erase(m_sourcesImages_Tps.begin()+i);
                for(k=7 ; k>=0 ; k--) m_nrgSI.erase(m_nrgSI.begin()+8*i+k);
                m_raySI.erase(m_raySI.begin()+i);
                m_raySIvec.erase(m_raySIvec.begin()+i);
                m_raySIlong.erase(m_raySIlong.begin()+i);
                m_raySITps.erase(m_raySITps.begin()+i);
            }
        }
    }

    if (m_sourcesImages.size()>tailleSi) m_nbIteration = 0;
    else m_nbIteration++; // s'il n'y a pas de nouvelle source images
    if(rayon.getRayMorts()<rayon.getNbRay() && m_nbIteration<10) return true;// on arrete la boucle en fonction de la longueur des rayons ou si on a fait 10 itération sans sources images
    else return false;
}



bool SourceImage::calculerRIR(int f_ech, std::vector<float> &absR, float gain, bool curve, float seuil)
{
    float freq = (float)f_ech/1000; // car on a des temps en ms (convertion en float)

    // On garde le temps max
    m_xMax = *std::max_element(m_sourcesImages_Tps.begin(), m_sourcesImages_Tps.end());

    float nb_ech = ceil(m_xMax*freq);

    double max(0), maxbuf(0);
    if (nb_ech > 0)
    {
        m_x.clear();
        m_y.clear();
        m_x.resize(nb_ech, 0);
        m_y.resize(8);
        m_FIR.clear();

       // Abscisses
        for (float i = 0 ; i <nb_ech ; i++)
       {
           m_x[i] = i/freq; // valeurs en ms
       }

        // Ordonnées
        m_FIR.resize(7);


        for (int k = 0 ; k < 8 ; k++) // pour chaque bande
        {
            m_y[k].resize(nb_ech);
            for (int i=0 ; i< m_sourcesImages_Tps.size(); i++) // pour chaque source image
            {
                m_y[k][floor(m_sourcesImages_Tps[i]*freq)] += (double)m_nrgSI[i*8 + k];
            }
            maxbuf = *std::max_element(m_y[k].begin(), m_y[k].end());
            if(max<maxbuf) max = maxbuf; // recuperation du max
        }

        // mise à l'échelle du son direct
        float sondirect = *std::min_element(m_sourcesImages_Tps.begin(), m_sourcesImages_Tps.end())*VITESSE_SON/1000; // en m

        m_curve.clear();
        m_curve.resize(m_y.size());

        double aa, bb;
        std::vector<double> y_buf;
        std::vector<double> x_buf;
        int size_max(0);

        for (int k = 0 ; k < 8 ; k++) // pour chaque bande
        {
            y_buf.clear();
            x_buf.clear();
            for(int i=0; i<m_y[k].size() ; i++)
            {
                m_y[k][i]/=max;// normalisation

                if (sondirect>0 && gain >=0) // -31dB à 10m
                {
                    m_y[k][i]*= pow(10,(gain-31)/10)*100*exp(-absR[k]*sondirect)/pow(sondirect,2); // on retire l'offset du son direct (ok si le premier son n'a pas touché de paroi)
                }

                if (k>0) m_FIR[k-1].push_back(sqrt(m_y[k][i])); // passage en puissance
            }


            // creation des decay curve
            m_curve[k] = m_y[k];
            for (int i = m_curve[k].size()-2; i >= 0 ; i--)
            {
                m_curve[k][i] += m_curve[k][i+1];
            }


            ///// REGRESSION LINEAIRE
            for(int i=0; i<m_curve[k].size() ; i++)
            {
                if(m_curve[k][i]>0)
                {
                    y_buf.push_back(10*log10(m_curve[k][i]));// passage en log
                    x_buf.push_back(i);
                }
            }

            aa=pente(x_buf, y_buf, y_buf.size());
            bb=ordonnee(x_buf, y_buf, y_buf.size());

            double new_y(0), j(nb_ech);

            maxbuf = *std::max_element(m_y[0].begin(), m_y[0].end());
            while(new_y>10*(log10(seuil)+log10(maxbuf)))
            {
                new_y = aa*j + bb;
                m_curve[k].push_back(pow(10,new_y/10));
                j++;
            }
            if (m_curve[k].size() > size_max) size_max = m_curve[k].size();

        }

        if (curve)
        {
            for (float i = m_x.size() ; i<size_max ; i++)
            {
                 m_x.push_back(i/freq);
            }
        }

        for (int k = 0 ; k < 8 ; k++) // pour chaque bande
        {
            for (int i = m_y[k].size() ; i<size_max ; i++)
            {
                 m_y[k].push_back(0);
            }
            for (int i = m_curve[k].size() ; i<size_max ; i++)
            {
                 m_curve[k].push_back(0);
            }
            if (curve) m_y.push_back(m_curve[k]);
        }

        return true;
    }
    else return false;
}

void partitionnage(std::vector< std::vector<float> > &fir, std::vector< std::vector<float> > &firPart, int taille)
{

    int j, k;
    int i(0), l(0);
    int n = taille/2;
    int firsize = fir[0].size();

    int nPart = ceil((float)firsize/(float)n)*fir.size();

    firPart.resize(nPart);

    for (k = 0 ; k < nPart ; k++) // pour chaque fir de taille n
    {
        firPart[k].resize(2*n, 0); // On met des zero partout

        for (j = n ; j <2*n ; j++) // on remplit la deuxième partie de chaque firPart
        {
            if (i == firsize) // si on arrive à la fin de la fir
            {
                i=0; // on remet à 0 le comtpeur
                l++; // on passe à la fir suivante
                j = 2*n; // on change de firPart (met fin à la boucle)
            }
            else
            {
                firPart[k][j] = fir[l][i]; // on range les valeurs
                i++; // puis on décale
            }
        }
    }
}

void unite(std::vector< CoordVector> &si_in, std::vector< CoordVector> &si_out, std::vector<float> &nrg_in, std::vector<float> &nrg_out, float distance)
{
    bool srcCommune = false;
    int i, j, k;

    for (i = 0; i< si_in.size() ; i++)
    {
        for (k = 0; k< si_out.size() ; k++)
        {
            if (proche(si_in[i],si_out[k], distance))
            {
                srcCommune = true;
                for (j=0 ; j<8 ; j++)
                {
                    nrg_out[8*k+j] += nrg_in[8*i+j];
                }
            }
        }
        if (!srcCommune)
        {
            si_out.push_back(si_in[i]);
            for (j=0 ; j<8 ; j++)
            {
                nrg_out.push_back(nrg_in[8*i+j]);
            }
        }
        else srcCommune = false;
    }
}
