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

#include "objreader.h"
#include "QFile"
#include "QIODevice"
#include "QTextStream"
#include "QDebug"
#include <QMessageBox>
#include <QFileDialog>
#include "mainwindow.h"


Source::Source() {
    m_nbDataSource = 0;
}

Source::~Source(){
}

void Source::chargerSource()
{
    // Remise à 0 du centre
    m_centreSource = CoordVector();
    CoordVector max(m_vert[m_nbDataSource], m_vert[m_nbDataSource+1], m_vert[m_nbDataSource+2]) , min(m_vert[m_nbDataSource], m_vert[m_nbDataSource+1], m_vert[m_nbDataSource+2]);

    // Moyenne des vertex sur chaque coordonnée
   for (int i = m_nbDataSource ; i < m_vert.size() ; i++)
   {
       m_centreSource[i] += m_vert[i]; // l'operateur [] a un modulo 3
       if (m_vert[i] > max[i]) max[i] = m_vert[i];
       if (m_vert[i] < min[i]) min[i] = m_vert[i];
   }

   // centre
   if (!m_vert.empty()) m_centreSource = (max+min)/2;

   // Ajout au vecteur de centres
   m_centresSources.push_back(m_centreSource);
   m_nbDataSource = m_vert.size(); // Si plusieurs sources : on indique à quel endroit du vecteur on change de source
}

void Source::chargerVert(float coord){
    m_vert.push_back(coord);
}

CoordVector Source::getCentre() {
    return m_centreSource;
}

CoordVector Source::getCentre(unsigned int n) {
    if (n < m_centresSources.size())
        return m_centresSources[n];
    else qDebug() << "Wrong source number";
}

std::vector<float>& Source::getVert() {
    return m_vert;
}

QString Source::afficher() const {
    return "Source\nCenter (m) : " + CoordVector2QString2(m_centreSource);
}

Source MeshObj::getSource() const {
    return m_source;
}

int Source::getNbSource(){
    return m_centresSources.size();
}

Listener::Listener()
{
    m_rayon = 1;
}

Listener::~Listener(){
}

void Listener::chargerVert(float coord){
    m_vert.push_back(coord);
}

void Listener::chargerListener()
{
   for (int i = 0 ; i < m_vert.size() ; i++)
   {
       m_centreListener[i] += m_vert[i]; // l'operateur [] a un modulo 3
   }
   m_centreListener = m_centreListener/(m_vert.size()/3);
   CoordVector point(m_vert[0], m_vert[1], m_vert[2]);
   m_rayon = norme(vecteur(m_centreListener, point));
   if (m_rayon < 0.001) QMessageBox::warning(NULL,"Beware","Listener radius is below 1mm. Please load a new Listener.", "OK");
}

QString Listener::afficher()
{    
    QString ray;
    ray.setNum(m_rayon);
    QString info = "Center (m) : "+ CoordVector2QString2(m_centreListener) +"\n"
            + "Radius (m) : " + ray;
    return info;
}

CoordVector Listener::getCentre() {
    return m_centreListener;
}

float Listener::getRayon() {
    return m_rayon;
}

std::vector<float>& Listener::getVert() {
    return m_vert;
}

std::vector<Listener> MeshObj::getListener() {
    return m_listener;
}

MeshObj::MeshObj() {
    m_listener.clear();
    //charger_obj(s);
}

MeshObj::~MeshObj() {
}

std::vector<CoordVector> &MeshObj::getVert() {
    return m_vertex;
}

std::vector<CoordVector> &MeshObj::getNorm() {
    return m_normales;
}
std::vector<CoordVector> &MeshObj::getVectFace() {
    return m_vecteurFace;
}

std::vector<float> &MeshObj::getIndMat() {
    return m_indMat;
}

CoordVector &MeshObj::getMin(){
    return m_min;
}

CoordVector &MeshObj::getMax(){
    return m_max;
}

QString &MeshObj::getfilenmae(){
    return m_filename;
}


