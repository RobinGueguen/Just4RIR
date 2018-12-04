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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"
#include "math.h"
#include "rir.h"
#include <QProgressDialog>
#include "audio.h"
#include "physic.h"
#include "fftext.h"
#include <QVector>
#include <QDesktopServices>
#include <QUrl>


void MainWindow::courbeAtmo()
{
    std::vector<float> m = absair(m_temperature, m_humidite);
    QVector<double> x,y;
    for (int i=0; i<8; i++)
    {
        x.push_back(62.5*pow(2,i));
        y.push_back((double)m[i]);
    }

    if(ui->absPlot->removeGraph(0)) ui->absPlot->addGraph();
    ui->absPlot->graph(0)->setData(x,y);
    ui->absPlot->replot();
}

void MainWindow::saveParameters()
{
    QSettings setting("RobinG", "just4RIR");
    setting.beginGroup("MainWindow");

    setting.setValue("temperature", m_temperature);
    setting.setValue("humidite", m_humidite);
    setting.setValue("nbRebond", m_nbRebond);
    setting.setValue("freq", m_freq);
    setting.setValue("nbRayon", m_nbRayon);
    setting.setValue("seuilArret", m_seuilArret);
    setting.setValue("seuilAttenuation", (int)(-10*log10(m_seuilAttenuation)));
    setting.setValue("gain", m_gain);
    setting.setValue("numListener", m_numListener+1);
    setting.setValue("numSource", m_numSource+1);
    setting.setValue("merge", ui->doubleSpinBox_regrouperSI->value());

    setting.setValue("fibonacci", m_fibonacci);
    setting.setValue("rayAuto", m_rayAuto);
    setting.setValue("nbRebondFixe", m_nbRebondFixe);
    setting.setValue("rayFromSI", m_rayFromSI);
    setting.setValue("audioInput",m_audioInput);
    setting.setValue("fichierObj",m_fichierObj);

    setting.setValue("tab", ui->tabWidget->currentIndex());
    setting.setValue("toolbox", ui->toolBox->currentIndex());

    setting.endGroup();
}

void MainWindow::loadParameters()
{
    //Courbe du coefficient m
    ui->absPlot->addGraph();
    ui->absPlot->xAxis->setRange(62.5, 8000);
    ui->absPlot->yAxis->setRange(0, 0.06);
    ui->absPlot->xAxis->setLabel("Frequency (Hz)");
    ui->absPlot->yAxis->setLabel("Air absorption (m-1)");


    // Chargement des derniers pamaretres sauvegardés dans l'iHM
    QSettings setting("RobinG", "just4RIR");
    setting.beginGroup("MainWindow");

    ui->spinBox_temperature->setValue(setting.value("temperature", ui->spinBox_temperature->value()).toInt());
    ui->spinBox_humidite->setValue(setting.value("humidite", ui->spinBox_humidite->value()).toInt());
    ui->spinBox_nbRebond->setValue(setting.value("nbRebond", ui->spinBox_nbRebond->value()).toInt());
    ui->spinBox_freqEchan->setValue(setting.value("freq", ui->spinBox_freqEchan->value()).toInt());
    ui->spinBox_nbRay->setValue(setting.value("nbRayon", ui->spinBox_nbRay->value()).toInt());
    ui->spinBox_seuilArret->setValue(setting.value("seuilArret", ui->spinBox_seuilArret->value()).toInt());
    ui->spinBox_attenuation->setValue(setting.value("seuilAttenuation", ui->spinBox_attenuation->value()).toInt());
    ui->spinBox_gain->setValue(setting.value("gain", ui->spinBox_gain->value()).toInt());
    ui->spinBox_numListener->setValue(setting.value("numListener", ui->spinBox_numListener->value()).toInt());
    ui->spinBox_numSource->setValue(setting.value("numSource", ui->spinBox_numSource->value()).toInt());
    ui->doubleSpinBox_regrouperSI->setValue(setting.value("merge", ui->doubleSpinBox_regrouperSI->value()).toDouble());

    ui->radioButton_Fibonacci->setChecked(setting.value("fibonacci", ui->radioButton_Fibonacci->isChecked()).toBool());
    ui->checkBox_rayAuto->setChecked(setting.value("rayAuto", ui->checkBox_rayAuto->isChecked()).toBool());
    ui->radioButton_rebFixe->setChecked(setting.value("nbRebondFixe", ui->radioButton_rebFixe->isChecked()).toBool());
    ui->radioButton_tpsSI->setChecked(setting.value("rayFromSI", ui->radioButton_tpsSI->isChecked()).toBool());
    m_fichierAudio = setting.value("audioInput", QDir::homePath()).toString();
    m_fichierObj = setting.value("fichierObj", QCoreApplication::applicationDirPath() + "/mesh4RIR.obj").toString();
    m_audioInput = m_fichierAudio;

    ui->tabWidget->setCurrentIndex(setting.value("tab",ui->tabWidget->currentIndex()).toInt());
    ui->toolBox->setCurrentIndex(setting.value("toolbox",ui->toolBox->currentIndex()).toInt());

    setting.endGroup();// les parametres par defaut sont ceux implementés dans le designer

    //m_fichierObj = QCoreApplication::applicationDirPath() + "/mesh4RIR.obj";
    m_meshObj.charger_obj(m_fichierObj);
    m_source = m_meshObj.getSource();
    m_listener = m_meshObj.getListener();
    m_fichierObj = m_meshObj.getfilenmae();

    // Attribus
   m_nbFaceFeuille = 5; // nombre de faces maximum dans les feuilles de l'octree
   m_temperature = ui->spinBox_temperature->value();
   m_humidite = ui->spinBox_humidite->value();
   m_nbRebond = ui->spinBox_nbRebond->value();
   m_freq = ui->spinBox_freqEchan->value();
   m_nbRayon = ui->spinBox_nbRay->value();
   m_seuilArret = ui->spinBox_seuilArret->value();
   if(ui->radioButton_rebFixe->isChecked()) on_radioButton_rebFixe_clicked();
   else on_radioButton_seuildB_clicked();
   if(ui->radioButton_Fibonacci->isChecked()) on_radioButton_Fibonacci_clicked();
   else on_radioButton_vertexSource_clicked();
   if(ui->radioButton_tpsSI->isChecked()) on_radioButton_tpsSI_clicked();
   else on_radioButton_initSource_clicked();

   m_seuilAttenuation = pow(10,(-(float)(ui->spinBox_attenuation->value()/10)));
   m_gain = ui->spinBox_gain->value();
   m_numListener = ui->spinBox_numListener->value()-1;
   m_numSource = ui->spinBox_numSource->value()-1;

   m_fichierExport = QCoreApplication::applicationDirPath() + "/mesh4RIR_EXPORT.obj";
   on_checkBox_rayAuto_toggled(ui->checkBox_rayAuto->isChecked());
   QStringList list = m_fichierAudio.split("/");
   ui->label_fichierAudio->setText(list[list.size()-1]);
   ui->inputAudio->addGraph();
   courbeAudioInput();
   courbeAtmo();


   // OCTREE
   if (m_meshObj.getVert().size() > 200) {
       m_methodeRapide = true;
       m_octree = Octree(m_meshObj,m_nbFaceFeuille);
   }
   else m_methodeRapide = false;

}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)//, m_meshObj(QCoreApplication::applicationDirPath() + "/mesh4RIR.obj"), m_source(m_meshObj.getSource()), m_listener(m_meshObj.getListener())
{
   // AFFICHAGE FENETRE
   ui->setupUi(this);


   // CHARGEMENT PARAMETRES
   loadParameters();

   //AFFICHAGE DES SOURCES
   for(int i = 0 ; i< m_source.getNbSource(); i++)
   {
       text = new QLabel;
       text2 = new QLabel;
       text->setText("Source" + QString::number(i));
       ui->gridLayout_4->addWidget(text,i,0, Qt::AlignLeft);
       text2->setText("Center (m) : "+CoordVector2QString2(m_source.getCentre(i)));
       ui->gridLayout_4->addWidget(text2,i,1, Qt::AlignLeft);
   }
   ui->spinBox_numSource->setMaximum(m_source.getNbSource());

   // AFFICHAGE DES LISTENERS
   for(int i = 0 ; i< m_listener.size() ; i++)
   {
       text = new QLabel;
       text2 = new QLabel;
       text->setText("Listener" + QString::number(i));
       ui->gridLayout_8->addWidget(text,i,0, Qt::AlignLeft);
       text2->setText(m_listener[i].afficher());
       ui->gridLayout_8->addWidget(text2,i,1, Qt::AlignLeft);
   }
   ui->spinBox_numListener->setMaximum(m_listener.size());
   on_spinBox_numListener_editingFinished();
   ui->lcdNumber_ListenerRadius->display(m_listener[m_numListener].getRayon());






   // PLAYER
   player = new QMediaPlayer(this);
   connect(player, &QMediaPlayer::positionChanged, this, &MainWindow::on_positionChanged);
   connect(player, &QMediaPlayer::durationChanged, this, &MainWindow::on_durationChanged);
}


