using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using System.Linq;


public class ImpBehaviourTex : MonoBehaviour
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

    public string FileNameImageList;
    public string FileName2d;
    public string FileNameNd;

    public Texture[] textures;

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



    public List<Vector2> vertices2d = new List<Vector2>();

    public Vector2 p;

    private bool controlKeyPressed = false;

    public Material material;


    public Vector2 MinCoords
    {
        get { return new Vector2(ImpPlugin.Imp_MinX(), ImpPlugin.Imp_MinY()); }
    }
    public Vector2 MaxCoords
    {
        get { return new Vector2(ImpPlugin.Imp_MaxX(), ImpPlugin.Imp_MaxY()); }
    }



    IEnumerator Start ()
    {
        StartImp();
        CreateTextureAndPassToPlugin();
        yield return StartCoroutine("CallPluginAtEndOfFrames");
    }

    void StartImp()
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


        if (!ImpPlugin.Imp_ExecutePcaImages(FileNameImageList, FileNameNd))
        {
            Debug.LogError("Could not execute pca for images: " + FileNameImageList);
            enabled = false;
            return;
        }

        if (!ImpPlugin.Imp_ExecuteLamp(FileNameNd, FileName2d))
        {
            Debug.LogError("Could not run lamp for " + FileNameNd + ' ' + FileName2d);
            enabled = false;
            return;
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

        ImpPlugin.Imp_BackProjectImageExecute(_p.x, _p.y);

        //ImpPlugin.Imp_CopyQ(q_handle.AddrOfPinnedObject());


    }

    private void CreateTextureAndPassToPlugin()
    {
        // Create a texture
        Texture2D tex = new Texture2D(256, 256, TextureFormat.ARGB32, false);
        //Texture2D tex = new Texture2D(256, 256, TextureFormat.RGB24, false);
        // Set point filtering just so we can see the pixels clearly
        tex.filterMode = FilterMode.Point;
        // Call Apply() so it's actually uploaded to the GPU
        tex.Apply();

        // Set texture onto our material
        material.mainTexture = tex;

        // Pass texture pointer to the plugin
        int channels = (tex.format == TextureFormat.ARGB32) ? 4 : 3;
        ImpPlugin.SetTextureFromUnity(tex.GetNativeTexturePtr(), tex.width, tex.height, channels);
    }


    private IEnumerator CallPluginAtEndOfFrames()
    {
        while (true)
        {
            // Wait until all frame rendering is done
            yield return new WaitForEndOfFrame();

            // Issue a plugin event with arbitrary integer identifier.
            // The plugin can distinguish between different
            // things it needs to do based on this ID.
            // For our simple plugin, it does not matter which ID we pass here.
            GL.IssuePluginEvent(ImpPlugin.GetRenderEventFunc(), 1);
        }
    }
}
