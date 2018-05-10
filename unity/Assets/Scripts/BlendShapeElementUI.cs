using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BlendShapeElementUI : MonoBehaviour
{
    private UnityEngine.UI.Text label;
    private SliderInputField sliderInputField;

    public int Index { get; set; }

    public string Label
    {
        get { return label.text; }
        set { label.text = value; }
    }

    public float Weight
    {
        get { return sliderInputField.Value; }
        set { sliderInputField.Value = value; }
    }

    void Awake ()
    {
        label = gameObject.GetComponentInChildren<UnityEngine.UI.Text>();
        sliderInputField = gameObject.GetComponentInChildren<SliderInputField>();
    }

}
