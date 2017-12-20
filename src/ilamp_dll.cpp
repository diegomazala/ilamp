#include "ilamp.h"


#define DllExport extern "C" __declspec (dllexport)



static std::vector<std::string> filePaths;
static std::unique_ptr<ILamp<float>> ilamp;
static std::vector<float> output_verts;
static Eigen::Matrix<float, Eigen::Dynamic, 1> q;


DllExport void ILamp_AddFilePath(const char* file_path)
{
	filePaths.push_back(file_path);
}

DllExport void ILamp_ResetFiles()
{
	filePaths.clear();
}

DllExport void ILamp_BuildNdFile(const char* filename_Nd)
{
}

DllExport void ILamp_RunLamp(const char* output_filename_2d)
{

}

DllExport bool ILamp_LoadInputFiles(const char* filename_2d, const char* filename_Nd)
{
	ilamp.reset(new ILamp<float>());

	if (!(ilamp->load_data_2d(filename_2d) && ilamp->load_data_Nd(filename_Nd)))
		return false;

	if (ilamp->verts_Nd.size() != ilamp->verts_2d.size())
	{
		//std::cerr << "<Error> Vertex arrays do not have the same size. Abort" << std::endl;
		return false;
	}

	return true;
}

DllExport void ILamp_BuildKdTree(uint16_t kdtree_count)
{
	assert(ilamp);

	ilamp->build_kdtree(kdtree_count);
}


DllExport void ILamp_RunILamp(float x, float y)
{
	assert(ilamp);

	q = ilamp->execute(x, y);
}

DllExport size_t ILamp_QRows()
{
	return q.rows();
}

DllExport size_t ILamp_QCols()
{
	return q.cols();
}


DllExport void ILamp_CopyQ(void* p_array_float_N)
{
	float* p_array_float = (float*)p_array_float_N;

	// safeguard - pointer must be not null
	if (!p_array_float)
		return;

	size_t r = q.rows();
	size_t c = q.cols();
	size_t coords_count = q.rows() * q.cols();

	std::memcpy(p_array_float, q.data(), coords_count * sizeof(float));
}

DllExport float ILamp_MinX()
{
	return ilamp->min_x;
}

DllExport float ILamp_MaxX()
{
	return ilamp->max_x;
}

DllExport float ILamp_MinY()
{
	return ilamp->min_y;
}

DllExport float ILamp_MaxY()
{
	return ilamp->max_y;
}