MainWindow::~MainWindow()
{
    saveParameters();
    delete ui;
}

void MainWindow::suppFichier()
{
    // Suppression des fichiers d'export existant
    QDir repertoire(QCoreApplication::applicationDirPath()); // chemin de type QDir
    QFileInfoList files = repertoire.entryInfoList(); //Liste des fichiers dans le répertoire contenant l'executable
    foreach (QFileInfo file, files)
    {
        if (file.baseName().contains("EXPORT")) {
         QFile fichier(file.absoluteFilePath());
         if(!fichier.remove())
             QMessageBox::critical(NULL,"Error","Impossible to delete the file !");
        }
    }
}



////// LES BOUTONS

void MainWindow::on_bouton_source_clicked()
{
    // IMPORT
    QString fichierObj = QCoreApplication::applicationDirPath() + "/src4RIR.obj";
    MeshObj monMeshObj;
    monMeshObj.charger_obj(fichierObj);
    m_source = monMeshObj.getSource();

    // Suppression du texte
    supprLayoutContent(ui->gridLayout_4);

    for(int i = 0 ; i< m_source.getNbSource(); i++)
    {
        text = new QLabel;
        text2 = new QLabel;
        text->setText("Source" + QString::number(i+1));
        ui->gridLayout_4->addWidget(text,i,0, Qt::AlignLeft);
        text2->setText(CoordVector2QString2(m_source.getCentre(i)));
        ui->gridLayout_4->addWidget(text2,i,1, Qt::AlignLeft);
    }
}

void MainWindow::on_bouton_listener_clicked()
{
    // IMPORT
   QString fichierObj = QCoreApplication::applicationDirPath() + "/listener4RIR.obj";
   MeshObj monMeshObj;
   monMeshObj.charger_obj(fichierObj);
    m_listener = monMeshObj.getListener();

    // Suppression du texte
    supprLayoutContent(ui->gridLayout_8);

    for(int i = 0 ; i< m_listener.size() ; i++)
    {
        text = new QLabel;
        text2 = new QLabel;
        text->setText("Listener" + QString::number(i));
        ui->gridLayout_8->addWidget(text,i,0, Qt::AlignLeft);
        text2->setText(m_listener[i].afficher());
        ui->gridLayout_8->addWidget(text2,i,1, Qt::AlignLeft);
    }

    on_checkBox_rayAuto_toggled(m_rayAuto);
    ui->spinBox_numListener->setMaximum(m_listener.size());
    ui->lcdNumber_ListenerRadius->display(m_listener[m_numListener].getRayon());
}



