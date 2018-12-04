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

#include "fonction.h"
#include "QDebug"
#include <math.h>

void debugStdVect(std::vector<float>& vect)//pour le debug
{
    QVector<float> vector = QVector<float>::fromStdVector(vect);
    qDebug() << vector;
}

void debugStdVect(std::vector<signed short>& vect)//pour le debug
{
    QVector<signed short> vector = QVector<signed short>::fromStdVector(vect);
    qDebug() << vector;
}

void debugStdVect(std::vector<int>& vect)
{
    QVector<int> vector = QVector<int>::fromStdVector(vect);
    qDebug() << vector;
}

CoordVector sph2cart(float ro, float theta, float phi)
{
    float x,y,z;
    x = ro*cos(phi)*cos(theta);
    y = ro*cos(phi)*sin(theta);
    z = ro*sin(phi);    

    return CoordVector(x,y,z);
}

CoordVector sph2cart(double ro, double theta, double phi)
{
    float x,y,z;
    x = ro*cos(phi)*cos(theta);
    y = ro*cos(phi)*sin(theta);
    z = ro*sin(phi);

    return CoordVector(x,y,z);
}

CoordVector vecteur(const CoordVector &a,const CoordVector &b)
{
    return CoordVector(b.x-a.x,b.y-a.y,b.z-a.z);
}

CoordVector vecteur(std::vector<float>& a, int indA, std::vector<float>& b, int indB)
{
    return CoordVector(b[indB]-a[indA], b[indB+1]-a[indA+1], b[indB+2]-a[indA+2]);
}

CoordVector vecteur(std::vector<float>& a, int indA, const CoordVector &b)
{
    return CoordVector(b.x-a[indA], b.y-a[indA+1], b.z-a[indA+2]);
}

CoordVector vecteur(const CoordVector &a, std::vector<float>& b, int indB)
{
    return CoordVector(b[indB]-a.x, b[indB+1]-a.y, b[indB+2]-a.z);
}

float produitScalaire(const CoordVector &a,const CoordVector &b)
{
    float resultat = a.x*b.x + a.y*b.y + a.z*b.z;

    if(resultat< 1e-8 && resultat > -1e-8)
    {
        return 0;
    }

    return resultat;
}

float produitScalaire(const std::vector<float>& a, int indA,const CoordVector &b)
{
    return (a[indA]*b.x + a[indA+1]*b.y + a[indA+2]*b.z);
}

float produitScalaire(std::vector<float>& a, int indA, std::vector<float>& b, int indB)
{

    float resultat(0);
    for (int i=0; i<3; i++)
    {
       resultat = resultat + a[indA+i]*b[indB+i];
    }
    if(resultat< 0.000001 && resultat > -0.000001)
    {
        return 0;
    }
    return resultat;

}


