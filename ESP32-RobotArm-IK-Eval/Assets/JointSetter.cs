using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class JointSetter : MonoBehaviour
{
    [SerializeField] GameObject joint1;
    [SerializeField] GameObject joint2;
    [SerializeField] GameObject joint3;
    [SerializeField] GameObject joint4;
    [SerializeField] GameObject joint5;

    [SerializeField] Vector3 joint1Rot;
    [SerializeField] Vector3 joint2Rot;
    [SerializeField] Vector3 joint3Rot;
    [SerializeField] Vector3 joint4Rot;
    [SerializeField] Vector3 joint5Rot;

    // Update is called once per frame
    void Update()
    {
        joint1.transform.localRotation = Quaternion.Euler(joint1Rot * 180f / Mathf.PI);
        joint2.transform.localRotation = Quaternion.Euler(joint2Rot * 180f / Mathf.PI);
        joint3.transform.localRotation = Quaternion.Euler(joint3Rot * 180f / Mathf.PI);
        joint4.transform.localRotation = Quaternion.Euler(joint4Rot * 180f / Mathf.PI);
        joint5.transform.localRotation = Quaternion.Euler(joint5Rot * 180f / Mathf.PI);
    }
}