void MainWindow::on_bouton_rayons_clicked() // Affichage de rayons
{
    suppFichier(); // Suppression des fichiers d'export existant

    // Ouvrir fenetre de progress bar
    QProgressDialog progress(this);
        progress.setWindowModality(Qt::WindowModal);
        progress.setLabelText("working...");
        progress.setMinimumDuration(0);
        progress.show();

    // lancer le timer
    QElapsedTimer timer;
    timer.start();

    if (!m_rayFromSI) // tracé des rayons depuis la/les source(s) initiale(s)
    {
        int ns, time;

        for (int nSrc = 0; nSrc < m_source.getNbSource() ; nSrc++)
        {
            time = m_source.getNbSource();
            if (m_numSource >= 0) // Cas d'un calcul monosource
            {
                ns = m_numSource;
                time=1;
            }
            else ns = nSrc;

            progress.setLabelText("Source " + QString::number(ns+1) + " - working...");
            progress.setRange(0,m_nbRebond*time);

            // RAYONS
            Ray monRay(m_nbRayon, m_source, ns, m_fibonacci);
            if(!m_fibonacci) m_nbRayon = monRay.getNbRay(); // Au cas où on prend la source blender


            // EXPORT
            ObjWriter monObjWriter(m_fichierExport, m_nbRayon);
            if (m_methodeRapide) m_octree.chargerRayonRacine(m_nbRayon);


            if (m_nbRebondFixe)
            {          
                //Méthode d'affichage incrémentale
                for (int i =0; i<m_nbRebond ; i++)
                {
                    progress.setValue(i+m_nbRebond*nSrc);
                    if (progress.wasCanceled()) {
                        i=m_nbRebond;
                        nSrc = m_source.getNbSource();
                    }
                    if (m_methodeRapide)
                    {
                        m_octree.chargerRayon(monRay.getRay(), monRay.getvDir(), monRay.getRayVivant());
                        if(!monRay.rebondSansMemoire(m_meshObj, m_octree)) // calcul des points d'intersection entre rayons et faces
                                i=m_nbRebond; // arrete la boucle si tous les rayons sont morts
                    }
                    else
                    {
                        if(!monRay.rebondSansMemoire(m_meshObj))
                                i=m_nbRebond;
                    }
                    monObjWriter.rec_Vert(m_source,ns,monRay, i); // ecriture des vertex

                }
                monObjWriter.rec_Line(m_nbRayon); // ecriture des edges entre les vertex
                progress.setValue(m_nbRebond*time);
            }
           else QMessageBox::warning(NULL,"Beware","Please use fixed iteration mode (configuration->advanced)");
            if (m_numSource >= 0) break; // Cas d'un calcul monosource
        }
    }
    else // tracé des rayons depuis les sources images
    {
        if(m_sourceImage.empty()) on_bouton_sourcesImages_clicked();

        std::vector<CoordVector> si = m_sourceImage[m_numListener].getSourcesImages();
        std::vector<float> longueurRayon;
        std::vector<float> SItps=m_sourceImage[m_numListener].getRaySITps();
        std::vector<CoordVector> point;
        std::vector<CoordVector> dir;
        float tps;

        float sitps = ui->doubleSpinBox_siTps->value();
        float sitpserror = ui->doubleSpinBox_siTps_erreur->value();
        std::vector<int> indices;
        for(int i=0; i<si.size(); i++)
        {
            if(sitps < SItps[i] && SItps[i] < sitpserror)
            {
                point.push_back(m_sourceImage[m_numListener].getRaySI()[i]);
                dir.push_back(m_sourceImage[m_numListener].getRaySIvec()[i]);
                longueurRayon.push_back(m_sourceImage[m_numListener].getRaySIlong()[i]);
                tps = m_sourceImage[m_numListener].getRaySITps()[i];
                indices.push_back(i);
            }
        }
        if(point.empty()) // si aucune source image dans la plage indiquée
        {
            std::vector<float> ecart;
            for(int i=0; i<si.size(); i++) // on mesure l'écart avec la borne inférieure pour obtenir la source image la plus proche
            {
                ecart.push_back(fabs(sitps-SItps[i]));
            }
            int ind = std::distance(ecart.begin(), std::min_element(ecart.begin(), ecart.end()));
            point.push_back(m_sourceImage[m_numListener].getRaySI()[ind]);
            dir.push_back(m_sourceImage[m_numListener].getRaySIvec()[ind]);
            longueurRayon.push_back(m_sourceImage[m_numListener].getRaySIlong()[ind]);
            tps = m_sourceImage[m_numListener].getRaySITps()[ind];

            QString txt = "Temps : " + QString::number(tps) + "\n";
            txt+=CoordVector2QString2(si[ind])+"\n";
            QMessageBox::information(NULL,"The closest image-source found",txt);
        }
        else
        {
            QString txt;
            for (int i=0; i<indices.size(); i++)
            {
                txt+=CoordVector2QString2(si[indices[i]])+"\n";
            }
            QMessageBox::information(NULL,"Image-source coordinate",txt);
        }

        //EXPORT
        ObjWriter monObjWriter(m_fichierExport, point.size());
        Ray monRay(point,dir);

        monObjWriter.rec_Vert_init(point);

        std::vector<float> dist, lg;

        if (m_methodeRapide)
        {
            m_octree.chargerRayonRacine(monRay.getNbRay());
            m_octree.chargerRayon(monRay.getRay(), monRay.getvDir(), monRay.getRayVivant2());

            while(monRay.rebondSansMemoire(m_meshObj, m_octree)) // TANT QUE TOUS LES RAYONS NE SONT PAS MORTS
            {
                progress.setValue(monRay.getRayMorts());
                if (progress.wasCanceled()) break; // arrete la boucle

                monObjWriter.rec_Vert(monRay);

                dist = monRay.getDist();
                lg = monRay.getLong();
                for(int i = 0;i<longueurRayon.size(); i++)
                {
                    if (dist[i]+lg[i]>longueurRayon[i])
                    {
                        monRay.killRay(i);
                        monObjWriter.rec_Vert(monRay, i, m_source.getCentre(0));
                    }
                }                
                m_octree.chargerRayon(monRay.getRay(), monRay.getvDir(), monRay.getRayVivant2());
            }
        }
        else
        {
            while(monRay.rebondSansMemoire(m_meshObj))
            {
                progress.setValue(monRay.getRayMorts());
                if (progress.wasCanceled()) break; // arrete la boucle


                monObjWriter.rec_Vert(monRay); // on inscrit les autres rayons

                dist = monRay.getDist();
                lg = monRay.getLong();
                for(int i = 0;i<longueurRayon.size(); i++) // on incrit tous les rayons morts
                {
                    if (dist[i]+lg[i]>longueurRayon[i])
                    {
                        monRay.killRay(i);
                        monObjWriter.rec_Vert(monRay, i, m_source.getCentre(0));
                    }
                }
            }
        }
    }

    double temps = timer.elapsed();
    temps = temps /1000;
    ui->lcd_timer->display(temps);

    progress.cancel();
}



