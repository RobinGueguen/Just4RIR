/*/////////////////////////////// INFORMATIONS ///////////////////////////////////////
Software name : Just4RIR
Creation date : November 30th 2018
Last modification : November 30th 2018
Author : Robin Gueguen
License : GPL 3.0 2018
Property : Institut des Sciences du Calcul et des Données - Sorbonne Université
Version : 1.0 beta
Function : Audio
*/////////////////////////////////////////////////////////////////////////////////////

#ifndef AUDIO_H
#define AUDIO_H

#include "QString"
#include "vector"
#include <QObject>
#include <QFile>
#include <QAudioFormat>
#include <math.h>

bool bandFilters(std::vector<std::vector<float> > &output);
void zeroPadding(std::vector<float>& vecteur, int taille);
void partitionner(std::vector<float> &donnee, int taille, std::vector<std::vector<float> > &output);
void recombiner(std::vector< std::vector<float> > &input, std::vector<float> &output);
std::vector<float> convolution_temporelle(std::vector<float> &a,std::vector<float> &b);


class WavFile : public QFile
{
public:
    WavFile(QObject *parent = 0);

    using QFile::open;
    bool open(const QString &fileName);
    const QAudioFormat &fileFormat() const;
    qint64 headerLength() const;
    void writeNewWav(std::vector<int> &donnees);

private:
    bool readHeader();

private:
    QAudioFormat m_fileFormat;
    qint64 m_headerLength;
};


#endif // AUDIO_H
