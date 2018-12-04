/*/////////////////////////////// INFORMATIONS ///////////////////////////////////////
Software name : Just4RIR
Creation date : November 30th 2018
Last modification : November 30th 2018
Author : Robin Gueguen
License : GPL 3.0 2018
Property : Institut des Sciences du Calcul et des Données - Sorbonne Université
Version : 1.0 beta
Function : Divide and conquer method for acceleration process
*/////////////////////////////////////////////////////////////////////////////////////

#include "octree.h"
#include "QDebug"
#include <math.h>
#include <QMessageBox>

// classes
Octree::Octree()
{
}


Octree::Octree(MeshObj monMesh, int nbFaceFeuille)
{
    // Declaration
    m_seuil = nbFaceFeuille;
    int k(0), i(0), nbBoiteNew(0), nbBoiteOld(0);

    // création du cube racine
    std::vector<CoordVector> vertex = monMesh.getVert();
    CoordVector Min(monMesh.getMin()-0.1), Max(monMesh.getMax()+0.1);


    //I- Création de la boite root (float)
    Boite boiteRacine;
    boiteRacine.m_arrete = coordMax(Max - Min);
    boiteRacine.m_indicePere = -1;
    boiteRacine.m_indiceBoite = 0;
    boiteRacine.m_min = Min;
    boiteRacine.m_max = Max;
    boiteRacine.m_coinMin = (Min+Max)/2 - coordMax(Max - Min)/2;

    for (k = 0 ; k < vertex.size() ; k+=3)
    {
        boiteRacine.m_numElt.push_back(k);
    }

    // II- Stockage de la boite racine
    m_vectBoite.push_back(boiteRacine);

    // III- Tant qu'il existe au moins une boite >seuil on continue de descendre
    bool DessusSeuil = true;
    while (DessusSeuil)
    {
        nbBoiteNew = m_vectBoite.size() - nbBoiteOld;
        DessusSeuil = false;

        if (nbBoiteNew < vertex.size()/3) // on arrete la boucle si on a autant de nouvelle boite que de face (soit une face par boite)
        {
            nbBoiteOld = m_vectBoite.size();

            // pour chaque boite du nouvel étage
            for(i = nbBoiteOld - nbBoiteNew ; i <nbBoiteOld ; i++ ) {
                // Si au moins une boite posséde plus de m_seuil elt
                if(m_vectBoite[i].m_numElt.size() > m_seuil) DessusSeuil = true;
            }
            if(DessusSeuil) {
                for(i = nbBoiteOld - nbBoiteNew ; i <nbBoiteOld ; i++ ) {
                    // IV- Si la boite n'est pas une feuille : Découpage de la boite courante en huit
                    etagesuivant(vertex, i);
                }
            }
        }
    }

    // Mise à jour taille des boites
    for (i = 0 ; i< m_vectBoite.size() ; i++) // pour chaque boite qui n'est pas une feuille vide
    {
        if (!m_vectBoite[i].estUneFeuille || !m_vectBoite[i].m_numElt.empty())
        {
            m_vectBoite[i].m_arrete = coordMax(m_vectBoite[i].m_max - m_vectBoite[i].m_min) + 0.00002;
            m_vectBoite[i].m_coinMin = m_vectBoite[i].m_min - 0.00001; // Pour eliminer les problèmes d'arrondi
        }
    }

    // Vérification
    int nbEltFeuille(0);
    for (i = 0 ; i< m_vectBoite.size() ; i++)
    {
        if (m_vectBoite[i].estUneFeuille)
        {
            nbEltFeuille+= m_vectBoite[i].m_numElt.size();
        }
        else if (!m_vectBoite[i].m_numElt.empty())
            qDebug() << "non feuille non vide n°: " << i;
    }
    if(vertex.size()/3 != nbEltFeuille)
        QMessageBox::critical(NULL,"Erreur","Mauvaise mise en boite des faces");
}

Octree::~Octree()
{
}

Octree Octree::operator=(const Octree &oct)
{
    m_vectBoite.resize(oct.getVectBoite().size(),Boite());
    m_vectBoite = oct.getVectBoite();
    m_seuil = oct.getSeuil();
    return *this;
}


const std::vector<Boite> &Octree::getVectBoite() const{
    return m_vectBoite;
}

int Octree::getSeuil() const{
    return m_seuil;
}

Boite::Boite()
{
    estUneFeuille = true;
}

