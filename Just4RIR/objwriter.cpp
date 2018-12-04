/*/////////////////////////////// INFORMATIONS ///////////////////////////////////////
Software name : Just4RIR
Creation date : November 30th 2018
Last modification : November 30th 2018
Author : Robin Gueguen
License : GPL 3.0 2018
Property : Institut des Sciences du Calcul et des Données - Sorbonne Université
Version : 1.0 beta
Function : Create .OBJ file with analyse results
*/////////////////////////////////////////////////////////////////////////////////////

#include "objwriter.h"
#include <QFile>
#include <QTextStream>
#include <QInputDialog>
#include <QIODevice>
#include "QDebug"
#include <QtMath>
#include <QMessageBox>
#include <QCoreApplication>


ObjWriter::ObjWriter(QString chemin, int nbRay) // recupere en attribue le nom de chemin de fichier specifié
{

    QFile fichier(chemin);

    QString newName(chemin);
    m_buff_rayMort.resize(nbRay, 0); // 0 = rayon vivant

    int i=0;
    while(fichier.exists()) // incrementation de version de fichier s'il existe deja
    {
        QStringList nom = fichier.fileName().split(".obj");
        if (nom[0].contains("EXPORT_"))
        {
            QStringList nom2 = nom[0].split("EXPORT_");
            newName = nom2[0] + "EXPORT_" + QString::number(i) + ".obj" ;
        }
        else
        {
            newName = nom[0] + "_" + QString::number(i) + ".obj" ;
        }

        i++;
        fichier.setFileName(newName);
    }


      m_chemin = newName; // attribution du dernier nom
      m_nbligne = 0;
}

ObjWriter::~ObjWriter()
{
}


void ObjWriter::rec_Vert(Source &source, int nSrc, Ray &monRay, int num_rebond)
{
    QFile fichier(m_chemin);
    std::vector<CoordVector> ray = monRay.getRay();
    std::vector<bool> vivant = monRay.getRayVivant2();


    int nbRay = ray.size();

    if(fichier.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) // ouvre le fichier
    {
        // creation d'un entete
        QString text("o Rays \n");

        if (num_rebond == 0)
        {
            // coordonnées du premier point
            text = text + "v " + CoordVector2QString(source.getCentre(nSrc)) + "\n";
            fichier.write(text.toLatin1());
        }

        // ecriture des vertex
        for (int i = 0; i < nbRay ; i++)
        {
            if(vivant[i])
            {
                text = "v "+ CoordVector2QString(ray[i]) + "\n";
                fichier.write(text.toLatin1());
                m_rayMort.push_back(false);
            }
            else // S'il était deja mort
            {
                m_rayMort.push_back(true);
            }
        }
    }
    fichier.close(); // ferme le fichier
}

void ObjWriter::rec_Vert_init(std::vector<CoordVector> &si)
{
    QFile fichier(m_chemin);
    if(fichier.open(QIODevice::WriteOnly | QIODevice::Text)) // ouvre le fichier
    {
        // creation d'un entete
        QString text("o Rays \n");
       for(int i=0 ; i<si.size() ; i++)
        {
            text+= "v " + CoordVector2QString(si[i]) + "\n";
        }

        fichier.write(text.toLatin1());
    }
    fichier.close();
}

void ObjWriter::rec_Vert(Ray &monRay, int ind, CoordVector source)
{
    QFile fichier(m_chemin);
    std::vector<CoordVector> A = monRay.getPos();
    QString text;

    if(fichier.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) // ouvre le fichier
    {
        text = "v " + CoordVector2QString(A[ind]) + "\n";
        text += "v " + CoordVector2QString(source) + "\n";
        fichier.write(text.toLatin1());
        text = "l "+ QString::number(m_nblignefin+1) + " " + QString::number(m_nblignefin+2) + "\n";
        fichier.write(text.toLatin1());
        m_nbligne+=2;
        m_nblignefin+=2;
    }
    fichier.close();
}


void ObjWriter::rec_Vert(Ray &monRay)
{
    QFile fichier(m_chemin);
    std::vector<CoordVector> A = monRay.getPos();

    std::vector<bool> vivant = monRay.getRayVivant2();
    QString text;
    int nbNouvelleVert(0);

    float nb_mort(0);

    if(fichier.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) // ouvre le fichier
    {
        for (int i=0 ; i<A.size() ; i++)
        {
            if (vivant[i])
            {
                text += "v " + CoordVector2QString(A[i]) + "\n";
                m_nbligne++;
                m_buff_rayMort[i] =m_nbligne;
                nbNouvelleVert++;
            }
            else nb_mort++;
        }
        fichier.write(text.toLatin1());
        text="";

        for (int i=0 ; i<A.size() ; i++)
        {
            if (vivant[i])
            {
                text += "l "+ QString::number(m_buff_rayMort[i]-nb_mort) + " " + QString::number(m_buff_rayMort[i]+nbNouvelleVert) + "\n";
                m_buff_rayMort[i]+=m_nbligne; // passer à la prochaine série
            }
            else nb_mort--;
        }
        fichier.write(text.toLatin1());
        m_nblignefin = m_nbligne+nbNouvelleVert;
    }
    fichier.close();
}