CoordVector produitVectoriel(const CoordVector &a,const CoordVector &b)
{
    return CoordVector(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}

CoordVector produitVectoriel(const std::vector<float> &a,int i, const CoordVector &b)
{
    return CoordVector(a[i+1]*b.z - a[i+2]*b.y, a[i+2]*b.x - a[i]*b.z, a[i]*b.y - a[i+1]*b.x);
}

bool colineaire(CoordVector &a, CoordVector &b)
{
    CoordVector resultat= produitVectoriel(a,b);
    if(fabs(resultat.x)<1e-6
            &&fabs(resultat.y)<1e-6
            &&fabs(resultat.z)<1e-6) return true;
    else return false;
}

float angle(const CoordVector &a,const CoordVector &b) // angle entre les deux vecteurs
{

    if (norme(a) < 1e-8 || norme(b) < 1e-8) return 0;
    else return acos(produitScalaire(a,b)/(norme(a)*norme(b)));
}

float norme(const CoordVector &a)
{
    return sqrt(produitScalaire(a,a));
}


bool proche(float a, float b)
{
    return proche(a,b,0.001);
}

bool proche(float a, float b, float seuil)
{
    if (fabs(a-b) < seuil) return true;
    else return false;
}

bool proche(CoordVector a, CoordVector b)
{
    return proche(a,b,0.0001);
}
bool proche(CoordVector a, CoordVector b, float seuil)
{
    if (sqrt(pow(a.x-b.x,2)+pow(a.y-b.y,2)+pow(a.z-b.z,2))<seuil)
        return true;
    else return false;
}

void arrondir(CoordVector &a)
{
    arrondir(a,4);
}

void arrondir(CoordVector &a, float nbDecimales)
{
    a = a*pow(10,nbDecimales);
    a.x = round(a.x);
    a.y = round(a.y);
    a.z = round(a.z);
    a = a/pow(10,nbDecimales);
}

std::vector<CoordVector> ranger(std::vector<CoordVector> a, std::vector<int> & indices)
{
    std::vector<CoordVector> resultat;
    int i, j, k;
    std::vector<float> ax, ay, az, ax2, ay2, az2, ax3, ay3, az3;
    for (auto& coord : a) // création de trois vecteur de float pour les coordonnée x, y, z
    {
        ax.push_back(coord.x);
        ay.push_back(coord.y);
        az.push_back(coord.z);
    }
    float xmin, ymin, zmin;
    std::vector<int> indice2erase, indice2erase2;
    indices.clear();
    int compteur(0);


    while(!ax.empty())
    {
        xmin=*std::min_element(ax.begin(), ax.end());
        for(i=0; i<ax.size(); i++)
        {
            if(ax[i]==xmin)
            {
                ax2.push_back(ax[i]);
                ay2.push_back(ay[i]);
                az2.push_back(az[i]);
                indice2erase.push_back(i);
            }
        }
        while(!ay2.empty())
        {
            ymin=*std::min_element(ay2.begin(), ay2.end());
            for(j=0; j<ay2.size(); j++)
            {
                if(ay2[j]==ymin)
                {
                    ax3.push_back(ax2[j]);
                    ay3.push_back(ay2[j]);
                    az3.push_back(az2[j]);
                    indice2erase2.push_back(j);
                }
            }
            while(!az3.empty())
            {
                zmin=*std::min_element(az3.begin(), az3.end());
                for(k=0; k<az3.size(); k++)
                {
                    if(az3[k]==zmin)
                    {
                        resultat.push_back(CoordVector(ax3[k], ay3[k], az3[k]));

                        for (int w=0; w<a.size(); w++)
                        {
                            if(a[w].x == ax3[k]
                            && a[w].y == ay3[k]
                            && a[w].z == az3[k])
                            indices.push_back(w);
                        }
                        break;
                    }
                }
                ax3.erase(ax3.begin()+k);
                ay3.erase(ay3.begin()+k);
                az3.erase(az3.begin()+k);
                compteur=0;
            }
            for(int ind=indice2erase2.size()-1; ind>=0; ind--) {
                ax2.erase(ax2.begin()+indice2erase2[ind]);
                ay2.erase(ay2.begin()+indice2erase2[ind]);
                az2.erase(az2.begin()+indice2erase2[ind]);
            }
            indice2erase2.clear();
        }
        for(int ind=indice2erase.size()-1; ind>=0; ind--) {
            ax.erase(ax.begin()+indice2erase[ind]);
            ay.erase(ay.begin()+indice2erase[ind]);
            az.erase(az.begin()+indice2erase[ind]);
            }
        indice2erase.clear();
    }


    return resultat;
}

// Les classes

CoordVector::CoordVector(float px,float py,float pz):x(px),y(py),z(pz)
{
}

CoordVector::~CoordVector()
{
}
CoordVector CoordVector::operator=(const CoordVector &fv)
{
    x=fv.x;
    y=fv.y;
    z=fv.z;

    return *this;
}

CoordVector CoordVector::operator+=(const CoordVector &fv)
{
    x+=fv.x;
    y+=fv.y;
    z+=fv.z;

    return *this;
}

CoordVector CoordVector::operator-=(const CoordVector &a)
{
    x-=a.x;
    y-=a.y;
    z-=a.z;

    return *this;
}

CoordVector CoordVector::operator+=(float fv)
{
    x+=fv;
    y+=fv;
    z+=fv;

    return *this;
}

CoordVector CoordVector::operator-=(float fv)
{
    x-=fv;
    y-=fv;
    z-=fv;

    return *this;
}


CoordVector operator+(const CoordVector &a, const CoordVector &b)
{
    CoordVector c(a);
    c+=b;
    return c;
}

CoordVector operator+(const CoordVector &a, float b) {
    return CoordVector (a.x+b, a.y+b, a.z+b);
}
CoordVector operator-(const CoordVector &a, float b){
    return a+(-b);
}


CoordVector operator*(const CoordVector &a, const CoordVector &b) {
    return CoordVector (a.x*b.x, a.y*b.y, a.z*b.z);
}

CoordVector operator*(const CoordVector &a, float b) {
    return CoordVector (a.x*b, a.y*b, a.z*b);
}

CoordVector operator/(const CoordVector &a, float b) {
    return CoordVector (a.x/b, a.y/b, a.z/b);
}

bool operator==(const CoordVector &a, const CoordVector &b){
    if(a.x==b.x &&a.y==b.y && a.z == b.z) return true;
    else return false;
}

float coordMax(const CoordVector &a)
{
    float max = a.x;
    if (max < a.y) max = a.y;
    if (max < a.z) max = a.z;

    return max;
}
float coordMin(const CoordVector &a)
{
    float min = a.x;
    if (min > a.y) min = a.y;
    if (min > a.z) min = a.z;

    return min;
}

CoordVector inverse(CoordVector const& a)
{
    return CoordVector(1/a.x, 1/a.y, 1/a.z);
}

QVector<float> CoordVector::CoordVector2Qvector(CoordVector coord)
{
    QVector<float> vect;
    vect.push_back(coord.x);
    vect.push_back(coord.y);
    vect.push_back(coord.z);

    return vect;
}

void CoordVector::debug()
{
    QString Mx,My,Mz;
    Mx.setNum(x);
    My.setNum(y);
    Mz.setNum(z);
    qDebug() << "(" + Mx + "," + My + "," + Mz +")";
}

QString CoordVector2QString(const CoordVector &coord)
{
    QString text = QString::number(coord.x) + " " + QString::number(coord.y) + " " + QString::number(coord.z);
     return text;
}

QString CoordVector2QString2(const CoordVector &coord)
{
    QString text = "x = " + QString::number(round(coord.x*100)/100) + " ; y = " + QString::number(round(-coord.z*100)/100) + " ; z = " + QString::number(round(coord.y*100)/100);
     return text;
}

CoordDouble CoordVector::CoordVector2CoordDouble()
{
    return CoordDouble((double)x,(double)y,(double)z);
}



