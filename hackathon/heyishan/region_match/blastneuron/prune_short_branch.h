#ifndef PRUNE_SHORT_BRANCH_H
#define PRUNE_SHORT_BRANCH_H
#include "basic_surf_objs.h"
#include <QtGlobal>
#include <vector>
using namespace std;
bool prune_branch(NeuronTree nt, NeuronTree & result, double prune_size);
bool export_listNeuron_2swc(QList<NeuronSWC> & list, const char* filename);
double calculate_diameter(NeuronTree nt, vector<V3DLONG> branches);
#endif // PRUNE_SHORT_BRANCH_H