void MainWindow::on_bouton_sourcesImages_clicked() // Calcul des positions et énergies des sources images pour chaque source et chaque listener
{
    suppFichier(); // Suppression des fichiers d'export existant

    std::vector<float> absAir = absair(m_temperature, m_humidite);

    // OCTREE
    if (m_methodeRapide) m_octree.chargerRayonRacine(m_nbRayon);

    //SOURCES IMAGES
    SourceImage SI;
    std::vector<SourceImage> maSourceImage;
    maSourceImage.resize(m_listener.size(), SI);


    // Ouvrir fenetre de progress bar
    QProgressDialog progress(this);
        progress.setWindowModality(Qt::WindowModal);
        progress.setLabelText("working...");
        progress.setRange(0,0);
        progress.setMinimumDuration(0);
        progress.show();

    // lancer le timer
    QElapsedTimer timer;
    timer.start();
    //m_timer.start();
    int i, j, ns, time(m_source.getNbSource());

    for (int nSrc = 0; nSrc < m_source.getNbSource() ; nSrc++)
    {

        if (m_numSource >= 0) // Cas d'un calcul monosource
        {
            ns = m_numSource;
            time=1;
        }
        else ns = nSrc;

        progress.setLabelText("Source " + QString::number(ns+1) + " - working...");


        // RAYONS
        Ray monRay(m_nbRayon, m_source, ns, m_fibonacci);
        if(!m_fibonacci) m_nbRayon = monRay.getNbRay(); // Au cas où on prend la source blender

        if (m_nbRebondFixe)
        {
            progress.setRange(0,m_nbRebond*time);

            //Méthode d'affichage incrémentale
            for (i =0; i<m_nbRebond ; i++)
            {
                progress.setValue(i+m_nbRebond*nSrc);
                if (progress.wasCanceled()) {
                    i=m_nbRebond;
                    nSrc=m_source.getNbSource();
                }

                if (m_methodeRapide)
                {
                    m_octree.chargerRayon(monRay.getRay(), monRay.getvDir(), monRay.getRayVivant());
                    if (!monRay.rebondSansMemoire(m_meshObj, m_octree)) i=m_nbRebond;
                }
                else
                {
                    if (!monRay.rebondSansMemoire(m_meshObj)) i=m_nbRebond;
                }
                for(j = 0; j < m_listener.size() ; j++)
                {
                   maSourceImage[j].addSourcesImages(monRay , m_listener[j], 1e6, absAir,-1 );
                }
            }
            progress.setValue(m_nbRebond*time);
        }
        else
        {
            progress.setRange(0,m_nbRayon*time);
            progress.setValue(1);

            bool seuilNonAtteind = true;

            while (seuilNonAtteind)
            {
                progress.setValue(monRay.getRayMorts()+m_nbRayon*nSrc);
                if (progress.wasCanceled())
                {
                    nSrc = m_source.getNbSource();
                    break; // arrete la boucle
                }
                if (m_methodeRapide)
                {
                    m_octree.chargerRayon(monRay.getRay(), monRay.getvDir(), monRay.getRayVivant());
                    monRay.rebondSansMemoire(m_meshObj, m_octree);// en plus
                }
                else
                {
                    monRay.rebondSansMemoire(m_meshObj);
                }

                seuilNonAtteind = false;
                for( j = 0; j < m_listener.size() ; j++)
                {
                   if (maSourceImage[j].addSourcesImages(monRay , m_listener[j], m_longueurRayMax[j], absAir,m_seuilAttenuation ))
                       seuilNonAtteind = true;
                }
            }
            progress.setValue(m_nbRayon*time);
        }
        if (m_numSource >= 0) // Cas d'un calcul monosource
        {
            nSrc = m_source.getNbSource(); // la boucle n'a lieu qu'une fois
        }
    }

    double temps = timer.elapsed();
    temps = temps /1000;
    ui->lcd_timer->display(temps);

    progress.cancel();

    m_sourceImage = maSourceImage;

}


void MainWindow::on_bouton_projection_clicked()
{
    if (m_sourceImage.empty()) on_bouton_sourcesImages_clicked();
    else
    {
        std::vector<CoordVector> SI = m_sourceImage[m_numListener].getSourcesImages();
        std::vector<CoordVector> SI2;
        std::vector<float> nrg = m_sourceImage[m_numListener].getNrgSI();
        std::vector<float> nrg2;
        int seuil = ui->doubleSpinBox_regrouperSI->value();

        unite(SI,SI2,nrg, nrg2,seuil); // unicité des sources images

        if (SI2.size() < 2) QMessageBox::warning(NULL,"Beware","Not enougth image-sources");
        else
        {
            //EXPORT
            suppFichier(); // Suppression des fichiers d'export existant
            ObjWriter monObjWriter(m_fichierExport, SI2.size());
            if (ui->checkBox_projeter->isChecked())
            {
                // création des impacts
                Ray monRay(m_listener[m_numListener].getCentre(), SI2);
                if (m_methodeRapide)
                {
                    m_octree.chargerRayonRacine(monRay.getNbRay());
                    m_octree.chargerRayon(monRay.getRay(), monRay.getvDir(), monRay.getRayVivant());
                    monRay.rebondSansMemoire(m_meshObj,m_octree);
                }
                else monRay.rebondSansMemoire(m_meshObj);

                monObjWriter.display_coloredTriangle(monRay.getRay(),nrg2, m_listener[m_numListener].getCentre(), m_source.getCentre(), m_seuilAttenuation);
            }
            else
                monObjWriter.display_coloredTriangle(SI2, nrg2, m_listener[m_numListener].getCentre(), m_source.getCentre(), m_seuilAttenuation);
        }
        for(auto &a : SI2) a.debug();
    }
}

void MainWindow::on_bouton_RIR_clicked()
{
    // ouvre une nouvelle fenetre
    plotWindow *plot = new plotWindow;
    std::vector<float> absR = absair(m_temperature, m_humidite);
    if (m_sourceImage[m_numListener].calculerRIR(m_freq, absR, m_gain, ui->checkBox_decaycurve->isChecked(), m_seuilAttenuation))
    {
        // recupération du min
        std::vector<double> y = m_sourceImage[m_numListener].getY()[0];
        double max = *std::max_element(y.begin(), y.end());

        plot->XY(m_sourceImage[m_numListener].getX(), m_sourceImage[m_numListener].getY(), max*m_seuilAttenuation);
        plot->makePlot();
        plot->setYLabel("SPL (dB)");
        plot->setTitle(" Source position : " + CoordVector2QString2(m_source.getCentre())+
                                            "\n Listener position : " + CoordVector2QString2(m_listener[m_numListener].getCentre()));
        QPoint position = QApplication::desktop()->screenGeometry().topLeft();
        plot->move(position);
        plot->show();

    }
    else QMessageBox::warning(NULL,"Beware","Direct sound only");
}


