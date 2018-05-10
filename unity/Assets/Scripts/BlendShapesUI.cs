using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BlendShapesUI : MonoBehaviour
{
    public SkinnedMeshRenderer SkinnedMesh;
    public BlendShapeElementUI BlendShapeUIElement;
    private List<BlendShapeElementUI> BdsElements;



	void Start ()
    {
		if (SkinnedMesh == null || SkinnedMesh.sharedMesh == null || SkinnedMesh.sharedMesh.blendShapeCount < 1)
        {
            Debug.LogError("Missing reference to BlendShape mesh");
            enabled = false;
            return;
        }

        BdsElements = new List<BlendShapeElementUI>(SkinnedMesh.sharedMesh.blendShapeCount);
        for (int i=0; i< SkinnedMesh.sharedMesh.blendShapeCount; ++i)
        {
            BlendShapeElementUI bds = Instantiate<BlendShapeElementUI>(BlendShapeUIElement, this.transform);
            BdsElements.Add(bds);
            bds.Index = i;
            bds.Label = SkinnedMesh.sharedMesh.GetBlendShapeName(i);
            bds.Weight = SkinnedMesh.GetBlendShapeWeight(i);
        }
    }


    private void Update()
    {
        for (int i = 0; i < SkinnedMesh.sharedMesh.blendShapeCount; ++i)
        {
            BlendShapeElementUI bds = BdsElements[i];
            SkinnedMesh.SetBlendShapeWeight(bds.Index, bds.Weight);
        }
    }

}
