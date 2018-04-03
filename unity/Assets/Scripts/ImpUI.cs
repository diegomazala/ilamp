using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ImpUI : MonoBehaviour
{
    public ImpBehaviour imp = null;
    public Texture2D mouseTarget;
    public UnityEngine.UI.Image thumbnailTemplate;
    public UnityEngine.Sprite[] sprites;
    private UnityEngine.UI.Image[] thumbnails;

    public void Setup(ImpBehaviour _ilamp)
    {
        imp = _ilamp;

        if (imp == null)
        {
            imp = (ImpBehaviour)FindObjectOfType(typeof(ImpBehaviour));

            if (imp == null)
            {
                Debug.LogError("Could not find ilamp object");
                enabled = false;
                return;
            }
        }

        if (imp.vertices2d.Count < 1)
            return;

        thumbnails = new UnityEngine.UI.Image[sprites.Length];

        for (int i = 0; i < thumbnails.Length; ++i)
        {
            thumbnails[i] = Instantiate<UnityEngine.UI.Image>(thumbnailTemplate, this.transform);
            thumbnails[i].name = sprites[i].name;
            thumbnails[i].sprite = sprites[i];
            Rect rect = thumbnails[i].rectTransform.rect;
            
            Vector2 v = imp.vertices2d[i];
            Vector2 pos = new Vector2(
                ImpPlugin.LinearInterpolation(v.x, imp.MinCoords.x, imp.MaxCoords.x, rect.width / 2.0f, Screen.width - rect.width),
                ImpPlugin.LinearInterpolation(v.y, imp.MinCoords.y, imp.MaxCoords.y, rect.height / 2.0f, Screen.height - rect.height));

            thumbnails[i].rectTransform.anchoredPosition = pos;
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
