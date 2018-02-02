using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ILampUI : MonoBehaviour
{

    public ILamp ilamp = null;
    public Texture2D mouseTarget;
    public UnityEngine.UI.Image thumbnailTemplate;
    public UnityEngine.UI.Image[] thumbnails;

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

        thumbnails = new UnityEngine.UI.Image[ilamp.vertices2d.Count];

        for (int i = 0; i < thumbnails.Length; ++i)
        {
            string thumbName = System.IO.Path.GetFileNameWithoutExtension(ilamp.project.InputFiles[i]);

            thumbnails[i] = Instantiate<UnityEngine.UI.Image>(thumbnailTemplate, this.transform);
            thumbnails[i].name = thumbName;
            Rect rect = thumbnails[i].rectTransform.rect;
            
            Vector2 v = ilamp.vertices2d[i];
            Vector2 pos = new Vector2(
                ILamp.LinearInterpolation(v.x, ilamp.MinCoords.x, ilamp.MaxCoords.x, rect.width / 2.0f, Screen.width - rect.width),
                ILamp.LinearInterpolation(v.y, ilamp.MinCoords.y, ilamp.MaxCoords.y, rect.height / 2.0f, Screen.height - rect.height));

            thumbnails[i].rectTransform.anchoredPosition = pos;
            thumbnails[i].sprite = Resources.Load<Sprite>(thumbName);
        }

    }


    void Update()
    {
        if (Input.GetKeyDown(KeyCode.I))
        {
            foreach (var t in thumbnails)
                t.enabled = !t.enabled;
        }
    }

    public CursorMode cursorMode;
    public void MouseTarget(bool on)
    {
        if (on)
            Cursor.SetCursor(mouseTarget, Vector2.zero, cursorMode);
        else
            Cursor.SetCursor(null, Vector2.zero, cursorMode);
    }
}