void MeshObj::charger_obj(QString file_obj)
{
    std::vector<CoordVector> ver, nor; // vecteurs de coordonnees
    std::vector<unsigned int> iv, in; // indice des points à assembler
    std::vector<float> imat;
    bool lecture_source = false, lecture_listener = false;
    int nb_verSource = 0, nb_verListener = 0, nb_norSource = 0, nb_norListener = 0, i, j;
    unsigned int v,n;
    float x,y,z;

    Material matOdeon; // chargement des materiaux Odéons
    std::vector<QString> matOdeon_vect = matOdeon.getNomMat();

    int rangCoeff = 7; // par default c'est le materiaux 50% absorbant
    QString nomMat, ligne;

    QFile fichier(file_obj); // fichier .obj
    QStringList coord, materiau, indice;

    if(!fichier.exists()) // Si on ne peut pas ouvrir le fichier
    {
       QMessageBox::warning(NULL,"Mesh file not found","Mesh file not found in the current folder. Please select a .obj mesh file.");
       file_obj = QFileDialog::getOpenFileName(NULL, "Select mesh file", QDir::homePath(), "Mesh (*.obj)");
       fichier.setFileName(file_obj);
    }
    if(fichier.open(QIODevice::ReadOnly | QIODevice::Text)) // Si on peut ouvrir le fichier
    {
        m_filename = file_obj;
        QTextStream flux(&fichier);
        while(!flux.atEnd())
        {
            ligne = flux.readLine(); // Lecture ligne par ligne du fichier

            if(lecture_source || lecture_listener) // Si l'objet lu est la source ou le listener
            {
                if(ligne[0]=='v')
                {
                    if(ligne[1]==' ') //Vertex
                    {
                        coord = ligne.split(" ");

                        if(lecture_source) {
                            for(i=0; i<3;i++) {
                                m_source.chargerVert(coord[i+1].toFloat());
                            }
                            nb_verSource++;
                        }
                        if(lecture_listener) {
                            for(i=0; i<3;i++) {
                                m_listener[m_listener.size()-1].chargerVert(coord[i+1].toFloat());
                            }
                            nb_verListener++;
                        }
                    }
                    else //normales + textures
                    {
                        if(lecture_source)   nb_norSource++;
                        if(lecture_listener) nb_norListener++;
                    }
                }
            }

            else // mode chargement du mesh normal
            {
                //Coordonnees de points (vertex, normale) (on ne s'occupe pas des textures)
                if(ligne[0]=='v' && ligne[1]!='t')
                {
                    coord = ligne.split(" ");
                    x = coord[1].toFloat();
                    y = coord[2].toFloat();
                    z = coord[3].toFloat();

                    if(ligne[1]==' ') //Vertex
                    {                       
                        ver.push_back(CoordVector(x,y,z)); // C'est un std::vector rempli avec les CoordVector de coordonnees des vertex
                    }
                    else if(ligne[1]=='n') //Normales
                    {
                        nor.push_back(CoordVector(x,y,z)); // C'est un std::vector rempli avec les CoordVector de coordonnees de normales
                    }
                }

                // Les matériaux
                if(ligne[0]=='u')
                {
                    materiau = ligne.split(" ");
                    indice = materiau[1].split("_"); // ne prend que le premier "mot" du nom du materiau

                    rangCoeff = 7; // valeur par default de l'absorption du materiau correspond à 50% sur toutes les bandes
                    for(int i=0 ; i < matOdeon_vect.size(); i++ )
                    {
                        if (matOdeon_vect[i] == indice[0]) // si le materiau existe dans le tableau odeon
                        {
                            nomMat = indice[0];
                            rangCoeff = i;
                        }
                    }
                }

                //Les faces : f V1/T1/N1 V2/T2/N2 V3/T3/N3
                if(ligne[0]=='f')
                {
                    if(ligne.contains("//")) // pour les faces sans texture
                    {
                        ligne = doubleSlash(ligne); // on donne 1 comme numero de texture
                    }
                    ligne = supprimeSlash(ligne); // on supprime les slashs pour pourvoir ecrire les vecteurs :f V1 T1 N1 V2 T2 N2 V3 T3 N3

                    indice = ligne.split(" ");

                    int nbDonnees = indice.size()/3; //nombre de donnees V1/T1/N1 par face (dans le cas de faces triangles) : 10/3

                    for(i= 1; i<=nbDonnees;i++)
                    {
                        v=indice[i*3-2].toInt() - (nb_verSource + nb_verListener) -1; // on enleve le nombre de vertex de source et listener s'il y en a car ils ne s'enregistrent pas dans les vecteurs de coordonnées et 1 pour que indice 1 corresponde à 0
                        n=indice[i*3].toInt() - (nb_norSource + nb_norListener) -1;

                        iv.push_back(v); // indice des vertex
                        in.push_back(n); // indice des normales
                    }
                    // pour chaque face on rempli le vecteur materiau avec son nom et ses 8 coefficients d'absorption
                    m_indMat.push_back(nomMat.toFloat());
                    for (i = 0; i< 8; i++)
                    {
                        m_indMat.push_back(matOdeon.getIndMat(rangCoeff*8+i));
                    }
                }
            }
            if (ligne[0]=='o') // Debut d'un objet
            {
                // condition qui se place apres les actions de ligne pour que l'activation du mode agisse à partir de la ligne suivante
                if(ligne.contains("source")) {
                    lecture_source = true; // on est en mode lecture de source
                    if (!m_source.getVert().empty()) m_source.chargerSource(); // Si ce n'est pas la premiere source on charge la source précedente
                }
                else lecture_source = false;

                if(ligne.contains("listener")) { // on est en mode lecture de listener
                    lecture_listener = true;
                    Listener l;
                    m_listener.push_back(l);
                }
                else lecture_listener = false;
            }
        }
        fichier.close();
    }
    else
    {
        QMessageBox::critical(NULL,"Error","No valid .obj mesh file selected", "OK");
    }


    if (!m_source.getVert().empty()) m_source.chargerSource(); // On charge la source 0 ou la dernière source trouvée et pas encore chargée
    else {// on met une source en 0
        for(i=0; i<3;i++)
        {
            m_source.chargerVert(0);
        }
        m_source.chargerSource();
    }
    if (m_listener.empty()) { // si le fichier n'a pas de listener on ajoute un listener par defaut
        Listener l;
        m_listener.push_back(l);
    }
    else {
       for (i=0; i<m_listener.size(); i++) m_listener[i].chargerListener();
    }

    // Initialisation des min et max
    if (!ver.empty())
         {m_min = ver[0]; m_max = ver[0];}
    else {m_min = 0;      m_max = 0;}

    // classement des coordonnées face par face
    for(i=0; i<iv.size(); i++) // vertex
    {
        if(iv[i]<ver.size())
        {
            m_vertex.push_back(ver[iv[i]]);
            // Recuperation des min et max
            for (j = 0 ; j<3 ; j++)
            {
                if(ver[iv[i]][j]<m_min[j])   m_min[j] = ver[iv[i]][j];
                if(ver[iv[i]][j]>m_max[j])   m_max[j] = ver[iv[i]][j];
            }
        }
    }
    for(i=0; i<in.size(); i++) //normales - pour ne prendre qu'une normale par face on prendrai comme increment i =i+3
    {
        if(in[i]<nor.size())
        {
            m_normales.push_back(nor[in[i]]);
        }
    }

    ///Boite englobante
    // Leger offset pour eviter que la boite englobante ne soit considérée comme une surface
    m_min-=0.01;
    m_max+=0.01;
    // Création des points
    CoordVector v1(m_min.x ,m_min.y ,m_max.z);
    CoordVector v2(m_min.x ,m_max.y ,m_max.z);
    CoordVector v3(m_min.x ,m_min.y ,m_min.z);
    CoordVector v4(m_min.x ,m_max.y ,m_min.z);
    CoordVector v5(m_max.x ,m_min.y ,m_max.z);
    CoordVector v6(m_max.x ,m_max.y ,m_max.z);
    CoordVector v7(m_max.x ,m_min.y ,m_min.z);
    CoordVector v8(m_max.x ,m_max.y ,m_min.z);

    // Création des normales
    CoordVector n1(0, 1, 0);
    CoordVector n2(0, -1, 0);
    CoordVector n3(0, 0, -1);
    CoordVector n4(-1, 0, 0);
    CoordVector n5(0, 0, 1);
    CoordVector n6(1, 0, 0);

    // Ajout des faces
    m_vertex.push_back(v5);m_vertex.push_back(v7);m_vertex.push_back(v3);
    m_vertex.push_back(v6);m_vertex.push_back(v2);m_vertex.push_back(v4);
    m_vertex.push_back(v6);m_vertex.push_back(v4);m_vertex.push_back(v8);
    m_vertex.push_back(v1);m_vertex.push_back(v5);m_vertex.push_back(v3);

    m_vertex.push_back(v6);m_vertex.push_back(v5);m_vertex.push_back(v2);
    m_vertex.push_back(v5);m_vertex.push_back(v1);m_vertex.push_back(v2);
    m_vertex.push_back(v8);m_vertex.push_back(v7);m_vertex.push_back(v6);
    m_vertex.push_back(v7);m_vertex.push_back(v5);m_vertex.push_back(v6);

    m_vertex.push_back(v4);m_vertex.push_back(v3);m_vertex.push_back(v7);
    m_vertex.push_back(v4);m_vertex.push_back(v7);m_vertex.push_back(v8);
    m_vertex.push_back(v2);m_vertex.push_back(v1);m_vertex.push_back(v3);
    m_vertex.push_back(v2);m_vertex.push_back(v3);m_vertex.push_back(v4);

    // Ajout des normales
    for (i = 0; i< 3 ; i++) m_normales.push_back(n1);
    for (i = 0; i< 6 ; i++) m_normales.push_back(n2);
    for (i = 0; i< 3 ; i++) m_normales.push_back(n1);
    for (i = 0; i< 6 ; i++) m_normales.push_back(n3);
    for (i = 0; i< 6 ; i++) m_normales.push_back(n4);
    for (i = 0; i< 6 ; i++) m_normales.push_back(n5);
    for (i = 0; i< 6 ; i++) m_normales.push_back(n6);

    // Ajout des materiaux 100 absorbant (12 faces, le nom du materiau est 1: 100%absorbant et les 8 coefficients valent 1)
    for (i = 0; i< 12*9; i++)
    {
        m_indMat.push_back(1);
    }

    // Création des vecteurs AB et AC pour chaque face ABC
    for(i = 0; i < m_vertex.size() ; i+=3)
    {
        m_vecteurFace.push_back(vecteur(m_vertex[i], m_vertex[i+1]));
        m_vecteurFace.push_back(vecteur(m_vertex[i], m_vertex[i+2]));
    }


    // nettoyage
    ver.clear();
    nor.clear();

    iv.clear();
    in.clear();
    imat.clear();

}



QString doubleSlash(QString s) // On remplace les // correspondant à l'absence de texture par des /1/ pour avoir un numero de texture
{
    QStringList groupeIndice = s.split("//");

    s = "";
    for(int i=0; i<groupeIndice.size()-1; i++)
    {
         s = s + groupeIndice[i] + "/1/";
    }
    s = s + groupeIndice[groupeIndice.size()-1];

    return s;
}

QString supprimeSlash(QString s) // On remplace les / par des espaces
{
    QStringList groupeIndice = s.split("/");

    s = "";
    for(int i=0; i<groupeIndice.size()-1; i++)
    {
         s = s + groupeIndice[i] + " ";
    }
    s = s + groupeIndice[groupeIndice.size()-1];

    return s;
}
