// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  RobotArm.cs
//  Author: T-Kuhn.
//  Sapporo, September, 2018. Released into the public domain.
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace RobotArm
{
    public class RobotArm : MonoBehaviour
    {
        [SerializeField] GameObject _circlePrefab;
        [SerializeField] bool _showLink0Radius;
        [SerializeField] bool _showLink1Radius;
        [SerializeField] bool _showLink2Radius;
        [SerializeField] bool _showLink3Radius;

        [SerializeField] GameObject _joint0;
        [SerializeField] GameObject _joint1;
        [SerializeField] GameObject _joint2;
        [SerializeField] GameObject _joint3;
        [SerializeField] GameObject _joint4;

        GameObject _circle0;
        GameObject _circle1;
        GameObject _circle2;
        GameObject _circle3;

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

        RobotArmIK _robotArmIK;

        void Start()
        {
            _robotArmIK = GetComponent<RobotArmIK>();

            _circle0 = Instantiate(_circlePrefab, _joint0.transform);
            _circle1 = Instantiate(_circlePrefab, _joint1.transform);
            _circle2 = Instantiate(_circlePrefab, _joint3.transform);
            _circle3 = Instantiate(_circlePrefab, _joint3.transform);

            _circle0.transform.localScale = Vector3.one * _robotArmIK.Link1 * 4;
            _circle1.transform.localScale = Vector3.one * _robotArmIK.Link2 * 4;
            _circle2.transform.localScale = Vector3.one * _robotArmIK.Link3 * 4;
            _circle3.transform.localScale = Vector3.one * _robotArmIK.Link4 * 4;
        }

        void Update()
        {
            _joint0.transform.localRotation = Quaternion.Euler(_joint0Rot * 180f / Mathf.PI);
            _joint1.transform.localRotation = Quaternion.Euler(_joint1Rot * 180f / Mathf.PI);
            _joint2.transform.localRotation = Quaternion.Euler(_joint2Rot * 180f / Mathf.PI);
            _joint3.transform.localRotation = Quaternion.Euler(_joint3Rot * 180f / Mathf.PI);
            _joint4.transform.localRotation = Quaternion.Euler(_joint4Rot * 180f / Mathf.PI);

            _circle0.SetActive(_showLink0Radius);
            _circle1.SetActive(_showLink1Radius);
            _circle2.SetActive(_showLink2Radius);
            _circle3.SetActive(_showLink3Radius);
        }
    }
}