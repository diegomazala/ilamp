using UnityEngine;
using UnityEditor;
using System.IO;


public class Lamp : EditorWindow
{
    public string[] inputPlyFileNames = null;


    [MenuItem("iLamp/Lamp")]
    static void OpenFileFolder()
    {
        //string path = EditorUtility.OpenFilePanelWithFilters("Load ply models", "G:",  new[] { "PLY files", "ply", "All files", "*" });
        string path = EditorUtility.OpenFolderPanel("Ply models directory", "G:", "*.ply");
        if (path.Length < 1)
            return;

        string[] inputPlyFileNames = Directory.GetFiles(path);

        foreach (string file in inputPlyFileNames)
        {
            if (file.EndsWith(".ply"))
                Debug.Log(file);
        }
    }
}