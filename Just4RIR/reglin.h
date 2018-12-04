#ifndef REGLIN_H
#define REGLIN_H
#include <vector>

/* Fonctions de test */

int test_zero(std::vector<double> tab,int n);
int test_negatif(std::vector<double> tab,int n);

/* Fonctions de changement de variables */

void logtab(std::vector<double> tab,std::vector<double> tabTemp,int n);
void lntab(std::vector<double> tab,std::vector<double> tabTemp,int n);
void invtab(std::vector<double> tab,std::vector<double> tabTemp,int n);

/* Fonctions de rÈgression */

double pente(std::vector<double> Xi,std::vector<double> Yi,int n);
double ordonnee(std::vector<double> Xi,std::vector<double> Yi,int n);
double corr(std::vector<double> Xi,std::vector<double> Yi,int n);
int ajustement(std::vector<double> Xi,std::vector<double> Yi,int n);

//std::vector<double> tab_temp;

/*******************/
/* Fin de reglin.h */
/*******************/

#endif // REGLIN_H
