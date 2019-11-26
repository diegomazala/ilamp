#ifndef __IMP_DLL_H__
#define __IMP_DLL_H__

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#define DllExport extern "C" __declspec (dllexport)

DllExport void* Imp_GetQ();
DllExport cv::Mat& Imp_BackProjectedImage();

//DllExport static void Imp_Create_ILamp();
//DllExport static bool Imp_LoadProject(const char* filename);


#endif // __IMP_DLL_H__