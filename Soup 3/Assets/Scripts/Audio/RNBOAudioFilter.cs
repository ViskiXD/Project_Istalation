using System.Collections.Generic;
using UnityEngine;

// Attach to any AudioSource you want processed by the RNBO SpaceEcho patch.
[RequireComponent(typeof(AudioSource))]
public class RNBOAudioFilter : MonoBehaviour
{
    public static readonly List<RNBOAudioFilter> ActiveFilters = new List<RNBOAudioFilter>();

    [HideInInspector]
    public SpaceEchoHandle handle;

    void OnEnable()
    {
        if (!ActiveFilters.Contains(this)) ActiveFilters.Add(this);
    }

    void OnDisable()
    {
        ActiveFilters.Remove(this);
    }

    void Start()
    {
        if (handle == null)
        {
            handle = new SpaceEchoHandle();
        }
    }

    void OnAudioFilterRead(float[] data, int channels)
    {
        if (handle == null) return;
        handle.Process(data, channels);
    }
}


