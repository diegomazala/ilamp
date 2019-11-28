#ifndef __IMP_DLL_H__
#define __IMP_DLL_H__

#define DllExport extern "C" __declspec (dllexport)

#define USE_OPENCV_IMAGES 0

#if USE_OPENCV_IMAGES
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
DllExport cv::Mat& Imp_BackProjectedImage();
#endif

#ifdef __cplusplus
extern "C" {
#endif

DllExport void Imp_Create_ILamp();
DllExport bool Imp_LoadProject(const char* filename, bool compute_lamp);
DllExport bool Imp_Build();
DllExport bool Imp_Execute(float x, float y);
DllExport void* Imp_GetVertices2d(size_t index);
DllExport void* Imp_GetVerticesNd(size_t index);
DllExport void* Imp_GetQ();
DllExport size_t Imp_QRows();
DllExport size_t Imp_QCols();



#ifdef __cplusplus
};
#endif

#endif // __IMP_DLL_H__