void MainWindow::on_bouton_saveRir_clicked()
{

    std::vector<std::vector<double> > rir = m_sourceImage[m_numListener].getY();
    if(!rir.empty())
    {
        QString chemin = QCoreApplication::applicationDirPath() + "/rirSauvegardee.txt";
        QFile fichier(chemin);

         // sauvegarder rir
        fichier.open(QIODevice::WriteOnly | QIODevice::Text); // ouvre le fichier
        QString text;
        float freq = 62.5/2;

        for(auto &a : rir)
            {
            freq*=2;
            text = QString::number(freq) + "\n";
            fichier.write(text.toLatin1());
            text = "";
                for(auto &b : a)
                {
                    text += QString::number(b) + " ";
                }
                text += "\n";
                fichier.write(text.toLatin1());
            }
         fichier.close(); // ferme le fichier
    }
    else QMessageBox::warning(NULL,"Beware","Direct sound only");
}


void MainWindow::on_bouton_diffRir_clicked()
{
     // différence de rir
     std::vector<std::vector<double> > diff_rir = m_sourceImage[m_numListener].getY();
     if(!diff_rir.empty())
     {
         QString chemin = QCoreApplication::applicationDirPath() + "/rirSauvegardee.txt";
         QFile fichier(chemin);

         float i(0), j;
         float min;
         QStringList valString;
         QString ligne;
         std::vector<float> val;
         std::vector<std::vector<double> > resultat;
         resultat.resize(8);
         if(fichier.open(QIODevice::ReadOnly | QIODevice::Text)) // Si on peut ouvrir le fichier
         {
             QTextStream flux(&fichier);
             while(!flux.atEnd())
             {
                 ligne = flux.readLine(); // Lecture ligne par ligne du fichier

                 valString = ligne.split(" ");
                 val.clear();
                 for(auto &a : valString) val.push_back(a.toFloat()); // récupération des valeurs d'une ligne en float
                 if (val[0] < 2 ) // on ne lit pas les lignes de titre
                 {
                     min = std::min(val.size(), diff_rir[i].size());
                     for (j = 0; j<min; j++)
                     {
                         resultat[i].push_back(std::abs(diff_rir[i][j] - val[j]));
                     }
                     i++;
                 }
             }
         }
         fichier.close();

         std::vector<float> x;
         std::vector<float> SIx = m_sourceImage[m_numListener].getX();
         for (j = 0; j<min; j++)
             x.push_back(SIx[j]);

        plotWindow *plot = new plotWindow;
         plot->XY(x, resultat, m_seuilAttenuation);
         plot->makePlot();
         plot->show();
     }
     else QMessageBox::warning(NULL,"Beware","Direct sound only");


}

