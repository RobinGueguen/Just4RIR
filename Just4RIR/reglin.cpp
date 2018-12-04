
#include <math.h>
#include <stdlib.h>
#include <reglin.h>

/* DÈclaratin globale des variables */

std::vector<double> tab_temp; //DÈclaration d'un tableau temporaire

/*********************/
/* Fonctions de test */
/*********************/

/* Fonction de test de prÈsence d'un zÈro */

int test_zero(std::vector<double> tab,int n)
{
        for(int i=0;i<n;i++)
        {
                if(tab[i]==0)
                { return i;}
        }

        return -1;
}

/* Fonction de test de prÈsence d'un nÈgatif */

int test_negatif(std::vector<double> tab,int n)
{
        for(int i=0;i<n;i++)
        {
                if(tab[i]<0)
                { return i;}
        }

        return -1;
}

/*********************************/
/* Fonctions de valeurs absolues */
/*********************************/

/*fonction qui retourne la valeur absolue*/

double val_abs(double x)
{
    if(x<0) {return -x;}
    else {return x;}
}
/********************************/
/* Fonction de recherche du max */
/********************************/

/* Fonction qui retourne celui qui est le max */

int rmax(std::vector<double> r)
{
        double temp=0;
        int ajust=0;

        for(int i=0;i<5;i++)
        {
                if(r[i]>temp)
                {
                     temp=r[i];
                     ajust = i+1;
                }
        }

        return ajust;
}

/**********************/
/* Fonctions de somme */
/**********************/

/* Fonction de somme d'ÈlÈments d'un tableau d'entier */

int somme(std::vector<int> tab,int n)
{
    int somme=0;

    for (int i=0;i<n;i++)
    {
     somme=((tab[i])+(somme));
    }

    return(somme);
}

/* Fonction de somme d'ÈlÈments d'un tableau de rÈel*/

double somme(std::vector<double> tab,int n)
{
    double somme=0;

    for (int i=0;i<n;i++)
    {
     somme=((tab[i])+(somme));
    }

    return(somme);
}

/**********************************/
/* Fonctions de calcul de moyenne */
/**********************************/

/* Fonction de calcul de moyenne d'ÈlÈments d'un tableau d'entier */

double moyenne(std::vector<int> tab,int n)
{
    double moyenne = double(somme(tab,n))/n;

    return (moyenne);
}

/* Fonction de calcul de moyenne d'ÈlÈments d'un tableau de rÈel */

double moyenne(std::vector<double> tab,int n)
{
    double moyenne = somme(tab,n)/n;

    return (moyenne);
}

/* Fonction de calcul de moyenne d'elements d'un tableau de rÈel(2) */

double moyenne2(double somme,int n)
{
    double moyenne = somme/n;

    return (moyenne);
}

/***********************/
/* Fonction de produit */
/***********************/

/* Fonction de calcul du produit d'ÈlÈments de deux tableau ligne par ligne */

std::vector<double> produittab(std::vector<double> tab1,std::vector<double> tab2,int n)
{
        tab_temp.resize(n,0);   //RÈservation de l'espace mÈmoire
    std::vector<double> resultat;
    for (int i=0;i<n;i++)
    {
        resultat.push_back(tab1[i]*tab2[i]);
    }
 return resultat;
}

/***************************************/
/* Fonctions de changement de variable */
/***************************************/

/* Fonctions de calcul du ln des ÈlÈments d'un tableau de rÈel */

void lntab(std::vector<double> tab,std::vector<double> tabTemp,int n)
{
        tab_temp.resize(n,0);    //RÈservation de l'espace mÈmoire

    for (int i=0;i<n;i++)
    {
        tabTemp[i]=(log(tab[i]));
    }
}

/* Fonctions de calcul du log base 10 des ÈlÈments d'un tableau de rÈel */

void logtab(std::vector<double> tab,std::vector<double> tabTemp,int n)
{
        tab_temp.resize(n,0);     //RÈservation de l'espace mÈmoire

    for (int i=0;i<n;i++)
    {
        tabTemp[i]=(log10(tab[i]));
    }
}

