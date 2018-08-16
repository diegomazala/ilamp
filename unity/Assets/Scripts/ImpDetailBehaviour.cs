using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using System.Linq;
using mattatz.MeshSmoothingSystem;


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

    //private float[] q_data = null;
    private Vector3[] q_data = null;
    private GCHandle q_handle;
    private Vector3[] q_data_laplace = null;
    private GCHandle q_handle_laplace;

    public bool RunLamp = true;

    public string FileName2d;
    public string FileNameNd;

    [Range(0, 100)]
    public int laplaceIterations = 0;
    [Range(0, 6)]
    public int levelIndex = 0;
    [Range(0, 7)]
    public int meshTargetIndex = 0;
    public MeshFilter[] baseMeshes;

    [System.Serializable]
    public class MeshFilterList
    {
        public string FileName;
        public MeshFilter[] MeshList;
    }
    public List<MeshFilterList> laplaceMeshes;

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
        if (templateMesh == null)
        {
            Debug.LogError("Template Mesh is null. Did you forget to fill inspector fields?");
            enabled = false;
            return;
        }

        //
        // Check if the number of meshes and vertices matches
        //
        foreach (var m in baseMeshes)
        {
            if (m == null)
            {
                Debug.LogError("Mesh is null. Did you forget to fill inspector fields?");
                enabled = false;
                return;
            }
        }
        foreach (var ml in laplaceMeshes)
        {
            if (baseMeshes.Length != ml.MeshList.Length)
            {
                Debug.LogError("The number of meshes does not match: " + baseMeshes.Length + " != " + ml.MeshList.Length);
                enabled = false;
                return;
            }

            if (ml.MeshList[0] == null)
            {
                Debug.LogError("Mesh is null. Did you forget to fill inspector fields?");
                enabled = false;
                return;
            }

            var vertCount = ml.MeshList[0].mesh.vertexCount;
            for (int i=1; i<ml.MeshList.Length; ++i)
            {
                if (ml.MeshList[i] == null)
                {
                    Debug.LogError("Mesh null. Did you forget to fill inspector fields?");
                    enabled = false;
                    return;
                }

                if (vertCount != ml.MeshList[i].mesh.vertexCount)
                {
                    Debug.LogError("The number of vertices does not match: " + ml.MeshList[i].name);
                    enabled = false;
                    return;
                }
            }
        }

        //Debug.Log("Building ILamp for base meshes: " + FileNameNd);
        if (!BuildILamp(baseMeshes, FileName2d, FileNameNd))
        {
            enabled = false;
            return;
        }

        foreach (var ml in laplaceMeshes)
        {
            //Debug.Log("Building ILamp for laplace meshes: " + ml.FileName);
            if (!BuildILamp(ml.MeshList, FileName2d, ml.FileName))
            {
                enabled = false;
                return;
            }
        }
        ImpPlugin.Imp_SetCurrent(0);


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
                //q_data = new float[ImpPlugin.Imp_QRows() * ImpPlugin.Imp_QCols()];
                q_data = new Vector3[templateMesh.mesh.vertexCount];
                q_handle = GCHandle.Alloc(q_data, GCHandleType.Pinned);
            }

            if (q_data_laplace == null) 
            {
                q_data_laplace = new Vector3[templateMesh.mesh.vertexCount];
                q_handle_laplace = GCHandle.Alloc(q_data_laplace, GCHandleType.Pinned);
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

    }


    private void OnDisable()
    {
        if (q_data != null)
            q_handle.Free();
        if (q_data_laplace != null)
            q_handle_laplace.Free();
    }



    void Update()
    {
        // 
        // Fixing possible OutOfRange
        //
        levelIndex = levelIndex % laplaceMeshes.Count;
        meshTargetIndex = meshTargetIndex % laplaceMeshes[levelIndex].MeshList.Length;

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
                    p = vertices2d[v % vertices2d.Count];

                    MeshRenderer source = baseMeshes[v % vertices2d.Count].GetComponent<MeshRenderer>();
                    if (target && source)
                        target.sharedMaterial = source.sharedMaterial;

                    meshTargetIndex = v % laplaceMeshes.Count;

                    Execute(p);
                }
                ++v;
            }

            if (Input.GetKeyDown(KeyCode.Alpha0))
            {
                target.sharedMaterial = wireframe;
            }
        }

        // +1 is because of laplaceMeshes + baseMeshes
        for (var key = KeyCode.Keypad0; key <= KeyCode.Keypad0 + laplaceMeshes.Count + 1; ++key)
        {
            if (Input.GetKeyDown(key))
            {
                ImpPlugin.Imp_SetCurrent(key - KeyCode.Keypad0);
            }
        }
        



        switch (levelIndex)
        {
            default:
            case 0: laplaceIterations = 0; break;
            case 1: laplaceIterations = 2; break;
            case 2: laplaceIterations = 5; break;
            case 3: laplaceIterations = 10; break;
            case 4: laplaceIterations = 25; break;
            case 5: laplaceIterations = 50; break;
            case 6: laplaceIterations = 100; break;
        }


        if (Input.GetKeyDown(KeyCode.Return) || Input.GetKeyDown(KeyCode.Space))
            Execute(p);

        //Execute(p);
    }

    public void Execute()
    {
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

        //
        // Computing laplace meshes blend
        //
        int currentImp = ImpPlugin.Imp_GetCurrent();
        { 
            ImpPlugin.Imp_SetCurrent(levelIndex + 1); // +1 is because the level 0 is for baseMeshes

            if (!ImpPlugin.Imp_Execute(_p.x, _p.y))
            {
                Debug.LogError("Could not execute imp : " + _p.ToString());
                return;
            }
            ImpPlugin.Imp_CopyQ(q_handle_laplace.AddrOfPinnedObject());
        }
        ImpPlugin.Imp_SetCurrent(currentImp);


        Vector3[] vertTarget = laplaceMeshes[levelIndex].MeshList[meshTargetIndex].mesh.vertices;
        Vector3[] vertResult = templateMesh.mesh.vertices;

        for (long v = 0; v < templateMesh.mesh.vertexCount; ++v)
        {
            Vector3 vertBlend = q_data[v] * ModelScaleFactor;
            Vector3 vertBlendLaplace = q_data_laplace[v] * ModelScaleFactor;
            vertResult[v] = vertBlend - vertBlendLaplace + vertTarget[v];
        }

        templateMesh.mesh.vertices = vertResult;
        templateMesh.mesh.RecalculateBounds();
        templateMesh.mesh.RecalculateNormals();
    }

    bool BuildILamp(MeshFilter[] meshes, string filename2d, string filenameNd)
    {
        if (ImpType == ImpTypeEnum.ILamp)
        {
            ImpPlugin.Imp_Create_ILamp();
            ImpPlugin.Imp_ILamp_Setup(ILampParams.KdTreeCount, ILampParams.NumNeighbours, ILampParams.KnnSearchChecks);
        }
        else
        {
            ImpPlugin.Imp_Create_Rbp();
            ImpPlugin.Imp_Rbf_Setup((ushort)(RbfParams.Function), RbfParams.Constant);
        }


        System.Globalization.CultureInfo customCulture = (System.Globalization.CultureInfo)System.Threading.Thread.CurrentThread.CurrentCulture.Clone();
        customCulture.NumberFormat.NumberDecimalSeparator = ".";
        System.Threading.Thread.CurrentThread.CurrentCulture = customCulture;


        if (RunLamp)
        {
            ImpPlugin.BuildNdFile(meshes, filenameNd, 1.0f / ModelScaleFactor);

            if (!ImpPlugin.Imp_ExecuteLamp(filenameNd, filename2d))
            {
                Debug.LogError("Could not run lamp for " + filename2d + ' ' + filenameNd);
                enabled = false;
                return false;
            }
        }

        if (!ImpPlugin.Imp_LoadInputFiles(filename2d, filenameNd))
        {
            Debug.LogError("Could not load input files: " + filename2d + ' ' + FileNameNd);
            enabled = false;
            return false;
        }


        if (!ImpPlugin.Imp_Build())
        {
            Debug.LogError("Could not build imp");
            enabled = false;
            return false;
        }

        return true;

    }
}
