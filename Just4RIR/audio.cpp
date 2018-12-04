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

#include "audio.h"
#include "QFile"
#include "QDebug"
#include "QDataStream"
#include <QCoreApplication>
#include "fonction.h"
#include <QMessageBox>
#include "fftext.h"


bool bandFilters(std::vector< std::vector<float> >& output)
{
    QFile fichier(":/bandFilter/bandFilters.txt");

     if(fichier.open(QIODevice::ReadOnly | QIODevice::Text)) // Si on peut ouvrir le fichier
     {
         std::vector<float> filtre;
         QTextStream flux(&fichier);

         while (!flux.atEnd()) {
              if (filtre.size() < 257) filtre.push_back(flux.readLine().toFloat());
             else {
                 output.push_back(filtre);
                 filtre.clear();
                }
          }
         output.push_back(filtre);
         fichier.close();
         return true;
     }
     else QMessageBox::critical(NULL,"Error","Error by loading bandFilter.txt file !");
return false;

}

void zeroPadding(std::vector<float>& vecteur, int taille)
{
    if (vecteur.size() > taille) QMessageBox::warning(NULL,"Beware","ZeroPadding issue !");
    else {
        while(vecteur.size() < taille) vecteur.push_back(0);
    }
}

void partitionner(std::vector<float> &donnee, int taille, std::vector< std::vector<float> > &output)
{
    std::vector<float> buffer;
    buffer.resize(taille, 0);
    int rang(0), i;

    //init
    buffer.resize(taille, 0); // zero padding sur le dernier vecteur
    for(i = 0; i< taille/2; i++)
        {
            buffer[i+taille/2] = donnee[i];
        }
    output.push_back(buffer);

    // corps
    while(rang+taille < donnee.size())
    {
        for(i = 0; i< taille; i++)
        {
            buffer[i] = donnee[i+rang];
        }
        output.push_back(buffer);
        rang+=taille/2;
    }

    // zero padding sur l'avant dernier vecteur
    buffer.assign(taille,0);
    for(i = 0; i< donnee.size()-rang; i++)
        {
            buffer[i] = donnee[i+rang];
        }
    output.push_back(buffer);

    // création d'un dernier vecteur pour avoir la premiere moitier correspondant à la fin du vecteur
    buffer.assign(taille, 0);
    for(i = 0; i< taille/2; i++)
        {
            buffer[i] = output[output.size()-1][i+taille/2];
        }
    output.push_back(buffer);

}

void recombiner(std::vector< std::vector<float> > &input, std::vector<float> &output)
{
    output.clear();
    int i;
    int size = input[0].size()/2 ;
    for(std::vector<float> &a : input)
    {
        for(i = 0 ; i< size; i++)
        {
            output.push_back(a[i]);
        }
    }
}

std::vector<float> convolution_temporelle(std::vector<float> &a,std::vector<float> &b)
{
    std::vector<float> resultat;
    resultat.resize(a.size()+b.size()-1,0);
    int i,  j;
    for (i=0; i< a.size() ; i++)
    {
        for(j=0; j <b.size() ; j++)
        {
            resultat[i+j] += a[i]*b[j];
        }
    }
    return resultat;
}




/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <qendian.h>
#include <QVector>

struct chunk
{
    char        id[4];
    quint32     size;
};

struct RIFFHeader
{
    chunk       descriptor;     // "RIFF"
    char        type[4];        // "WAVE"
};

struct WAVEHeader
{
    chunk       descriptor;
    quint16     audioFormat;
    quint16     numChannels;
    quint32     sampleRate;
    quint32     byteRate;
    quint16     blockAlign;
    quint16     bitsPerSample;
};

struct DATAHeader
{
    chunk       descriptor;
};

struct CombinedHeader
{
    RIFFHeader  riff;
    WAVEHeader  wave;
};

WavFile::WavFile(QObject *parent)
    : QFile(parent)
    , m_headerLength(0)
{

}

bool WavFile::open(const QString &fileName)
{
    close();
    setFileName(fileName);
    return QFile::open(QIODevice::ReadOnly) && readHeader();
}

const QAudioFormat &WavFile::fileFormat() const
{
    return m_fileFormat;
}

qint64 WavFile::headerLength() const
{
return m_headerLength;
}