/* Fonction d'inverstion des Èlements d'un tableau de rÈel */

void invtab(std::vector<double> tab,std::vector<double> tabTemp,int n)
{
        tab_temp.resize(n,0);     //RÈservation de l'espace mÈmoire

    for (int i=0;i<n;i++)
    {
        tabTemp[i]=(1/tab[i]);
    }
}

/********************/
/* Autres fonctions */
/********************/

/* Fonction de calcul des Ècarts ‡ la moyenne */

void ecart_a_moyenne(std::vector<double> tab,double Moyenne,int n)
{
        tab_temp.resize(n,0);    //RÈservation de l'espace mÈmoire

    for (int i=0;i<n;i++)
    {
        tab_temp[i]=(tab[i] - Moyenne);
    }
}

/****************************/
/* Fonctions de statistique */
/****************************/

/* Fonction de calcul de la covariance */

double covariance(std::vector<double> Xi,std::vector<double> Yi,int n)
{
    double cov;


    cov = moyenne(produittab(Xi,Yi,n),n) - ( moyenne(Xi,n) * moyenne(Yi,n) );

    return (cov);
}

/* Fonction de calcul de la somme des carrÈs des Ècarts a la moyenne */

double variance(std::vector<double> val,int n)
{
    double sce;

    sce = moyenne(produittab(val,val,n),n) - ( moyenne(val,n) * moyenne(val,n));

    return (sce);
}

/* Fonction de calcul de l'Ècart-type */

double ecarttype(std::vector<double> val,int n)
{
    double ect= sqrt(variance(val,n));

    return (ect);
}
/******************************************************/
/* Fonctions pour le calcul de la rÈgression linÈaire */
/* par la mÈthode des moindres carrÈ                  */
/******************************************************/

/* Fonction de clacul de la pente (a) */

double pente(std::vector<double> Xi,std::vector<double> Yi,int n)
{
    double a = covariance(Xi,Yi,n)/variance(Xi,n);

    return (a);
}

/* Fonction de clacul de l'ordonnÈe a l'origine (b) */

double ordonnee(std::vector<double> Xi,std::vector<double> Yi,int n)
{
    double b = moyenne(Yi,n) - ( pente(Xi,Yi,n) * moyenne(Xi,n) );

    return (b);
}

/* Fonction de calcul du coef de corrÈlation (r) */

double corr(std::vector<double> Xi,std::vector<double> Yi,int n)
{
    double r = covariance(Xi,Yi,n)/(ecarttype(Xi,n)*ecarttype(Yi,n));
        //double r=pente(Xi,Yi,n)*pente(Xi,Yi,n)*(variance(Xi,n)/variance(Yi,n));
    return (r);
}

/* Fonction de dÈtermination du meilleur ajustement */

int ajustement(std::vector<double> Xi,std::vector<double> Yi,int n)
{
        std::vector<double> r(5),lnXi(100),lnYi(100),logXi(100),logYi(100),invXi(100);

        //corrÈlation pour linÈaire

        r[0]=val_abs(corr(Xi,Yi,n));

        //corrÈlation pour exponetielle

        lntab(Yi,lnYi,n);
        r[1]=val_abs(corr(Xi,lnYi,n));

        //corrÈlation pour puissance

        logtab(Xi,logXi,n);
        logtab(Yi,logYi,n);
        r[2]=val_abs(corr(logXi,logYi,n));

        //corrÈlation pour inverse

        invtab(Xi,invXi,n);
        r[3]=val_abs(corr(invXi,Yi,n));

        //corrÈlation pour logarithmique

        lntab(Xi,lnXi,n);
        r[4]=val_abs(corr(lnXi,Yi,n));

        //Test du meilleur ajustement

        return rmax(r);
}

/*****************************/
/* Fin du fichier reglin.cpp */
/*****************************/


