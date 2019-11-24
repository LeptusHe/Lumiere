#pragma once
#include "LumiereConfig.h"

#define __FUNCTION_SITE__   __FUNCTION__, __FILE__, __LINE__
#define __CODE__SITE__      __FUNCTION__, __FILE__, __LINE__
#define __EXCEPTION_SITE__  __FILE__, __FUNCTION__, __LINE__


#define BEGIN_LUMIERE_NAMESPACE namespace NAMESPACE_NAME {
#define END_LUMIERE_NAMESPACE } // NAMESPACE_NAME
