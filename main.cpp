
#include "RFserial.h"


using namespace domoaster ;

int LoadPlugins (Kernel & k)
{
  k.RegisterConnector (new RFserial) ;
  return 1 ;
}