void ObjWriter::rec_Line(int nbRay)
{
    QFile fichier(m_chemin);
    QString ligne;

    if(fichier.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) // ouvre le fichier et place le curseur à la fin
    {

        std::vector<int> dernierVertex;
        dernierVertex.resize(nbRay,0); // numero de ligne du dernier vertex du rayon

        int indiceRay(2);
      // relier les premiers points à la source
        for (int i = 0 ; i<nbRay ; i++)
        {
            if(!m_rayMort[i]){
            ligne = "l 1 " + QString::number(indiceRay) + "\n";
            fichier.write(ligne.toLatin1());
            dernierVertex[i] = indiceRay;
            indiceRay++;
            }
        }
        float nbIt = m_rayMort.size()/nbRay;
        for (int j = 1; j<nbIt; j++)
        {
            for(int i=0; i<nbRay ; i++)
            {
                if(!m_rayMort[i+j*nbRay]){
                ligne = "l " + QString::number(dernierVertex[i]) +" "+ QString::number(indiceRay) + "\n";
                fichier.write(ligne.toLatin1());
                dernierVertex[i] = indiceRay;
                indiceRay++;
                }
            }
        }
    }
    fichier.close(); // ferme le fichier
}




void ObjWriter::display_sourceImages(std::vector<CoordVector> &sourcesImages)
{
    QFile fichier(m_chemin);

    fichier.open(QIODevice::WriteOnly | QIODevice::Text); // ouvre le fichier

    // creation d'un entete
    QString text("o Sourcesimages \n");
    fichier.write(text.toLatin1());

    // ecriture des vertex représentant les posotions de sources images
    for (int i = 0; i < sourcesImages.size() ; i++)
    {
        text = "v "+ CoordVector2QString(sourcesImages[i]) + "\n";
        fichier.write(text.toLatin1());
    }

    fichier.close(); // ferme le fichier
}

void ObjWriter::display_coloredTriangle(std::vector<CoordVector> &point, std::vector<float> &nrg, const CoordVector &dirNormal, const CoordVector &posSource, float seuil)
{
    QFile fichier(m_chemin);

    fichier.open(QIODevice::WriteOnly | QIODevice::Text); // ouvre le fichier

    // creation d'un entete
    QString text("mtllib materiaux.mtl\n");
    text += "o SI_Projected \n";
    fichier.write(text.toLatin1());

    CoordVector vect;

    float normVec, nbpoint(point.size());
    std::vector<float> nrgMoy;
    nrgMoy.resize(nbpoint, 0);


    int j;

    // pour faire un carré
    float splatsize = 0.5;
    CoordVector a( splatsize/2, splatsize/2,0);
    CoordVector b( splatsize/2,-splatsize/2,0);
    CoordVector c(-splatsize/2,-splatsize/2,0);
    CoordVector d(-splatsize/2, splatsize/2,0);


    //ECRITURE SOURCE
    vect    = vecteur(posSource,dirNormal);
    normVec = norme(vect);
    if(normVec==0) // source et recepteur confonduq
    {
        vect.x=1;
    }
    else vect    = vect/normVec;
    CoordVector A(a),B(b),C(c),D(d); // initialisation
    makeSplat(A,posSource,vect);
    makeSplat(B,posSource,vect);
    makeSplat(C,posSource,vect);
    makeSplat(D,posSource,vect);
    // ecriture des carrés
    text  = "v "+ CoordVector2QString(A) + "\n";
    text += "v "+ CoordVector2QString(B) + "\n";
    text += "v "+ CoordVector2QString(C) + "\n";
    text += "v "+ CoordVector2QString(D) + "\n";
    fichier.write(text.toLatin1());

    // ecriture des vertex représentant les positions de sources images
    for (int i = 1; i < nbpoint ; i++)
    {
        vect    = vecteur(point[i],dirNormal);
        normVec = norme(vect);
        vect    = vect/normVec;
        //point[i]+= vect*0.1;// on reduit la distance au listener
        CoordVector A(a),B(b),C(c),D(d); // initialisation

        makeSplat(A,point[i],vect);
        makeSplat(B,point[i],vect);
        makeSplat(C,point[i],vect);
        makeSplat(D,point[i],vect);

        // ecriture des carrés
        text  = "v "+ CoordVector2QString(A) + "\n";
        text += "v "+ CoordVector2QString(B) + "\n";
        text += "v "+ CoordVector2QString(C) + "\n";
        text += "v "+ CoordVector2QString(D) + "\n";
        fichier.write(text.toLatin1());
    }

    // ecriture de la normale
    for (int i = 0; i < nbpoint ; i++)
    {
        vect = vecteur(point[i],dirNormal);
        normVec = norme(vect);

        text = "vn "+ CoordVector2QString(vect/normVec) + "\n";
        fichier.write(text.toLatin1());

        // vecteur energie
        for (j=0 ; j<8 ; j++) nrgMoy[i] += nrg[8*i+j];
        nrgMoy[i] /= 8;
    }

    float max = *std::max_element(nrgMoy.begin(), nrgMoy.end());

    for(auto &a : nrgMoy) a/=max; // normalisation
    float min = *std::min_element(nrgMoy.begin(), nrgMoy.end());
    if (min > seuil) min = seuil;
    genererMTL(min);


    for (int i = 0; i < nbpoint ; i++)
    {
        text = "usemtl " + QString::number(round(10*log10(nrgMoy[i]))) + "dB\n";
        text += "s off\n";
        text += "f ";
        for (j=1 ; j< 5 ;j++)
        {
            text += QString::number(4*i+j) + "//" + QString::number(i+1) + " "; // ecriture des vertex et leur normale
        }
        text += "\n";
        fichier.write(text.toLatin1());
    }


    fichier.close(); // ferme le fichier
}

