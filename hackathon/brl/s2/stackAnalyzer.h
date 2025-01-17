#ifndef STACKANALYZER_H
#define STACKANALYZER_H

#include <QObject>
#include <v3d_interface.h>
#include <QString>
#include <QFileInfo>
#include <QDebug>
#include <QMutex>



class StackAnalyzer : public QObject
{
    Q_OBJECT
public:
    StackAnalyzer(V3DPluginCallback2 &callback);
	bool offOp;

signals:
    void analysisDone(QList<LandmarkList> newTipsList, LandmarkList newTargets, Image4DSimple* total4DImage_mip, double tileIndex, QString tileSaveString, int tileStatus);
    void messageSignal(QString msg);
    void combinedSWC(QString fileSaveName);
    void loadingDone(Image4DSimple* total4DImage_mip);
    void bail();
public slots:


    void processSmartScan(QString fileWithData);
    void loadGridScan(QString latestString,  LocationSimple tileLocation, QString saveDirString);
    void updateChannel(QString newChannel);
    void updateRedThreshold(int rThresh);
    void updateRedAlpha(float rAlpha);
    void updateLipoMethod(int lipoMethod);
    void updateGlobalMinMaxBlockSizes(int newMinBlockSize, int newMaxBlockSize);
    void updateSearchRadius(double inputRadius);

    NeuronTree sort_eliminate_swc(NeuronTree nt,LandmarkList inputRootList,Image4DSimple* total4DImage,bool isSoma);
    LandmarkList eliminate_seed(NeuronTree nt,LandmarkList inputRootList,Image4DSimple* total4DImage);

    NeuronTree generate_crossing_swc(Image4DSimple* total4DImage);
    NeuronTree neuron_sub(NeuronTree nt_total, NeuronTree nt);


    void ada_win_finding(LandmarkList tips,LocationSimple tileLocation,LandmarkList *newTargetList,QList<LandmarkList> *newTipsList,Image4DSimple* total4DImage,int max_block_size,int direction,float overlap, int min_block_size);
    bool combine_list2file(QList<NeuronSWC> & lN, QString fileSaveName);

    QList<LandmarkList> group_tips(LandmarkList tips,int block_size, int direction);

    void startTracing(QString latestString, float overlap, int background, bool interrupt, LandmarkList inputRootList, LocationSimple tileLocation, QString saveDirString, bool useGSDT, bool isSoma, bool isAdaptive, int methodChoice, int tileStatus);

    //void loadScan(QString latestString, float overlap, int background, bool interrupt, LandmarkList inputRootList, LocationSimple tileLocation, QString saveDirString, bool useGSDT, bool isSoma);
   void APP2Tracing(Image4DSimple* total4DImage,  Image4DSimple* total4DImage_mip, QString swcString, float overlap, int background, bool interrupt, LandmarkList inputRootList, bool useGSDT, bool isSoma, LocationSimple tileLocation, QString tileSaveString, int tileStatus);
    void APP2Tracing_adaptive(Image4DSimple* total4DImage,  Image4DSimple* total4DImage_mip, QString swcString, float overlap, int background, bool interrupt, LandmarkList inputRootList, LocationSimple tileLocation, QString saveDirString, bool useGSDT, bool isSoma, QString tileSaveString, int tileStatus);

    void SubtractiveTracing(QString latestString, QString imageSaveString, Image4DSimple* total4DImage, Image4DSimple* total4DImage_mip,QString swcString,float overlap, int background, bool interrupt, LandmarkList inputRootList, LocationSimple tileLocation, QString saveDirString,bool useGSDT, bool isSoma, int methodChoice, int tileStatus);
    void SubtractiveTracing_adaptive(QString latestString, QString imageSaveString, Image4DSimple* total4DImage, Image4DSimple* total4DImage_mip,QString swcString,float overlap, int background, bool interrupt, LandmarkList inputRootList, LocationSimple tileLocation, QString saveDirString,bool useGSDT, bool isSoma, int methodChoice, int tileStatus);

    int methodSelection(Image4DSimple* total4DImage, LandmarkList inputRootList, int background, bool isSoma);
private:
    QMutex sAMutex;
    V3DPluginCallback2 * cb;
    QString channel;
    unsigned short int redThreshold;
    float redAlpha;
    int lipofuscinMethod;
    int globalMaxBlockSize;
    int globalMinBlockSize;
    double radius;



    template <class T> void gaussian_filter(T* data1d,
                         V3DLONG *in_sz,
                         unsigned int Wx,
                         unsigned int Wy,
                         unsigned int Wz,
                         unsigned int c,
                         double sigma,
                         float* &outimg);

};

#endif // STACKANALYZER_H