Boite::~Boite()
{
}

Boite Boite::operator=(const Boite &boite)
{
    m_min         = boite.m_min;
    m_max         = boite.m_max;
    m_coinMin     = boite.m_coinMin;
    m_arrete      = boite.m_arrete;
    m_indicePere  = boite.m_indicePere;
    m_indiceBoite = boite.m_indiceBoite;
    m_numElt      = boite.m_numElt;
    m_numRayon    = boite.m_numRayon;
    estUneFeuille = boite.estUneFeuille;

    return *this;
}

int Boite::getNbElt() {
    return m_numElt.size();
}

std::vector<int> &Boite::getVectNumElt() {
    return m_numElt;
}

void Boite::chargerElt(std::vector<CoordVector> const& vert, int indice)
{
    // on garde l'indice
    m_numElt.push_back(indice);

    // Recupération du min et max
    for (int i = 0; i<3 ; i++)
    {
        for (int j = 0 ; j<3 ; j++)
        {
            if(vert[indice+i][j] < m_min[j]) m_min[j] = vert[indice+i][j];
            if(vert[indice+i][j] > m_max[j]) m_max[j] = vert[indice+i][j];
        }
    }
}

void Boite::chargerRay(int indice) {
    m_numRayon.push_back(indice);
}

void Boite::supprElt(int position) {
    m_numElt.erase(m_numElt.begin()+position);
}

void Boite::supprRay(int position) {
    m_numRayon.erase(m_numRayon.begin()+position);
}


void Octree::etagesuivant(std::vector<CoordVector> const& vert, int indiceBoite)
{ 

    // Si la boite possede au moins 1 elt
    if(!m_vectBoite[indiceBoite].m_numElt.empty())
    {
        m_vectBoite[indiceBoite].estUneFeuille = false; // Alors on la decoupe et ce n'est plus une feuille

        int i, k, ind;
        std::vector<int> elt;
        bool premierElt;

        // V- Création des 8 boites filles à partir de la boite actuelle
        std::vector<Boite> boitesFilles;
        decoupage(m_vectBoite[indiceBoite], boitesFilles);

        // VI- Pour chaque nouvelle boite :
        for (i = 0 ; i<8 ; i++)
        {
            elt = m_vectBoite[indiceBoite].getVectNumElt();
            premierElt = true;

            // Pour chaque face compris dans la boite père
            for (k = elt.size()-1; k >=0  ; k--)
            {
                ind = elt[k];

                if (appartientBoite(boitesFilles[i], vert, ind))
                {
                    if(premierElt)
                    {
                        boitesFilles[i].m_min = vert[ind];
                        boitesFilles[i].m_max = vert[ind];
                        premierElt = false;

                    }
                    boitesFilles[i].chargerElt(vert, ind); // Si le centre de la face est inclue dans la boite fille en test on stock son indice
                    m_vectBoite[indiceBoite].supprElt(k);  // et on le retire de la boite pere (ancienne methode)
                }
            }
            boitesFilles[i].m_indiceBoite = m_vectBoite.size();
            m_vectBoite.push_back(boitesFilles[i]); // Ajout de la boite fille            
        }
    }
}

void decoupage(Boite &boitePere, std::vector<Boite>& boitesFilles)
{

    // Initialisation du vecteur de boites filles
    boitesFilles.resize(8, Boite());

    // Recuperation des données du père avec leger offset pour eviter les pb d'arrondi
    CoordVector coinMin = boitePere.m_coinMin - 0.001;
    float arrete = boitePere.m_arrete/2 + 0.002;

    boitesFilles[0].m_coinMin = CoordVector(coinMin.x           , coinMin.y          , coinMin.z         );
    boitesFilles[1].m_coinMin = CoordVector(coinMin.x + arrete  , coinMin.y          , coinMin.z         );
    boitesFilles[2].m_coinMin = CoordVector(coinMin.x           , coinMin.y + arrete , coinMin.z         );
    boitesFilles[3].m_coinMin = CoordVector(coinMin.x + arrete  , coinMin.y + arrete , coinMin.z         );
    boitesFilles[4].m_coinMin = CoordVector(coinMin.x           , coinMin.y          , coinMin.z + arrete);
    boitesFilles[5].m_coinMin = CoordVector(coinMin.x + arrete  , coinMin.y          , coinMin.z + arrete);
    boitesFilles[6].m_coinMin = CoordVector(coinMin.x           , coinMin.y + arrete , coinMin.z + arrete);
    boitesFilles[7].m_coinMin = CoordVector(coinMin.x + arrete  , coinMin.y + arrete , coinMin.z + arrete);

    for (int i = 0 ; i< 8 ; i++)
    {
        boitesFilles[i].m_arrete     = arrete;
        boitesFilles[i].m_indicePere = boitePere.m_indiceBoite;
    }
}

