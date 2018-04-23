using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using System.Linq;




public class ImpPlugin
{
    public const string DllName = "imp";

    [DllImport(DllName)]
    public static extern void Imp_Initialize_ILamp();

    [DllImport(DllName)]
    public static extern void Imp_Initialize_Rbf();

    [DllImport(DllName)]
    public static extern bool Imp_ExecuteLamp(string input_filename_nd, string output_filename_2d);

    [DllImport(DllName)]
    public static extern bool Imp_LoadInputFiles(string filename_2d, string filename_Nd);

    [DllImport(DllName)]
    public static extern void Imp_ILamp_Setup(ushort kdtree_count, ushort num_neighbours, ushort knn_search_checks);

    [DllImport(DllName)]
    public static extern void Imp_Rbf_Setup(ushort function_type_enum, float constant);

    [DllImport(DllName)]
    public static extern bool Imp_Build();

    [DllImport(DllName)]
    public static extern bool Imp_Execute(float x, float y);

    [DllImport(DllName)]
    public static extern long Imp_QRows();

    [DllImport(DllName)]
    public static extern long Imp_QCols();

    [DllImport(DllName)]
    public static extern bool Imp_CopyQ(System.IntPtr float_array);

    [DllImport(DllName)]
    public static extern float Imp_MinX();

    [DllImport(DllName)]
    public static extern float Imp_MaxX();

    [DllImport(DllName)]
    public static extern float Imp_MinY();

    [DllImport(DllName)]
    public static extern float Imp_MaxY();

    [DllImport(DllName)]
    public static extern bool Imp_ExecutePcaImages(string input_image_list_file, string output_file);

    [DllImport(DllName)]
    public static extern bool Imp_BackProjectImageExecute(float x, float y);

    [DllImport(DllName)]
    public static extern bool Imp_BackProjectFile();

    [DllImport(ImpPlugin.DllName)]
    public static extern void SetTextureFromUnity(System.IntPtr texture, int w, int h, int channels);

    [DllImport(ImpPlugin.DllName)]
    public static extern System.IntPtr GetRenderEventFunc();


    static public float LinearInterpolation(float x, float x0, float x1, float y0, float y1)
    {
        if ((x1 - x0) == 0)
        {
            return (y0 + y1) / 2;
        }
        return y0 + (x - x0) * (y1 - y0) / (x1 - x0);
    }


    static public void BuildNdFile(MeshFilter[] baseMeshes, string outputFileNameNd, float scale = 100)
    {
        using (System.IO.StreamWriter writer = new System.IO.StreamWriter(outputFileNameNd))
        {
            System.Globalization.CultureInfo customCulture = (System.Globalization.CultureInfo)System.Threading.Thread.CurrentThread.CurrentCulture.Clone();
            customCulture.NumberFormat.NumberDecimalSeparator = ".";
            System.Threading.Thread.CurrentThread.CurrentCulture = customCulture;

            foreach (var m in baseMeshes)
            {
                Vector3[] verts = m.mesh.vertices;
                foreach (var vec in verts)
                {
                    var v = vec * scale;
                    writer.Write(v.x.ToString("0.####") + ' ' + v.y.ToString("0.####") + ' ' + v.z.ToString("0.####") + ' ');
                }
                writer.WriteLine();
            }
        }
    }

}

