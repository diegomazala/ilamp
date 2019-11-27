#ifndef __IMP_DLL_H__
#define __IMP_DLL_H__



#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#define DllExport extern "C" __declspec (dllexport)

DllExport void* Imp_GetQ();
DllExport cv::Mat& Imp_BackProjectedImage();


#ifdef __cplusplus
extern "C" {
#endif

DllExport void Imp_Create_ILamp();
DllExport bool Imp_LoadProject(const char* filename, bool compute_lamp);
DllExport bool Imp_Build();
DllExport bool Imp_Execute(float x, float y);
DllExport void* Imp_GetVertices2d(int index);
DllExport void* Imp_GetVerticesNd(int index);
DllExport void* Imp_GetQ();
DllExport size_t Imp_QRows();
DllExport size_t Imp_QCols();



#ifdef __cplusplus
};
#endif

#endif // __IMP_DLL_H__