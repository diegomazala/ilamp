using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public class FloatEvent : UnityEngine.Events.UnityEvent<float>
{
}

public class SliderInputField : MonoBehaviour
{
    private UnityEngine.UI.Slider slider;
    private UnityEngine.UI.InputField inputField;

    public FloatEvent OnValueChanged;

    public float Value
    {
        get { return slider.value; }
        set { UpdateValueFromFloat(value); }
    }

    void Awake()
    {
        slider = gameObject.GetComponentInChildren<UnityEngine.UI.Slider>();
        inputField = gameObject.GetComponentInChildren<UnityEngine.UI.InputField>();
    }

    public void UpdateValueFromFloat(float value)
    {
        if (slider) { slider.value = value; }
        if (inputField) { inputField.text = System.String.Format(System.Globalization.CultureInfo.InvariantCulture, "{0:0.00}", value); }

        OnValueChanged.Invoke(value);
    }

    public void UpdateValueFromString(string value)
    {
        if (slider) { slider.value = float.Parse(value, System.Globalization.CultureInfo.InvariantCulture); }
        if (inputField) { inputField.text = value; }
    }


}