bool appartientBoite(Boite &boite, std::vector<CoordVector> const& vert, int indice)
{
    CoordVector centreFace;
    for (int i = 0 ; i < 3 ; i++)
    {
        centreFace+= vert[indice+i];
    }

    if (centreFace.x/3 < boite.m_coinMin.x)                   return false;
    if (centreFace.x/3 > boite.m_coinMin.x + boite.m_arrete)  return false;
    if (centreFace.y/3 < boite.m_coinMin.y)                   return false;
    if (centreFace.y/3 > boite.m_coinMin.y + boite.m_arrete)  return false;
    if (centreFace.z/3 < boite.m_coinMin.z)                   return false;
    if (centreFace.z/3 > boite.m_coinMin.z + boite.m_arrete)  return false;

    return true;
}


void Octree::chargerRayonRacine(int nbRay)
{
    int j;
    m_vectBoite[0].m_numRayon.clear();
    // Chargement de la boite racine avec tous les indices rayons
    for (j = 0; j<nbRay ; j++)
    {
        m_vectBoite[0].m_numRayon.push_back(j);
    }
}

void Octree::chargerRayon(std::vector<CoordVector> const& orig, std::vector<CoordVector> const& dir, std::vector<bool> const& RayVivant)
{
    int i, j, ind, numPere;

    // Pour chaque boite : chargement de l'indice des rayons qui intersectent avec elle
    for (i = 1 ; i < m_vectBoite.size() ; i++)
    {
        // On ne s'occupe que des feuilles avec des elts ou des non-feuilles
        if (!m_vectBoite[i].m_numElt.empty() || !m_vectBoite[i].estUneFeuille)
        {
            // On supprime les indices des rayons précedemment enregistrés
            m_vectBoite[i].m_numRayon.clear();

            numPere = m_vectBoite[i].m_indicePere;

            for (j = 0 ; j < m_vectBoite[numPere].m_numRayon.size() ; j++) // Pour tous les rayons contenus dans la boite père
            {
                ind = m_vectBoite[numPere].m_numRayon[j];
                // Si le rayon est toujours vivant
                if (RayVivant[ind])
                {
                    // test intersection entre rayon ind et boite i
                    if (intersecBoiteRay(m_vectBoite[i], orig[ind], inverse(dir[ind])))
                    {
                        m_vectBoite[i].m_numRayon.push_back(ind); // Ajout des rayons à la boite courante
                    }
                }
            }
        }
    }
}


bool intersecBoiteRay(const Boite &boite, const CoordVector &orig, const CoordVector &invDir)
{
    // Ref : http://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection

    float tmin, tmax, tymin, tymax;

    if (invDir.x >=0) {
        tmin = (boite.m_min.x - orig.x) * invDir.x;
        tmax = (boite.m_max.x - orig.x) * invDir.x;
    }
    else {
        tmax = (boite.m_min.x - orig.x) * invDir.x;
        tmin = (boite.m_max.x - orig.x) * invDir.x;
    }

    if (invDir.y >=0) {
        tymin = (boite.m_min.y - orig.y) * invDir.y;
        tymax = (boite.m_max.y - orig.y) * invDir.y;
    }
    else {
        tymax = (boite.m_min.y - orig.y) * invDir.y;
        tymin = (boite.m_max.y - orig.y) * invDir.y;
    }

   if ((tmin > tymax) || (tymin > tmax)) return false;

   if (tymin > tmin) tmin = tymin;
   if (tymax < tmax) tmax = tymax;

   float tzmin, tzmax;

   if(invDir.z >= 0) {
       tzmin = (boite.m_min.z - orig.z) * invDir.z;
       tzmax = (boite.m_max.z - orig.z) * invDir.z;
   }
   else {
       tzmax = (boite.m_min.z - orig.z) * invDir.z;
       tzmin = (boite.m_max.z - orig.z) * invDir.z;
   }

   if ((tmin > tzmax) || (tzmin > tmax)) return false;

   return true;

}

