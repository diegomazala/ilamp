using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ILampUI : MonoBehaviour
{

    public ILamp ilamp = null;
    public UnityEngine.UI.Image thumbnailTemplate;
    public UnityEngine.UI.Image[] thumbnails;

    bool initializedIlamp = false;

	
	public void Setup(ILamp _ilamp)
    {
        ilamp = _ilamp;

        if (ilamp == null)
        {
            ilamp = (ILamp)FindObjectOfType(typeof(ILamp));

            if (ilamp == null)
            {
                Debug.LogError("Could not find ilamp object");
                enabled = false;
                return;
            }
        }

        if (ilamp.vertices2d.Count < 1)
            return;

        //if (thumbnails.Length != ilamp.vertices2d.Count)
        //{
        //    Debug.LogError("Number of thumbnails does not match vertex count: " + thumbnails.Length + " != " + ilamp.vertices2d.Count);
        //    enabled = false;
        //    return;
        //}

        thumbnails = new UnityEngine.UI.Image[ilamp.vertices2d.Count];
        


        for (int i = 0; i < thumbnails.Length; ++i)
        {
            string thumbName = System.IO.Path.ChangeExtension(ilamp.project.InputFiles[i], "");

            thumbnails[i] = Instantiate<UnityEngine.UI.Image>(thumbnailTemplate, this.transform);
            thumbnails[i].name = thumbName;
            Rect rect = thumbnails[i].rectTransform.rect;
            //Debug.Log("prev: " + thumbnails[i].rectTransform.anchoredPosition);
            Vector2 v = ilamp.vertices2d[i];
            Vector2 pos = new Vector2( 
                LinearInterpolation(v.x, ilamp.MinCoords.x, ilamp.MaxCoords.x, 0, Screen.width - rect.width / 2.0f),
                LinearInterpolation(v.y, ilamp.MinCoords.y, ilamp.MaxCoords.y, 0, Screen.height - rect.height / 2.0f));

            thumbnails[i].rectTransform.anchoredPosition = pos;
            //Debug.Log("post: " + thumbnails[i].rectTransform.anchoredPosition);
        }

        initializedIlamp = true;
    }

    void LoadThumbnail(string thumbName)
    {

    }

    static public float LinearInterpolation(float x, float x0, float x1, float y0, float y1)
    {
        if ((x1 - x0) == 0)
        {
            return (y0 + y1) / 2;
        }
        return y0 + (x - x0) * (y1 - y0) / (x1 - x0);
    }
}