bool WavFile::readHeader()
{
    seek(0);
    CombinedHeader header;
    bool result = read(reinterpret_cast<char *>(&header), sizeof(CombinedHeader)) == sizeof(CombinedHeader);
    if (result) {
        if ((memcmp(&header.riff.descriptor.id, "RIFF", 4) == 0
            || memcmp(&header.riff.descriptor.id, "RIFX", 4) == 0)
            && memcmp(&header.riff.type, "WAVE", 4) == 0
            && memcmp(&header.wave.descriptor.id, "fmt ", 4) == 0
            && (header.wave.audioFormat == 1 || header.wave.audioFormat == 0)) {

            // Read off remaining header information
            DATAHeader dataHeader;

            if (qFromLittleEndian<quint32>(header.wave.descriptor.size) > sizeof(WAVEHeader)) {
                // Extended data available
                quint16 extraFormatBytes;
                if (peek((char*)&extraFormatBytes, sizeof(quint16)) != sizeof(quint16))
                    return false;
                const qint64 throwAwayBytes = sizeof(quint16) + qFromLittleEndian<quint16>(extraFormatBytes);
                if (read(throwAwayBytes).size() != throwAwayBytes)
                    return false;
            }

            if (read((char*)&dataHeader, sizeof(DATAHeader)) != sizeof(DATAHeader))
                return false;

            // Establish format
            if (memcmp(&header.riff.descriptor.id, "RIFF", 4) == 0)
                m_fileFormat.setByteOrder(QAudioFormat::LittleEndian);
            else
                m_fileFormat.setByteOrder(QAudioFormat::BigEndian);

            int bps = qFromLittleEndian<quint16>(header.wave.bitsPerSample);
            m_fileFormat.setChannelCount(qFromLittleEndian<quint16>(header.wave.numChannels));
            m_fileFormat.setCodec("audio/pcm");
            m_fileFormat.setSampleRate(qFromLittleEndian<quint32>(header.wave.sampleRate));
            m_fileFormat.setSampleSize(qFromLittleEndian<quint16>(header.wave.bitsPerSample));
            m_fileFormat.setSampleType(bps == 8 ? QAudioFormat::UnSignedInt : QAudioFormat::SignedInt);
        } else {
            result = false;
        }
    }
    m_headerLength = pos();
    return result;
}


/// http://www.cplusplus.com/forum/beginner/166954/
#include <fstream>
#include <iostream>
using namespace std;

namespace little_endian_io
{
  template <typename Word>
  std::ostream& write_word( std::ostream& outs, Word value, unsigned size = sizeof( Word ) )
  {
    for (; size; --size, value >>= 8)
      outs.put( static_cast <char> (value & 0xFF) );
    return outs;
  }
}
using namespace little_endian_io;

void WavFile::writeNewWav(std::vector<int> &donnees)
{
    ofstream f( "result.wav", ios::binary );
    int val = (m_fileFormat.sampleRate() * m_fileFormat.sampleSize() * m_fileFormat.channelCount()) / 8;

      // Write the file headers
      f << "RIFF----WAVEfmt ";     // (chunk size to be filled in later)
      write_word( f,     16, 4 );  // no extension data
      write_word( f,      1, 2 );  // PCM - integer samples
      write_word( f, m_fileFormat.channelCount(), 2 );  // number of channels
      write_word( f, m_fileFormat.sampleRate(), 4 );  // samples per second (Hz)
      write_word( f,    val, 4 );  // (Sample Rate * BitsPerSample * Channels) / 8
      write_word( f, 2*m_fileFormat.channelCount(), 2 );  // data block size (size of two integer samples, one for each channel, in bytes)
      write_word( f, m_fileFormat.sampleSize(), 2 );  // number of bits per sample (use a multiple of 8)


      // Write the data chunk header
      size_t data_chunk_pos = f.tellp();
      f << "data----";  // (chunk size to be filled in later)

      // recupération du maximum
      for(auto &a : donnees) { write_word(f, a, 2);}

      // (We'll need the final file size to fix the chunk sizes above)
      size_t file_length = f.tellp();

      // Fix the data chunk header to contain the data size
      f.seekp( data_chunk_pos + 4 );
      write_word( f, file_length - data_chunk_pos + 8 );

      // Fix the file header to contain the proper RIFF chunk size, which is (file size - 8) bytes
      f.seekp( 0 + 4 );
      write_word( f, file_length - 8, 4 );
}
