/*/////////////////////////////// INFORMATIONS ///////////////////////////////////////
Software name : Just4RIR
Creation date : November 30th 2018
Last modification : November 30th 2018
Author : Robin Gueguen
License : GPL 3.0 2018
Property : Institut des Sciences du Calcul et des Données - Sorbonne Université
Version : 1.0 beta
Function : Analytic test case of a shoes box
*/////////////////////////////////////////////////////////////////////////////////////

#ifndef ANALYTIQUE_H
#define ANALYTIQUE_H

#include "objreader.h"
#include "rir.h"

void cubeAnalytique(int temp, int hum, Source source, Listener listener, int nbRebond, SourceImage sourceImage, int numListener);


#endif // ANALYTIQUE_H