void MainWindow::on_bouton_data_clicked()
{
    std::vector<float> absR = absair(m_temperature, m_humidite);
    for(int l=0; l < m_listener.size() ; l++)
    {
        if(m_sourceImage[l].getY().empty()) m_sourceImage[l].calculerRIR(m_freq, absR, m_gain, ui->checkBox_decaycurve->isChecked(), m_seuilAttenuation);
        std::vector<std::vector<double> > rir = m_sourceImage[l].getY();
        std::vector<std::vector<double> > curve = m_sourceImage[l].getCurve();
        std::vector<CoordVector> si = m_sourceImage[l].getSourcesImages();
        std::vector<float> nrgSi = m_sourceImage[l].getNrgSI();

        CoordVector vect1 = vecteur(m_listener[l].getCentre(),m_source.getCentre());
        // vecteur normal dans le plan xy.
        float buf=  vect1.x;
        vect1.x = -vect1.z;
        vect1.z= buf;
        vect1=vect1/norme(vect1);

        float cosBeta, nor;
        CoordVector vect2;

        std::vector<float> maxY;
        for(auto &a : curve) maxY.push_back(*std::max_element(a.begin(), a.end()));

        std::vector<float> maxY2;
        for(auto &a : rir) maxY2.push_back(*std::max_element(a.begin(), a.end()));

            float i(0), j(0);
            std::vector<float> Tr60, Tr30, Tr30_buf, C80, D50, Ts, EDT, SPL, LF, G;
            float C80_0, C80_1, D50_0, Ts_buf, LF_buf, LF_tot, Tr60_buf, EDT_buf;
            // determination du son direct
            int sonDirect=0; // numéro de l'échantilllon
            for (j=0; j<rir[0].size(); j++)
            {
                if (rir[0][j]>0)
                {
                    sonDirect = j;
                    break;
                }
            }
            for (i=0; i<8; i++) // pour chaque bande
            {
                for (j=sonDirect; j<rir[0].size(); j++) // Pour chaque échantillon à partir du son direct
                {
                    if(curve[i][j] >= maxY[i]*1e-6) Tr60_buf = (j-sonDirect)/m_freq*1000; // on conserve le dernier terme

                    if (curve[i][j] <= maxY[i]*pow(10,-0.5) && curve[i][j] >= maxY[i]*pow(10,-3.5)) Tr30_buf.push_back((j-sonDirect)/m_freq*1000);

                    if(curve[i][j] >= maxY[i]*0.1) EDT_buf = (j-sonDirect)/m_freq*1000;

                    if(j <= 80*m_freq/1000 + sonDirect) C80_0 += rir[i][j];
                    else C80_1 += rir[i][j];

                    if(j <= 50*m_freq/1000 + sonDirect) D50_0 += rir[i][j];

                    if (j>=sonDirect)
                    Ts_buf += ((j-sonDirect)/m_freq*1000)*rir[i][j];

                }

                //RT 60
                Tr60.push_back(Tr60_buf);

                // temps pour passer de -5dB à -35dB
                Tr30.push_back(2*(*std::max_element(Tr30_buf.begin(), Tr30_buf.end())-*std::min_element(Tr30_buf.begin(), Tr30_buf.end())));
                Tr30_buf.clear();

                // temps où l'énergie descend sous 10dB
                EDT.push_back(EDT_buf*6);


                // ratio avant et après 80ms
                C80.push_back(10*log10(C80_0/C80_1));
                C80_0=0;
                C80_1=0;

                // ratio avant 50ms sur total
                D50.push_back(100*D50_0/maxY[i]);
                D50_0=0;

                // ratio temps central
                Ts.push_back(Ts_buf/maxY[i]);
                Ts_buf = 0;

                // spl
                SPL.push_back(10*log10(maxY[i])-m_gain);

                // Gain
                G.push_back(10*log10(maxY[i]/maxY2[i]));

                // LF somme des energie multiplié par l'angle
                for (j=0; j<si.size(); j++)
                {
                    vect2 = vecteur(m_listener[l].getCentre(), si[j]);
                    nor = norme(vect2);
                    if (1000*nor/VITESSE_SON < (80 + 1000*sonDirect/m_freq) && nor/VITESSE_SON > sonDirect/m_freq)
                    {
                        LF_tot += nrgSi[8*j+i];
                        if (1000*nor/VITESSE_SON > (5 + 1000*sonDirect/m_freq) && nor/VITESSE_SON > sonDirect/m_freq)
                        {
                            cosBeta = produitScalaire(vect1, vect2)/nor;
                            LF_buf += nrgSi[8*j+i]*pow(cosBeta,2);
                        }
                    }
                }
                LF.push_back(LF_buf/LF_tot);
                LF_buf=0;
                LF_tot=0;


            }
            if(m_listener.size()<2)
            {
                // Afficher valeur
                fenetre = new Data;
                for ( i=0; i<8; i++)
                {
                    fenetre->addValue(QString::number(EDT[i])   +"ms",  1, i+1);
                    fenetre->addValue(QString::number(Tr30[i])  +"ms",  2, i+1);
                    fenetre->addValue(QString::number(Tr60[i])  +"ms",  3, i+1);
                    fenetre->addValue(QString::number(SPL[i])   +"dB",  4, i+1);
                    fenetre->addValue(QString::number(G[i])     +"dB",  5, i+1);
                    fenetre->addValue(QString::number(C80[i])   +"dB",  6, i+1);
                    fenetre->addValue(QString::number(D50[i])   +"%",   7, i+1);
                    fenetre->addValue(QString::number(Ts[i])    +"ms",  8, i+1);
                    fenetre->addValue(QString::number(LF[i])    +"dB",  9, i+1);
                }
                fenetre->move((QApplication::desktop()->screenGeometry().bottomLeft()+QApplication::desktop()->screenGeometry().bottomRight())/2);
                fenetre->show();
            }

            // Exporter tableau
            QString newName(QCoreApplication::applicationDirPath() + "/data.txt");
            QFile fichier(newName);
             i=0;
            while(fichier.exists()) // incrementation de version de fichier s'il existe deja
            {
                QStringList nom = fichier.fileName().split(".txt");
                if (nom[0].contains("data_"))
                {
                    QStringList nom2 = nom[0].split("data_");
                    newName = nom2[0] + "data_" + QString::number(i) + ".txt" ;
                }
                else
                {
                    newName = nom[0] + "_" + QString::number(i) + ".txt" ;
                }

                i++;
                fichier.setFileName(newName);
            }

            CoordVector s(m_source.getCentre(m_numSource)), r(m_listener[l].getCentre());
            arrondir(s,2);
            arrondir(r,2);

            fichier.open(QIODevice::WriteOnly | QIODevice::Text); // ouvre le fichier
            QString text;
            text = "Source ,";
            text+=QString::number(s.x) + " ; " + QString::number(-s.z) + " ; " + QString::number(s.y);
            text+="\nListener ,";
            text+=QString::number(r.x) + " ; " + QString::number(-r.z) + " ; " + QString::number(r.y);
            text+="\nNumber of rays ,";
            text+= QString::number(m_nbRayon);
            text+= "\n\n";
            text += "Factors , 62.5Hz , 125Hz , 250Hz , 500Hz , 1kHz , 2kHz , 4kHz , 8kHz\n";
            fichier.write(text.toLatin1());

            text = "EDT (ms)";
            for (i=0; i<8; i++) text+= ", " + QString::number(round(EDT[i]));
            text+= "\n";
            fichier.write(text.toLatin1());
            text = "T30 (ms)";
            for (i=0; i<8; i++) text+= ", " + QString::number(round(Tr30[i]));
            text+= "\n";
            fichier.write(text.toLatin1());
            text = "RT60 (ms)";
            for (i=0; i<8; i++) text+= ", " + QString::number(round(Tr60[i]));
            text+= "\n";
            fichier.write(text.toLatin1());
            text = "spl (dB)";
            for (i=0; i<8; i++) text+= ", " + QString::number(round(SPL[i]));
            text+= "\n";
            fichier.write(text.toLatin1());
            text = "G (dB)";
            for (i=0; i<8; i++) text+= ", " + QString::number(round(G[i]*10)/10);
            text+= "\n";
            fichier.write(text.toLatin1());
            text = "C80 (dB)";
            for (i=0; i<8; i++) text+= ", " + QString::number(round(C80[i]*100)/100);
            text+= "\n";
            fichier.write(text.toLatin1());
            text = "D50 (\\%)";
            for (i=0; i<8; i++) text+= ", " + QString::number(round(D50[i]*100)/100);
            text+= "\n";
            fichier.write(text.toLatin1());
            text = "Ts (ms)";
            for (i=0; i<8; i++) text+= ", " + QString::number(round(Ts[i]));
            text+= "\n";
            fichier.write(text.toLatin1());
            text = "LF80 (dB)";
            for (i=0; i<8; i++) text+= ", " + QString::number(round(LF[i]*1000)/1000);
            text+= "\n";
            fichier.write(text.toLatin1());
    }
}

void MainWindow::courbeAudioInput(){

   WavFile wav;
    if(wav.open(m_fichierAudio))
    {
        QByteArray donnees = wav.readAll();
        m_freq = wav.fileFormat().sampleRate();
        ui->spinBox_freqEchan->setValue(m_freq);

        const char * data=donnees.constData(); // retourne le pointeur d'accés aux données
        const qint16 * datai=reinterpret_cast<const qint16 *>(data);
        int len=donnees.size()/(sizeof(qint16));
        m_vectWav.clear();
        float samplerate = wav.fileFormat().sampleRate()/1000;
        QVector<double> x,y;
        for (int i=0;i<len;++i) {
            m_vectWav.push_back(((float)datai[i]));
            x.push_back((double)i/samplerate/1000);
        }
        double yMax = *std::max_element(m_vectWav.begin(), m_vectWav.end());
        for (auto &a:m_vectWav) y.push_back((double)a/yMax); //normalization

        ui->inputAudio->xAxis->setRange(0, x[x.size()-1]);

        ui->inputAudio->yAxis->setRange(-1, 1);
        ui->inputAudio->xAxis->setLabel("Time (s)");
        ui->inputAudio->yAxis->setLabel("Amplitude");

        if(ui->inputAudio->removeGraph(0)) ui->inputAudio->addGraph();
        ui->inputAudio->graph(0)->setData(x,y);
        ui->inputAudio->replot();
    }
}


