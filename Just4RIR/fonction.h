/*/////////////////////////////// INFORMATIONS ///////////////////////////////////////
Software name : Just4RIR
Creation date : November 30th 2018
Last modification : November 30th 2018
Author : Robin Gueguen
License : GPL 3.0 2018
Property : Institut des Sciences du Calcul et des Données - Sorbonne Université
Version : 1.0 beta
Function : Generic functions
*/////////////////////////////////////////////////////////////////////////////////////


#ifndef FONCTION_H
#define FONCTION_H

#include <vector>
#include <QVector>
#include <stdio.h> 


// Les classes
// CoordDouble
class CoordDouble{
    public:

    inline CoordDouble(){x=y=z=0;}
    inline CoordDouble(double a,double b, double c){x=a;y=b;z=c;}
    inline double operator[](int i)const{return ((double*)this)[i];}
    inline double& operator[](int i){return ((double*)this)[i];}


    inline void print (const char* nom) const
    {printf("%s : %f %f %f \n",nom,x,y,z);}

    inline void operator+=(CoordDouble const&a){x+=a.x;y+=a.y;z+=a.z;}
    inline void operator-=(CoordDouble const&a){x-=a.x;y-=a.y;z-=a.z;}
    inline void operator*=(double a){x*=a;y*=a;z*=a;}

    double x;
    double y;
    double z;
};

class CoordVector
{
    /*
    Classe CoordVector : simple vecteur de coordonnées XYZ
    */
    public:
        CoordVector(float px=0,float py=0,float pz=0); // Constructeur
        ~CoordVector();// Destructeur
        CoordVector operator=(const CoordVector &fv);
        CoordVector operator+=(const CoordVector &fv);
        CoordVector operator-=(CoordVector const&a);
        CoordVector operator+=(float fv);
        CoordVector operator-=(float fv);
        QVector<float> CoordVector2Qvector(CoordVector coord);
        void debug();
        inline float operator[](int i)const{return ((float*)this)[i%3];}
        inline float& operator[](int i){return ((float*)this)[i%3];}

        CoordDouble CoordVector2CoordDouble();

        float x,y,z;
};

// Méthodes
CoordVector sph2cart(float ro, float theta, float phi); // convertion coordonnée spherique à cartesiens
CoordVector sph2cart(double ro, double theta, double phi);
CoordVector vecteur(const CoordVector &a,const CoordVector &b);
CoordVector vecteur(std::vector<float>& a, int indA, std::vector<float>& b, int indB);
CoordVector vecteur(std::vector<float>& a, int indA, const CoordVector &b);
CoordVector vecteur(const CoordVector &a, std::vector<float>& b, int indB);
float produitScalaire(const CoordVector &a,const CoordVector &b);
float produitScalaire(const std::vector<float>& a, int indA,const CoordVector &b);
//float produitScalaire(float x1,float y1,float z1,float x2,float y2,float z2);
float produitScalaire(std::vector<float>& a, int indA, std::vector<float>& b, int indB);
CoordVector produitVectoriel(const CoordVector &a,const CoordVector &b);
CoordVector produitVectoriel(const std::vector<float> &a,int i, const CoordVector &b);
float norme(const CoordVector &a);
float angle(const CoordVector &a,const CoordVector &b);
void debugStdVect(std::vector<float>& vect); // permet d'afficher un std::vector dans la console
void debugStdVect(std::vector<signed short>& vect);
void debugStdVect(std::vector<int>& vect);
//CoordVector coord_New_Base(const CoordVector &point, std::vector<float> &mat );
bool proche(float a, float b);
bool proche(float a, float b, float seuil);
bool proche(CoordVector a, CoordVector b);
bool proche(CoordVector a, CoordVector b, float seuil);
//inline float normalize(ffloat max){return sqrt(a)/max;}
std::vector<CoordVector> ranger(std::vector<CoordVector> a, std::vector<int> &indices);
void arrondir(CoordVector &a);
void arrondir(CoordVector &a, float nbDecimales);
bool colineaire(CoordVector &a, CoordVector &b);

CoordVector operator+(const CoordVector &a, const CoordVector &b);
CoordVector operator+(const CoordVector &a, float b);
CoordVector operator-(const CoordVector &a, float b);
CoordVector operator*(const CoordVector &a, float b);
CoordVector operator/(const CoordVector &a, float b);
inline CoordVector operator-(CoordVector const& a){return CoordVector(-a.x,-a.y,-a.z);}
inline CoordVector operator-(CoordVector const&a,CoordVector const&b){CoordVector c(a);c-=b;return c;}
bool operator==(const CoordVector &a, const CoordVector &b);
CoordVector inverse(CoordVector const& a);
// inline permet de mettre les fonction dans le .h
float coordMax(const CoordVector &a);
float coordMin(const CoordVector &a);
QString CoordVector2QString(const CoordVector &coord);
QString CoordVector2QString2(const CoordVector &coord);


inline CoordDouble operator+(CoordDouble const&a,CoordDouble const&b){CoordDouble c(a);c+=b;return c;}
inline CoordDouble operator+(CoordDouble const&a,double b){return CoordDouble(a.x+b, a.y+b, a.z+b);}
inline CoordDouble operator-(CoordDouble const&a,CoordDouble const&b){CoordDouble c(a);c-=b;return c;}
inline CoordDouble operator-(CoordDouble const&a,double b){return a+(-b);}

inline CoordDouble operator-(CoordDouble const& a){return CoordDouble(-a.x,-a.y,-a.z);}

inline CoordDouble operator*(double a,CoordDouble const& b){CoordDouble v(b);v*=a;return v;}
inline double operator*(CoordDouble const& a,CoordDouble const& b){return a.x*b.x+a.y*b.y+a.z*b.z;}

inline CoordDouble vectorielle(CoordDouble const& a,CoordDouble const& b){
    return CoordDouble(
        a.y*b.z-a.z*b.y,
        b.x*a.z-b.z*a.x,
        a.x*b.y-a.y*b.x);
}
inline CoordDouble operator^(CoordDouble const& a,CoordDouble const& b){
    return CoordDouble(
        a.y*b.z-a.z*b.y,
        b.x*a.z-b.z*a.x,
        a.x*b.y-a.y*b.x);
}
inline CoordDouble doubleproduit(CoordDouble const& a,CoordDouble const& b,CoordDouble const& c){
    return (a*c)*b-(a*b)*c;
}



#endif // FONCTION_H
