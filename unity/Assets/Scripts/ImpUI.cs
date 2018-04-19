using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ImpUI : MonoBehaviour
{
    public Texture2D mouseTarget;
    public UnityEngine.UI.Image thumbnailTemplate;
    public UnityEngine.Sprite[] sprites;
    private UnityEngine.UI.Image[] thumbnails;

    public void Setup(List<Vector2> Vertices2d, Vector2 MinCoords, Vector2 MaxCoords)
    {
        if (Vertices2d.Count < 1)
            return;

        thumbnails = new UnityEngine.UI.Image[sprites.Length];

        for (int i = 0; i < thumbnails.Length; ++i)
        {
            thumbnails[i] = Instantiate<UnityEngine.UI.Image>(thumbnailTemplate, this.transform);
            thumbnails[i].name = sprites[i].name;
            thumbnails[i].sprite = sprites[i];
            Rect rect = thumbnails[i].rectTransform.rect;
            
            Vector2 v = Vertices2d[i];
            Vector2 pos = new Vector2(
                ImpPlugin.LinearInterpolation(v.x, MinCoords.x, MaxCoords.x, rect.width / 2.0f, Screen.width - rect.width),
                ImpPlugin.LinearInterpolation(v.y, MinCoords.y, MaxCoords.y, rect.height / 2.0f, Screen.height - rect.height));

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
