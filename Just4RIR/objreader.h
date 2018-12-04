/*/////////////////////////////// INFORMATIONS ///////////////////////////////////////
Software name : Just4RIR
Creation date : November 30th 2018
Last modification : November 30th 2018
Author : Robin Gueguen
License : GPL 3.0 2018
Property : Institut des Sciences du Calcul et des Données - Sorbonne Université
Version : 1.0 beta
Function : Read a .OBJ file and classify the mesh, the sources and the listeners
*/////////////////////////////////////////////////////////////////////////////////////

#ifndef OBJREADER_H
#define OBJREADER_H

#include "physic.h"

// Les methodes
QString doubleSlash(QString s); // fonction qui remplace // par /1/
QString supprimeSlash(QString s); // fonction qui supprime les slashs d'un string

// Les classes

// Classe Source : vecteur XYZ des coordonnees du point d'emission
class Source
{
    public:
        Source();// Constructeur
        ~Source();// Destructeur
        void chargerSource();
        void chargerVert(float coord);
        QString afficher() const;
        //accesseurs
        CoordVector getCentre();
        CoordVector getCentre(unsigned int n);
        std::vector<float>& getVert();
        int getNbSource();

    private:
        CoordVector m_centreSource;
        std::vector<float> m_vert;
        std::vector<CoordVector> m_centresSources;
        int m_nbDataSource;
};

// Classe Listener : vecteur XYZ des coordonnees du point de reception et le rayon de mesure
class Listener
{
      public:
        Listener();// Constructeur
        ~Listener();// Destructeur
        void chargerListener(); // affecte une valeur aux attribus
        void chargerVert(float coord);
        QString afficher(); // pour afficher les coordonnées du centre et le rayon dans une fenetre
        //accesseurs
        CoordVector getCentre();
        float getRayon();
        std::vector<float>& getVert();

    private:
        CoordVector m_centreListener;
        float m_rayon;
        std::vector<float> m_vert;
};


class MeshObj
{
    public:
        MeshObj();
        ~MeshObj();

        void charger_obj(QString file_obj);

        Source getSource() const; //accesseur aux parametres de source
        std::vector<Listener> getListener(); //accesseur aux parametres du listener
        //std::vector<float>& getVertex() ; //accesseur aux vertex
        //std::vector<float>& getNormals() ;
        std::vector<float>& getIndMat() ;
        //int getNb_data() const;

        std::vector<CoordVector> &getVert() ;
        std::vector<CoordVector> &getNorm() ;
        std::vector<CoordVector> &getVectFace() ;
        CoordVector &getMin();
        CoordVector &getMax();
        QString &getfilenmae();


    private:

        //std::vector<float> m_vert;
        //std::vector<float> m_norm;
        CoordVector m_min, m_max; // min et max du mesh pour création de boite englobante
        std::vector<float> m_indMat;

        std::vector<CoordVector> m_vertex;
        std::vector<CoordVector> m_normales;

        std::vector<CoordVector> m_vecteurFace;

        //int m_nbData;
        Source m_source;
        std::vector<Listener> m_listener;
        QVector<QString> m_materiaux;

        QString m_filename;

};

#endif // OBJREADER_H

