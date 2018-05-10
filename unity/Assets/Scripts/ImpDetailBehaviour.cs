using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using System.Linq;


public class ImpDetailBehaviour : MonoBehaviour
{
    public enum RbfFunctionEnum
    {
        Linear,
        Gaussian,
        Multiquadrics,
        InvMultiquadrics
    }

    public enum ImpTypeEnum
    {
        ILamp,
        Rbf
    }
    public ImpTypeEnum ImpType = ImpTypeEnum.ILamp;

    private ImpUI impUI = null;

    private float[] q_data = null;
    private GCHandle q_handle;

    public bool RunLamp = true;

    public string FileName2d;
    public string FileNameNd;
    public int meshSelected = 0;
    public MeshFilter[] baseMeshes;
    public MeshFilter[] laplaceMeshes;

    [System.Serializable]
    public class ILampConfig
    {
        public ushort KdTreeCount = 4;
        public ushort NumNeighbours = 4;
        public ushort KnnSearchChecks = 128;
    }
    public ILampConfig ILampParams;

    [System.Serializable]
    public class RbfConfig
    {
        public RbfFunctionEnum Function = RbfFunctionEnum.Multiquadrics;
        public float Constant;
    }
    public RbfConfig RbfParams;



    public float ModelScaleFactor = 0.01f;

    public List<Vector2> vertices2d = new List<Vector2>();

    public Vector2 p;

    public Material wireframe = null;

    public MeshFilter templateMesh;

    private bool controlKeyPressed = false;

    Vector3[] originalVertices;


    public Vector2 MinCoords
    {
        get { return new Vector2(ImpPlugin.Imp_MinX(), ImpPlugin.Imp_MinY()); }
    }
    public Vector2 MaxCoords
    {
        get { return new Vector2(ImpPlugin.Imp_MaxX(), ImpPlugin.Imp_MaxY()); }
    }



    void Start ()
    {
        if (ImpType == ImpTypeEnum.ILamp)
        {
            ImpPlugin.Imp_Initialize_ILamp();
            ImpPlugin.Imp_ILamp_Setup(ILampParams.KdTreeCount, ILampParams.NumNeighbours, ILampParams.KnnSearchChecks);
        }
        else
        {
            ImpPlugin.Imp_Initialize_Rbf();
            ImpPlugin.Imp_Rbf_Setup((ushort)(RbfParams.Function), RbfParams.Constant);
        }


        System.Globalization.CultureInfo customCulture = (System.Globalization.CultureInfo)System.Threading.Thread.CurrentThread.CurrentCulture.Clone();
        customCulture.NumberFormat.NumberDecimalSeparator = ".";
        System.Threading.Thread.CurrentThread.CurrentCulture = customCulture;


        if (RunLamp)
        {
#if false
            ImpPlugin.BuildNdFile(baseMeshes, FileNameNd, 1.0f / ModelScaleFactor);
#else
            if (!ImpPlugin.BuildNdDetailsFile(baseMeshes, laplaceMeshes, FileNameNd, 1.0f / ModelScaleFactor))
            {
                Debug.LogError("Could not build Nd file. Abort");
                enabled = false;
                return;
            }
#endif

            if (!ImpPlugin.Imp_ExecuteLamp(FileNameNd, FileName2d))
            {
                Debug.LogError("Could not run lamp for " + FileNameNd + ' ' + FileName2d);
                enabled = false;
                return;
            }
        }


        if (!ImpPlugin.Imp_LoadInputFiles(FileName2d, FileNameNd))
        {
            Debug.LogError("Could not load input files: " + FileName2d + ' ' + FileNameNd);
            enabled = false;
            return;
        }

        if (!ImpPlugin.Imp_Build())
        {
            Debug.LogError("Could not build imp");
            enabled = false;
            return;
        }


        using (System.IO.TextReader reader = System.IO.File.OpenText(FileName2d))
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


        if (ImpPlugin.Imp_Execute(vertices2d[0].x, vertices2d[0].y))
        {
            if (q_data == null) // q_data.Count must be (3 * vertices.Length)
            {
                q_data = new float[ImpPlugin.Imp_QRows() * ImpPlugin.Imp_QCols()];
                q_handle = GCHandle.Alloc(q_data, GCHandleType.Pinned);
            }
        }
        else
        {
            Debug.LogError("Could not run imp");
            enabled = false;
            return;
        }

        
        impUI = (ImpUI)FindObjectOfType(typeof(ImpUI));
        if (impUI)
            impUI.Setup(vertices2d, MinCoords, MaxCoords);


        //originalVertices = templateMesh.mesh.vertices;
        originalVertices = laplaceMeshes[meshSelected].mesh.vertices;
    }


