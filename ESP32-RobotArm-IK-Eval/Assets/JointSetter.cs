using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class JointSetter : MonoBehaviour
{
    [SerializeField] GameObject _joint0;
    [SerializeField] GameObject _joint1;
    [SerializeField] GameObject _joint2;
    [SerializeField] GameObject _joint3;
    [SerializeField] GameObject _joint4;

    [SerializeField] Vector3 _joint0Rot;
    [SerializeField] Vector3 _joint1Rot;
    [SerializeField] Vector3 _joint2Rot;
    [SerializeField] Vector3 _joint3Rot;
    [SerializeField] Vector3 _joint4Rot;

    public Vector3 Joint0Rot { set { _joint0Rot = value; } }
    public Vector3 Joint1Rot { set { _joint1Rot = value; } }
    public Vector3 Joint2Rot { set { _joint2Rot = value; } }
    public Vector3 Joint3Rot { set { _joint3Rot = value; } }
    public Vector3 Joint4Rot { set { _joint4Rot = value; } }

    // Update is called once per frame
    void Update()
    {
        _joint0.transform.localRotation = Quaternion.Euler(_joint0Rot * 180f / Mathf.PI);
        _joint1.transform.localRotation = Quaternion.Euler(_joint1Rot * 180f / Mathf.PI);
        _joint2.transform.localRotation = Quaternion.Euler(_joint2Rot * 180f / Mathf.PI);
        _joint3.transform.localRotation = Quaternion.Euler(_joint3Rot * 180f / Mathf.PI);
        _joint4.transform.localRotation = Quaternion.Euler(_joint4Rot * 180f / Mathf.PI);
    }
}
