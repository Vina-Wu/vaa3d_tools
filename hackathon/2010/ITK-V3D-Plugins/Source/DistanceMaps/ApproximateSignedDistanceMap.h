#ifndef __ApproximateSignedDistanceMap_H__
#define __ApproximateSignedDistanceMap_H__

#include "V3DITKPluginDefaultHeader.h"

class ApproximateSignedDistanceMapPlugin : public QObject, public V3DPluginInterface
{
  Q_OBJECT
  Q_INTERFACES(V3DPluginInterface)
  V3DITKPLUGIN_DEFAULT_CLASS_DECLARATION_BODY(ApproximateSignedDistanceMap);
};

#endif
