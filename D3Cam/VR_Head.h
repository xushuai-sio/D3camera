#pragma once

#include "vrmusbcam2.h"
#include "vrmusbcam2win32.h"

#ifdef min
#undef min(a,b)
#endif // min

void LogExit();

#define VRMEXECANDCHECK(function)\
{\
	if(VRM_SUCCESS!=function)\
		{\
		LogExit();\
		}\
}
