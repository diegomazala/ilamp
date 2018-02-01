#include "ilamp.h"
#include "ilamp_project.h"
#include "ilamp_utils.h"

#include <vector>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#define DllExport extern "C" __declspec (dllexport)



static std::unique_ptr<ILamp<float>> ilamp;
static std::vector<float> output_verts;
static Eigen::Matrix<float, Eigen::Dynamic, 1> q;



DllExport void ILamp_BuildNdFile(const char* ilamp_project_filename)
{
	//
	// Creating nd file
	//
	ilamp_project ilp_prj(ilamp_project_filename);
	build_nd_file(ilp_prj.inputFiles, ilp_prj.filenameNd);
}

DllExport bool ILamp_RunLamp(const char* input_filename_nd, const char* output_filename_2d)
{
	//
	// Running lamp in order to generate 2d file from nd file
	//
	std::string lamp_script = std::getenv("ILAMP_LAMP_SCRIPT");

	if (!fs::exists(lamp_script))
	{
		std::cerr << "Lamp python script not found" << std::endl;
		return false;
	}

	std::stringstream lamp_cmd;
	lamp_cmd << "python " << lamp_script << ' ' << input_filename_nd << ' ' << output_filename_2d << " > lamp.log";
	std::system(lamp_cmd.str().c_str());
#if _DEBUG
	std::cout << std::ifstream("lamp.log").rdbuf();
#endif

	return true;
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


DllExport void ILamp_RunILamp(float x, float y, uint16_t num_neighbours, uint16_t knn_search_checks)
{
	assert(ilamp);

	q = ilamp->execute(x, y, num_neighbours, knn_search_checks);
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

DllExport void ILamp_WritePly(size_t verts_count, void* p_verts_array, void* p_normals_array, void* p_uv_array, void* p_faces_array)
{
	float* p_verts_float = (float*)p_verts_array;
	float* p_normals_float = (float*)p_normals_array;
	float* p_uv_float = (float*)p_uv_array;
	int* p_faces_float = (int*)p_faces_array;

	// safeguard - pointer must be not null
	if (!p_verts_float)
		return;

	std::vector<float> verts(p_verts_float, p_verts_float + verts_count * 3);

	write_ply_file("C:/tmp/point_cloud.ply", verts);
	write_ply_file("C:/tmp/mesh.ply", verts, "C:/Users/diego/Google Drive/Data/Test/Heads/Head01.ply");
}

