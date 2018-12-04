/*/////////////////////////////// INFORMATIONS ///////////////////////////////////////
Software name : Just4RIR
Creation date : November 30th 2018
Last modification : November 30th 2018
Author : Robin Gueguen
License : GPL 3.0 2018
Property : Institut des Sciences du Calcul et des Données - Sorbonne Université
Version : 1.0 beta
Function : Rays propagation
*/////////////////////////////////////////////////////////////////////////////////////

#ifndef RAYTRACING_H
#define RAYTRACING_H

#include "objreader.h"
#include "octree.h"

// Méthodes
CoordVector intersection(const CoordVector &point_ray, const CoordVector &vect_dir, const CoordVector &vect_norm, float k);
std::vector<float> intersection(std::vector<float>& point_ray, std::vector<float>& vect_dir,int indRay, std::vector<float>& vect_norm, int indFace, float k);
bool appartient_face(const CoordVector &point, std::vector<float>& face);
bool appartient_face(std::vector<float>& point, int iP, std::vector<float>& face, int iF);
bool appartient_face(const CoordVector &point, const CoordVector &a, const CoordVector &b, const CoordVector &c);
std::vector<float> &vecteur_reflechi(std::vector<float> &i, int ii, std::vector<float> &n, int in);
CoordVector vecteur_reflechi(const CoordVector &i, const CoordVector &n);
CoordVector vecteur_reflechi(const std::vector<float> &i, int ind, const CoordVector &n);
float triangle_intersection(const CoordVector &orig, const CoordVector &dir,
                            const CoordVector &v0, const CoordVector &v1, const CoordVector &v2);


// Les classes
class Ray
{
public:
    Ray(int Nray, Source S, int nSrc, bool fibonacci);   //Constructeur
    Ray(const CoordVector &point, const std::vector<CoordVector>& dir); // Constructeur surchargé
    Ray(const std::vector<CoordVector> &point, const std::vector<CoordVector>& dir);
    ~Ray();                               //Destructeur
    std::vector<CoordVector>& getRay() ;
    std::vector<float>& getNRG() ;
    std::vector<float>& getNRGbackup() ;
    std::vector<CoordVector>& getPos() ;
    std::vector<CoordVector>& getDir() ;
    std::vector<CoordVector>& getvDir();
    std::vector<float>& getDist() ;
    std::vector<float>& getLong() ;
    std::vector<bool>& getRayVivant() ;
    std::vector<bool>& getRayVivant2() ;
    void killRay(int i);

    int getNbRay() const;
    int getRayMorts() const;

    bool rebondSansMemoire(MeshObj mesh);
    bool rebondSansMemoire(MeshObj &mesh, Octree &oct); // fonction surchargée avec octree

    void stockage();


private:    

    std::vector<int> m_col;     // Donnees de collision des rayons sur le maillage [iray, ielt, vray]    
    float m_dMax;               // Distance max parcourue par les rayons

    // Utilisés
    int m_Nray;                 // Nombre de rayon * 3 (car trois coordonnées)
    CoordVector m_src;          // Position absolue de la source
    std::vector<float> m_nrg;   // Energie portee par le rayon au moment t
    std::vector<float> m_nrgBackup;   // Energie portee par le rayon

    std::vector<CoordVector> m_pos;   // Position du point de départ du rayon stockée
    std::vector<CoordVector> m_ray;   // Position du point de départ du rayon au moment t
    std::vector<CoordVector> m_dir;   // Direction des rayons stockée
    std::vector<CoordVector> m_vDir;  // vecteur directeur des rayons au moment t
    std::vector<float> m_long;  // longueur du dernier segment de rayon
    std::vector<float> m_dist;  // Distance totale parcourue par les rayons au moment t

    std::vector<bool> m_rayVivant;  // longueur du dernier segment de rayon
    std::vector<bool> m_rayVivantBackup;
    int m_nbRayMort;
    std::vector<bool> m_rayColineaire;

};


#endif // RAYTRACING_H