    private void OnDisable()
    {
        if (q_data != null)
            q_handle.Free();
    }



    void Update()
    {
        if (Input.GetKeyDown(KeyCode.LeftControl))
        {
            controlKeyPressed = true;
            if (impUI)
                impUI.MouseTarget(controlKeyPressed);
        }
        else if (Input.GetKeyUp(KeyCode.LeftControl))
        {
            controlKeyPressed = false;
            if (impUI)
                impUI.MouseTarget(controlKeyPressed);
        }


        if (controlKeyPressed)
        {
            p = new Vector2(
                ImpPlugin.LinearInterpolation(Input.mousePosition.x, 0, Screen.width, MinCoords.x, MaxCoords.x),
                ImpPlugin.LinearInterpolation(Input.mousePosition.y, 0, Screen.height, MinCoords.y, MaxCoords.y));

            if (p.x > ImpPlugin.Imp_MinX() && p.x < ImpPlugin.Imp_MaxX() &&
                p.y > ImpPlugin.Imp_MinY() && p.y < ImpPlugin.Imp_MaxY())
            {
                Execute(p);
            }
        }
        else
        {
            MeshRenderer target = templateMesh.GetComponent<MeshRenderer>();
            int v = 0;
            for (var key = KeyCode.Alpha1; key <= KeyCode.Alpha9; ++key)
            {
                if (Input.GetKeyDown(key))
                {
                    originalVertices = laplaceMeshes[v % laplaceMeshes.Length].mesh.vertices;

                    p = vertices2d[v % vertices2d.Count];

                    MeshRenderer source = baseMeshes[v % vertices2d.Count].GetComponent<MeshRenderer>();
                    if (target && source)
                        target.sharedMaterial = source.sharedMaterial;
                }
                ++v;
            }

            if (Input.GetKeyDown(KeyCode.Alpha0))
            {
                target.sharedMaterial = wireframe;
            }

            Execute(p);
        }


        if (Input.GetKeyDown(KeyCode.Return) || Input.GetKeyDown(KeyCode.Space))
            Execute(p);

    }


    void Execute(Vector2 _p)
    {
        if (!ImpPlugin.Imp_Execute(_p.x, _p.y))
        {
            Debug.LogError("Could not execute imp : " + _p.ToString());
            return;
        }

        ImpPlugin.Imp_CopyQ(q_handle.AddrOfPinnedObject());

        if (templateMesh)
        {
            //Vector3[] vertices = templateMesh.mesh.vertices;
            Vector3[] vertices = new Vector3[originalVertices.Length];

            for (long v = 0; v < vertices.Length; ++v)
            {
                long i = v * 3;
                Vector3 detail = new Vector3(q_data[i + 0] * ModelScaleFactor, q_data[i + 1] * ModelScaleFactor, q_data[i + 2] * ModelScaleFactor);
                vertices[v] = originalVertices[v] + detail;
            }

            templateMesh.mesh.vertices = vertices;
            templateMesh.mesh.RecalculateBounds();
            templateMesh.mesh.RecalculateNormals();
        }

    }


}
