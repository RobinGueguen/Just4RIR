/*/////////////////////////////// INFORMATIONS ///////////////////////////////////////
Software name : Just4RIR
Creation date : November 30th 2018
Last modification : November 30th 2018
Author : Robin Gueguen
License : GPL 3.0 2018
Property : Institut des Sciences du Calcul et des Données - Sorbonne Université
Version : 1.0 beta
Function : Main window
*/////////////////////////////////////////////////////////////////////////////////////

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "objwriter.h"
#include "raytracing.h"
#include "octree.h"
#include <QElapsedTimer>
#include <QMediaPlayer>
#include "plotwindow.h"
#include "data.h"

#include <QBuffer>
#include <QAudioDeviceInfo>
#include <QAudioOutput>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void suppFichier();// Suppression des fichiers d'export existant
    void saveParameters();
    void loadParameters();
    void courbeAtmo();
    void courbeAudioInput();


private slots:

    void on_bouton_rayons_clicked();

    void on_bouton_source_clicked();

    void on_bouton_listener_clicked();

    void on_spinBox_nbRebond_valueChanged(int arg1);

    //void on_checkBox_rebFixe_toggled(bool checked);

    void on_spinBox_attenuation_valueChanged(int arg1);

    void on_bouton_sourcesImages_clicked();

    //void on_radioButton_vertexSource_toggled(bool checked);

    //void on_radioButton_Fibonacci_toggled(bool checked);

    void on_bouton_RIR_clicked();

    void on_checkBox_rayAuto_toggled(bool checked);

    //void on_checkBox_methodeRapide_toggled(bool checked);

    void on_bouton_audioFile_clicked();

    void on_bouton_ecouter_clicked();

    void on_AudioSlider_valueChanged(int value);

    void on_positionChanged(qint64 position);

    void on_durationChanged(qint64 position);

    //void on_spinBox_nbFaceFeuille_editingFinished();

    void on_spinBox_freqEchan_editingFinished();

    void on_spinBox_seuilArret_editingFinished();

   // void slotGetMusicData(QAudioBuffer musicBuffer);

    void on_bouton_convolution_clicked();

    void on_bouton_projection_clicked();


   // void handleStateChanged(QAudio::State newState);

    void on_bouton_saveRir_clicked();

    void on_bouton_diffRir_clicked();

    void on_bouton_data_clicked();

    void on_spinBox_gain_editingFinished();

    void on_spinBox_numListener_editingFinished();

    void on_spinBox_nbRay_editingFinished();

    void on_doubleSpinBox_siTps_editingFinished();

    void on_doubleSpinBox_siTps_erreur_editingFinished();

    void on_spinBox_numSource_editingFinished();

    void on_actionInfo_triggered();

    void on_radioButton_rebFixe_clicked();

    void on_radioButton_seuildB_clicked();

    void on_radioButton_Fibonacci_clicked();

    void on_radioButton_vertexSource_clicked();

    void on_spinBox_temperature_valueChanged(int arg1);

    void on_spinBox_humidite_valueChanged(int arg1);

    void on_radioButton_tpsSI_clicked();

    void on_radioButton_initSource_clicked();


    void on_actionUser_manual_triggered();

private:
    Ui::MainWindow *ui;
    MeshObj m_meshObj;
    Source m_source;
    //Listener m_listener;
    std::vector<Listener> m_listener;
    //SourceImage m_sourceImage;
    std::vector<SourceImage> m_sourceImage;
    Octree m_octree;
    QString m_fichierExport, m_fichierAudio, m_audioInput, m_fichierObj;
    float m_seuilAttenuation, m_gain, m_tpsDiff;
    int m_temperature, m_humidite, m_nbRebond, m_freq, m_nbRayon, m_seuilArret, m_nbFaceFeuille, m_numListener, m_numSource;
    bool m_fibonacci, m_rayAuto, m_methodeRapide, m_nbRebondFixe, m_rayFromSI;
    QMediaPlayer *player;
    std::vector<float> m_longueurRayMax, m_vectWav;

    plotWindow *plot, *audioPlot, *audioPlot2, *firPlot, *filtrePlot, *plotcumsum;
    Data *fenetre;

    QFile raw;

    QLabel *text, *text2;

};

// Autres methodes
void supprLayoutContent(QGridLayout *layout);

#endif // MAINWINDOW_H