//https://visheshvision.wordpress.com/2014/04/28/rendering-a-colored-point-cloud-in-blender/
void RotateX(CoordVector &P, float ang)
{
    float y= P.y*cos(ang) - P.z*sin(ang);
    float z= P.y*sin(ang) + P.z*cos(ang);
    P.y=y;
    P.z=z;
}

void RotateY(CoordVector &P, float ang)
{
    float x= P.x*cos(ang) + P.z*sin(ang);
    float z= -P.x*sin(ang) + P.z*cos(ang);
    P.x=x;
    P.z=z;
}

void Translate(CoordVector &P,CoordVector V)
{
    P.x+=V.x;
    P.y+=V.y;
    P.z+=V.z;
}

void makeSplat(CoordVector &P, CoordVector pos,CoordVector nor)
{

    float theta = asin(nor.y); // OK !
    float phi = atan(nor.x/nor.z);
    if (nor.z >0) phi+= M_PI;

    RotateX(P,theta);
    RotateY(P,phi);
    Translate(P,pos);
}

void genererMTL(float min)
{
    QFile fichier(QCoreApplication::applicationDirPath() + "/materiaux.mtl");

    fichier.open(QIODevice::WriteOnly | QIODevice::Text); // ouvre le fichier

    // creation d'un entete
    QString text;
    fichier.write(text.toLatin1());
    min = -10*log10(min);

    // ecriture des vertex représentant les posotions de sources images
    for (float i = 0; i <= min ; i++)
    {
        if (i==0) text  = "newmtl " + QString::number(i) + "dB\n";  // nom du materiaux
        else text  = "newmtl -" + QString::number(i) + "dB\n";  // nom du materiaux
        text += "Ka 1 1 1\n"; // couleur ambiante
        text += "Kd " + HSV2RGB(round(240*i/min), 1, 1) + "\n"; // couleur diffuse RGB (Hue de 240 à 0)
        text += "Ks 0 0 0\n"; // specular
        text += "Ni 1\n"; // densité
        text += "d 0.5\n"; // transparence
        text += "illum 2\n"; // lumière
        text += "\n";

        fichier.write(text.toLatin1());
    }
    fichier.close(); // ferme le fichier
}


// Méthodes
QString HSV2RGB(float h, float s, float v)
{
    float r = 0, g = 0, b = 0;

    int i;
    float f, p, q, t;

    if (h == 360)
        h = 0;
    else
        h = h / 60;

    i = (int)trunc(h);
    f = h - i;

    p = v * (1.0 - s);
    q = v * (1.0 - (s * f));
    t = v * (1.0 - (s * (1.0 - f)));

    switch (i)
    {
    case 0:
        r = v;
        g = t;
        b = p;
        break;

    case 1:
        r = q;
        g = v;
        b = p;
        break;

    case 2:
        r = p;
        g = v;
        b = t;
        break;

    case 3:
        r = p;
        g = q;
        b = v;
        break;

    case 4:
        r = t;
        g = p;
        b = v;
        break;

    default:
        r = v;
        g = p;
        b = q;
        break;
    }

    return QString::number(r) + " " + QString::number(g) + " " + QString::number(b);
}