void MainWindow::on_bouton_audioFile_clicked()
{

    player->stop();// On arrete la lecture

    ui->bouton_ecouter->setText("Play");

    m_fichierAudio = QFileDialog::getOpenFileName(this, tr("Open WAV File"),
                                                    m_audioInput,
                                                    tr("Audio (*.wav)"));
    if (m_fichierAudio =="") m_fichierAudio = m_audioInput;
    QStringList list = m_fichierAudio.split("/");
    ui->label_fichierAudio->setText(list[list.size()-1]);

    courbeAudioInput();
    m_audioInput = m_fichierAudio;
}

void MainWindow::on_bouton_ecouter_clicked()
{
    if (player->state() == QMediaPlayer::PlayingState) // si lecture en cours
    {
        player->pause();
        ui->bouton_ecouter->setText("Play");

    }else
    if (player->state() == QMediaPlayer::PausedState) // si en pause
    {
        player->play();
        ui->bouton_ecouter->setText("Pause");
    }else
    if (player->state() == QMediaPlayer::StoppedState) // si à l'arret
    {
        player->setMedia(QUrl::fromLocalFile(m_fichierAudio));
        player->setVolume(50);
        player->play();
        ui->bouton_ecouter->setText("Pause");
    }
}


void MainWindow::on_bouton_convolution_clicked()
{
    int nfft(1024);
    long nlog;
    int k, j, i;

    if (player->state() != QMediaPlayer::StoppedState)
    {
        player->stop();
        ui->bouton_ecouter->setText("Play");
    }

    if(!m_vectWav.empty())
    {
        std::vector<float> absR = absair(m_temperature, m_humidite);
        if (m_sourceImage.empty())
        {
            on_bouton_sourcesImages_clicked();
        }
        if (m_sourceImage[m_numListener].calculerRIR(m_freq, absR, -1, ui->checkBox_decaycurve->isChecked(), m_seuilAttenuation))
        {

            // Ouvrir fenetre de progress bar
            QProgressDialog progress(this);
                progress.setWindowModality(Qt::WindowModal);
                progress.setLabelText("working...");
                progress.setRange(0,13);
                progress.setMinimumDuration(0);
                progress.show();

            nlog = round(log(nfft) / log(2));


            std::vector< std::vector<float> > filtres;
            if(!bandFilters(filtres)) return;

            std::vector< std::vector<float> > fir;
            // convolution avec les filtres
            for (k= 0; k<filtres.size() ; k++)
            {
                fir.push_back(convolution_temporelle(m_sourceImage[m_numListener].getFIR()[k],filtres[k]));
            }

            // Partitionnement de la FIR
            std::vector< std::vector<float> > firPart;
            partitionnage(fir, firPart, nfft);


            fftInit(nlog);

            // fft des FIR partitionnées
            for (auto &a : firPart) { rffts(a.data(), nlog, 1); // on passe fir en frequentielle (directement enregistré dans lui-même)
            }

            // Somme fréquencielle
            int nFiltre = fir.size(); // nombre de bande fréquentielle
            int nPart = firPart.size()/nFiltre; // nombre de partition par bande

            for (k=1; k<nFiltre ; k++)
            {
                for (i=0; i<nPart ; i++)
                {
                    *std::transform(firPart[i].begin(), firPart[i].end(), firPart[i+k*nPart].begin(), firPart[i].begin(), std::plus<float>()); // somme des 7 bandes dans la première
                }
            }

            // découpage du wav
            std::vector< std::vector<float> > wavPart;
            partitionner(m_vectWav, nfft, wavPart);

            //passage du wav en fft
            std::vector<float> buf1;
            buf1.resize(nfft, 0);
            std::vector< std::vector<float> >  buf2;
            buf2.resize(wavPart.size()+nPart-1);
            for (auto &a : buf2) { a.resize(nfft, 0); }

            int prog = wavPart.size();
            progress.setRange(0,prog*1.1);

            // initialisation lecture temps reel
            std::vector<qint16> newData2;
            newData2.resize(nfft/2);

            for (k = 0; k < wavPart.size(); k++)
            {
                rffts(wavPart[k].data(), nlog, 1); // fft
                for(j=0 ; j <nPart ; j++) // pour chaque partie du filtre
                {
                    // multiplication spectrale du wav et des filtres
                    rspectprod(wavPart[k].data(), firPart[j].data(), buf1.data(), nfft);
                    std::transform(buf2[j+k].begin(), buf2[j+k].end(), buf1.begin(), buf2[j+k].begin(), std::plus<float>()); // somme terme à terme http://www.cplusplus.com/reference/algorithm/transform/
                }
                progress.setValue(k);

                riffts(buf2[k].data(), nlog, 1); //ifft

                for (i = 0; i< nfft/2 ; i++) newData2[i]=(qint16)buf2[k][i];
            }

            for (k=wavPart.size(); k < buf2.size() ; k++)
            {
                riffts(buf2[k].data(), nlog, 1);

                for (i = 0; i< nfft/2 ; i++) newData2[i]=(qint16)buf2[k][i];
            }

            std::vector<float> newWav;
            recombiner(buf2, newWav);

            fftFree();


            // Création du nouveau fichier audio
            std::vector<int> newData;
            for (auto &a : newWav) { newData.push_back((int)a); }

            WavFile wav;
             if(wav.open(m_fichierAudio))
             {
                wav.writeNewWav(newData);
                m_fichierAudio = QCoreApplication::applicationDirPath() + "/result.wav";
                ui->bouton_ecouter->setText("Result");

                wav.close();
             }
            progress.setValue(1.1*prog);
            courbeAudioInput();

        }
        else QMessageBox::warning(NULL,"Beware","Direct sound only");
    }    
    else QMessageBox::warning(NULL,"Beware","No readable file");

}


// SLOTS DE PARAMETRAGE

void MainWindow::on_AudioSlider_valueChanged(int value) {
    player->setPosition(value);

}

void MainWindow::on_positionChanged(qint64 position)
{
    ui->AudioSlider->setValue(position);
    if (position >= ui->AudioSlider->maximum()){
        ui->AudioSlider->setValue(0);
        if(player->currentMedia().canonicalUrl() == QUrl::fromLocalFile(QCoreApplication::applicationDirPath() + "/result.wav"))
            ui->bouton_ecouter->setText("Result");
        else
            ui->bouton_ecouter->setText("Play");
    }
}

void MainWindow::on_durationChanged(qint64 position) {
    ui->AudioSlider->setMaximum(position);
}


