#ifndef SORT_SWC_REDIFINED_H
#define SORT_SWC_REDIFINED_H

// Adapted from /Users/pengxie/Applications/vaa3d/v3d_external/released_plugins_more/v3d_plugins/sort_neuron_swc/sort_swc.h
#include "QtGlobal"
#include "math.h"
//#include "unistd.h" //remove the unnecessary include file. //by PHC 20131228
#include "basic_surf_objs.h"
#include "string.h"
#include "vector"
#include "iostream"
using namespace std;
#include "QtGui"
#include "QDialog"
#include "v3d_interface.h"
#include "utilities.h"
//#include "unordered_map"

#ifndef VOID
#define VOID 1000000000
#endif

//#define PI 3.14159265359
#define getParent(n,nt) ((nt).listNeuron.at(n).pn<0)?(1000000000):((nt).hashNeuron.value((nt).listNeuron.at(n).pn))
#define NTDIS(a,b) (sqrt(((a).x-(b).x)*((a).x-(b).x)+((a).y-(b).y)*((a).y-(b).y)+((a).z-(b).z)*((a).z-(b).z)))
#define NTDOT(a,b) ((a).x*(b).x+(a).y*(b).y+(a).z*(b).z)
#define angle(a,b,c) (acos((((b).x-(a).x)*((c).x-(a).x)+((b).y-(a).y)*((c).y-(a).y)+((b).z-(a).z)*((c).z-(a).z))/(NTDIS(a,b)*NTDIS(a,c)))*180.0/3.14159265359)

#ifndef MAX_DOUBLE
#define MAX_DOUBLE 1.79768e+308        //actual: 1.79769e+308
#endif

QVector< QVector<V3DLONG> > get_neighbors(QList<NeuronSWC> &neurons, const QHash<V3DLONG,V3DLONG> & LUT);
QHash<V3DLONG, V3DLONG> getUniqueLUT(QList<NeuronSWC> &neurons);
QList<V3DLONG> DFS(QVector< QVector<V3DLONG> > neighbors, V3DLONG newrootid, V3DLONG siz);

double computeDist2(const NeuronSWC & s1, const NeuronSWC & s2, double xscale=1, double yscale=1, double zscale=1);
bool combine_linker(vector<QList<NeuronSWC> > & linker, QList<NeuronSWC> & combined);

NeuronTree my_SortSWC(NeuronTree nt, V3DLONG newrootid, double thres);
bool SortSWC(QList<NeuronSWC> & neurons, QList<NeuronSWC> & result, V3DLONG newrootid, double thres);

bool export_list2file(QList<NeuronSWC> & lN, QString fileSaveName, QString fileOpenName);

void connect_swc(NeuronTree nt,QList<NeuronSWC>& newNeuron, double disThr,double angThr);

NeuronTree pruneswc(NeuronTree nt, double length);

#endif // SORT_SWC_REDIFINED_H
