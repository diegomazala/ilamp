using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using System.Linq;


public class ILamp : MonoBehaviour
{

    public const string DllName = "ilamp_dlld";

    private class Plugin
    {
        [DllImport(DllName)]
        public static extern bool ILamp_RunLamp(string input_filename_nd, string output_filename_2d);

        [DllImport(DllName)]
        public static extern bool ILamp_LoadInputFiles(string filename_2d, string filename_Nd);

        [DllImport(DllName)]
        public static extern bool ILamp_BuildKdTree(ushort kdtree_count);

        [DllImport(DllName)]
        public static extern void ILamp_RunILamp(float x, float y, ushort num_neighbours, ushort knn_search_checks);

        [DllImport(DllName)]
        public static extern long ILamp_QRows();

        [DllImport(DllName)]
        public static extern long ILamp_QCols();

        [DllImport(DllName)]
        public static extern void ILamp_CopyQ(System.IntPtr float_array);

        [DllImport(DllName)]
        public static extern float ILamp_MinX();

        [DllImport(DllName)]
        public static extern float ILamp_MaxX();

        [DllImport(DllName)]
        public static extern float ILamp_MinY();

        [DllImport(DllName)]
        public static extern float ILamp_MaxY();

    }

    [System.Serializable]
    public class Project
    {
        public string ProjectName;
        public string FileName2d;
        public string FileNameNd;
        public string OutputFolder;
        public string[] InputFiles;
        public ushort NumNeighbours = 4;
        public ushort KdTreeCount = 4;
        public ushort KnnSearchChecks = 128;
    }


    private ILampUI ilampUI = null;

    private float[] q_data = null;
    private GCHandle q_handle;

    public string projectFileName;
    public Project project = null;

    public float ModelScaleFactor = 0.01f;

    public List<Vector2> vertices2d = new List<Vector2>();

    public Vector2 p;

    public MeshFilter templateMesh;

    public MeshFilter[] baseMeshes;

    public Vector2 MinCoords
    {
        get { return new Vector2(Plugin.ILamp_MinX(), Plugin.ILamp_MinY()); }
    }
    public Vector2 MaxCoords
    {
        get { return new Vector2(Plugin.ILamp_MaxX(), Plugin.ILamp_MaxY()); }
    }

    void Start ()
    {
        System.Globalization.CultureInfo customCulture = (System.Globalization.CultureInfo)System.Threading.Thread.CurrentThread.CurrentCulture.Clone();
        customCulture.NumberFormat.NumberDecimalSeparator = ".";
        System.Threading.Thread.CurrentThread.CurrentCulture = customCulture;

        if (!System.IO.File.Exists(projectFileName))
        {
            Debug.LogError("Project file does not exist: " + projectFileName);
            enabled = false;
            return;
        }

        JsonUtility.FromJsonOverwrite(System.IO.File.ReadAllText(projectFileName), project);

        BuildNdFile(project.FileNameNd, 1.0f/ModelScaleFactor);

        if (!Plugin.ILamp_RunLamp(project.FileNameNd, project.FileName2d))
        {
            Debug.LogError("Could not run lamp for " + projectFileName);
            enabled = false;
            return;
        }


        if (!Plugin.ILamp_LoadInputFiles(project.FileName2d, project.FileNameNd))
        {
            Debug.LogError("Could not load project file: " + projectFileName);
            enabled = false;
            return;
        }
        
        if (!Plugin.ILamp_BuildKdTree(project.KdTreeCount))
        {
            Debug.LogError("Could not build kd-tree");
            enabled = false;
            return;
        }



        using (System.IO.TextReader reader = System.IO.File.OpenText(project.FileName2d))
        {
            string line;
            while ((line = reader.ReadLine()) != null)
            {
                string[] v_str = line.Split();

                float x, y = 0;
                if (float.TryParse(v_str[0], out x) && float.TryParse(v_str[1], out y))
                    vertices2d.Add(new Vector2(x, y));
            }

            reader.Close();
        }


        ilampUI = (ILampUI)FindObjectOfType(typeof(ILampUI));
        if (ilampUI)
            ilampUI.Setup(this);

    }

    private void OnDisable()
    {
        if (q_data != null)
            q_handle.Free();
    }

    void Update()
    {

        //computing p
        {
            Vector2 p_norm = new Vector2(Input.mousePosition.x / Screen.width, Input.mousePosition.y / Screen.height);
            Vector2 p_cartesian = new Vector2((p_norm.x - 0.5f) * 2.0f, (p_norm.y - 0.5f) * 2.0f);

            float p_width = Plugin.ILamp_MaxX() - Plugin.ILamp_MinX();
            float p_height = Plugin.ILamp_MaxY() - Plugin.ILamp_MinY();

            p = new Vector2(p_cartesian.x * p_width, p_cartesian.y * p_height);
        }

        if (Input.GetKeyDown(KeyCode.Space) || Input.GetKey(KeyCode.LeftControl))
        {
            ExecuteIlamp();
        }

        if (Input.GetKeyDown(KeyCode.Alpha1))
        {
            p = vertices2d[0];
            //ExecuteIlamp();
        }
        else if (Input.GetKeyDown(KeyCode.Alpha2))
        {
            p = vertices2d[1];
            //ExecuteIlamp();
        }
        else if (Input.GetKeyDown(KeyCode.Alpha3))
        {
            p = vertices2d[2];
            //ExecuteIlamp();
        }
        else if (Input.GetKeyDown(KeyCode.Alpha4))
        {
            p = vertices2d[3];
            //ExecuteIlamp();
        }

        if (Input.GetKeyDown(KeyCode.Return))
            ExecuteIlamp();



        if (Input.GetKeyDown(KeyCode.E))
        {
            BuildNdFile(project.FileNameNd);
        }

    }

    void ExecuteIlamp()
    {
        Plugin.ILamp_RunILamp(p.x, p.y, project.NumNeighbours, project.KnnSearchChecks);

        if (q_data == null)
        {
            q_data = new float[Plugin.ILamp_QRows() * Plugin.ILamp_QCols()];
            q_handle = GCHandle.Alloc(q_data, GCHandleType.Pinned);
        }

        Plugin.ILamp_CopyQ(q_handle.AddrOfPinnedObject());

        if (templateMesh)
        {
            Vector3[] vertices = templateMesh.mesh.vertices;

            for (long v = 0; v < vertices.Length; ++v)
            {
                long i = v * 3;
                vertices[v] = new Vector3(q_data[i + 0] * ModelScaleFactor, q_data[i + 1] * ModelScaleFactor, q_data[i + 2] * ModelScaleFactor);
            }

            templateMesh.mesh.vertices = vertices;
            templateMesh.mesh.RecalculateBounds();
            templateMesh.mesh.RecalculateNormals();
        }
    }


    void BuildNdFile(string outputFileNameNd, float scale = 100)
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