void MainWindow::on_spinBox_nbRebond_valueChanged(int arg1) {
    m_nbRebond = arg1;
}

void MainWindow::on_spinBox_attenuation_valueChanged(int arg1) {
    m_seuilAttenuation = pow(10,(-(float)arg1/10));
}


void MainWindow::on_checkBox_rayAuto_toggled(bool checked) {
   m_rayAuto = checked;
   m_longueurRayMax.resize(m_listener.size(), 1e6);
   if(m_rayAuto)
   {
       for (int i = 0 ; i< m_longueurRayMax.size() ; i++)
           m_longueurRayMax[i]=m_nbRayon*m_listener[i].getRayon()/(2*m_seuilArret*sqrt(m_nbRayon/m_seuilArret-1));

       if(m_longueurRayMax.size()<2 && !m_longueurRayMax.empty()) // mode mono-listener
       {
           ui->lcdTempsmax->setEnabled(true);
           ui->lcdTempsmax->display(m_longueurRayMax[m_numListener]/VITESSE_SON);
       }
       else {
           ui->lcdTempsmax->setEnabled(false);
           QString text;
           for (int i =0 ; i< m_longueurRayMax.size() ; i++) text += "Listener"+QString::number(i+1) + "\n" + m_listener[i].afficher() + "\nDurée maximum : " + QString::number(m_longueurRayMax[i]/VITESSE_SON) + "\n\n";
           QMessageBox::information(NULL,"Mode multi listener",text);
       }
   }
   ui->lcdNumber_ListenerRadius->display(m_listener[m_numListener].getRayon());

}

void MainWindow::on_spinBox_freqEchan_editingFinished() {
     m_freq = ui->spinBox_freqEchan->value();
}

void MainWindow::on_spinBox_seuilArret_editingFinished()
{
    m_seuilArret = ui->spinBox_seuilArret->value();
    if(m_rayAuto) on_checkBox_rayAuto_toggled(true);
}

void MainWindow::on_spinBox_gain_editingFinished()
{
    m_gain = ui->spinBox_gain->value();
}

void MainWindow::on_spinBox_numListener_editingFinished()
{
    m_numListener = ui->spinBox_numListener->value()-1;
    on_checkBox_rayAuto_toggled(m_rayAuto);
}

void MainWindow::on_spinBox_numSource_editingFinished()
{
    m_numSource = ui->spinBox_numSource->value()-1;
}

void MainWindow::on_spinBox_nbRay_editingFinished()
{
    m_nbRayon = ui->spinBox_nbRay->value();
    if(m_rayAuto) {
        on_checkBox_rayAuto_toggled(true);
    }
}

void MainWindow::on_doubleSpinBox_siTps_editingFinished()
{
    ui->doubleSpinBox_siTps_erreur->setValue(ui->doubleSpinBox_siTps->value()+m_tpsDiff);
}

void MainWindow::on_doubleSpinBox_siTps_erreur_editingFinished()
{
    m_tpsDiff = ui->doubleSpinBox_siTps_erreur->value() - ui->doubleSpinBox_siTps->value();
}


void supprLayoutContent(QGridLayout *layout)
{
    QLayoutItem *child;
    QWidget *wid;
    while(layout->count())
    {
        child = layout->takeAt(0);
        wid = child->widget();
        wid->deleteLater();
    }
}


void MainWindow::on_actionInfo_triggered()
{
    QString textInfo = "Software name : Just4RIR\n";
            textInfo+="Creation date : November 30th 2018\n";
            textInfo+="Last modification : November 30th 2018\n";
            textInfo+="Author : Robin Gueguen\n";
            textInfo+="License : GPL 3.0 2018\n";
            textInfo+="Property : Institut des Sciences du Calcul et des Données - Sorbonne Université\n";
            textInfo+="Version : 1.0 beta";

    QMessageBox::information(NULL,"Information",textInfo);
}


void MainWindow::on_radioButton_rebFixe_clicked()
{
    m_nbRebondFixe=true;
    ui->spinBox_nbRebond->setEnabled(true);
    ui->radioButton_seuildB->setChecked(false);
    ui->spinBox_attenuation->setEnabled(false);
    ui->checkBox_rayAuto->setEnabled(false);
    ui->lcdTempsmax->setEnabled(false);
    ui->spinBox_seuilArret->setEnabled(false);
    ui->label_tpsmax->setEnabled(false);
    ui->label_ListenerRadius->setEnabled(false);
}

void MainWindow::on_radioButton_seuildB_clicked()
{
    m_nbRebondFixe=false;
    ui->spinBox_nbRebond->setEnabled(false);
    ui->radioButton_rebFixe->setChecked(false);
    ui->spinBox_attenuation->setEnabled(true);
    ui->checkBox_rayAuto->setEnabled(true);
    ui->lcdTempsmax->setEnabled(true);
    ui->spinBox_seuilArret->setEnabled(true);
    ui->label_tpsmax->setEnabled(true);
    ui->label_ListenerRadius->setEnabled(true);



}

void MainWindow::on_radioButton_Fibonacci_clicked()
{
    m_fibonacci=true;
    ui->spinBox_nbRay->setEnabled(true);
}

void MainWindow::on_radioButton_vertexSource_clicked()
{
    m_fibonacci=false;
    ui->spinBox_nbRay->setEnabled(false);
}

void MainWindow::on_spinBox_temperature_valueChanged(int arg1)
{
    m_temperature = arg1;
    courbeAtmo();
}

void MainWindow::on_spinBox_humidite_valueChanged(int arg1)
{
    m_humidite = arg1;
    courbeAtmo();
}


void MainWindow::on_radioButton_tpsSI_clicked()
{
    m_rayFromSI=true;
    ui->doubleSpinBox_siTps->setEnabled(true);
    ui->doubleSpinBox_siTps_erreur->setEnabled(true);
    ui->label_to->setEnabled(true);
}

void MainWindow::on_radioButton_initSource_clicked()
{
    m_rayFromSI=false;
    ui->doubleSpinBox_siTps->setEnabled(false);
    ui->doubleSpinBox_siTps_erreur->setEnabled(false);
    ui->label_to->setEnabled(false);
}

void MainWindow::on_actionUser_manual_triggered()
{
    QString link = "https://github.com/RobinGueguen/Just4RIR/blob/master/Just4RIR%20-%20User%20Manuel.pdf";
    QDesktopServices::openUrl(QUrl(link));
}
