using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraRotater : MonoBehaviour
{

    [SerializeField] Transform _centerOfRotation;
    [SerializeField] float _speed;
    [SerializeField] float _radius;


    void Update()
    {
        transform.position = new Vector3(
            _centerOfRotation.position.x + Mathf.Sin(Time.time * _speed) * _radius,
            transform.position.y,
            _centerOfRotation.position.z + Mathf.Cos(Time.time * _speed) * _radius
        );

        transform.LookAt(_centerOfRotation);

        transform.rotation = Quaternion.Euler(
            0f,
            transform.rotation.eulerAngles.y,
            transform.rotation.eulerAngles.z
        );
    }
}
