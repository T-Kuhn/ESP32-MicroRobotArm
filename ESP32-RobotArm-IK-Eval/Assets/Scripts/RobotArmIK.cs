using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TargetPositions
{
    public float Link1 { get; set; }
    public float Link2 { get; set; }
    public float Link3 { get; set; }
    public float Link4 { get; set; }
}

public class RobotArmIK : MonoBehaviour
{
    [SerializeField] float _link1;
    [SerializeField] float _link2;
    [SerializeField] float _link3;
    [SerializeField] float _link4;

    [SerializeField] Transform _target;
    [SerializeField] Transform _rotationTarget;

    void Update()
    {
        var targetAngles = RunIK(_target.position.x, _target.position.y, _rotationTarget.rotation.eulerAngles.z * Mathf.Deg2Rad, true);

        if (targetAngles != null)
        {
            var jointSetter = GetComponent<JointSetter>();
            jointSetter.Joint1Rot = new Vector3(0f, 0f, targetAngles.Link1);
            jointSetter.Joint2Rot = new Vector3(0f, 0f, targetAngles.Link2);
            jointSetter.Joint3Rot = new Vector3(0f, 0f, targetAngles.Link3);
            //jointSetter.Joint4Rot = new Vector3(0f, targetAngles.Link4);
        }
    }

    // - - - - - - - - - - - - - - -
    // - - - - - RUN IK  - - - - - -
    // - - - - - - - - - - - - - - -
    TargetPositions RunIK(float x, float y, float ohm, bool elbowUp)
    {
        Vector2 _P_A, _P_B, _P_C;
        float _g, _f;
        float _phi, _c, _d, _e;
        float _lambda1, _lambda2, _lambda3;
        float _gamma, _alpha, _beta;
        // Note how there's no z-coordinate.

        // 1. Set up P_endeffector and the angle "ohm"
        Vector2 _P_endeffector = new Vector2(elbowUp ? x : -x, y);
        float _ohm = ohm;

        // 2. Calculate point A
        Vector2 P_origin = new Vector2(0.0f, 0.0f);

        _P_A = new Vector2(
            P_origin.x,
            P_origin.y + _link1);

        // 3. Calculate point B
        _g = Mathf.Cos(_ohm) * _link4;
        _f = Mathf.Sin(_ohm) * _link4;
        _P_B = new Vector2(
            _P_endeffector.x - _f,
            _P_endeffector.y + _g);

        // 4. Calculate angle phi
        _d = _P_B.x - _P_A.x;
        _e = _P_B.y - _P_A.y;
        _phi = Mathf.Atan2(_e, _d);

        // 5. Calculate the length of side c
        _c = Mathf.Sqrt(_d * _d + _e * _e);

        // 6. Calculate angles gamma, alpha and beta
        _gamma = Mathf.Acos((_link2 * _link2 + _link3 * _link3 - _c * _c) / (2 * _link2 * _link3));
        _alpha = Mathf.Acos((_link2 * _link2 + _c * _c - _link3 * _link3) / (2 * _link2 * _c));
        _beta = Mathf.PI - _gamma - _alpha;

        // 7. Calculate angles lambda1, lambda2 and lambda3 in the case of "elbow up."
        _lambda1 = Mathf.PI / 2 - (_phi + _alpha);
        _lambda2 = Mathf.PI - _gamma;
        _lambda3 = Mathf.PI - ((Mathf.PI / 2 - _phi) + _beta) - _ohm;

        // 8. Calculate angles lambda1, lambda2 and lambda3 in the case of "elbow down."
        if (!elbowUp)
        {
            _lambda1 = -_lambda1;
            _lambda2 = -_lambda2;
            _lambda3 = -_lambda3;
        }

        var targets = new TargetPositions();

        targets.Link1 = _lambda1; // 0
        targets.Link2 = _lambda2; // 1
        targets.Link3 = _lambda3; // 2

        /*
        Serial.print("lambda1: ");
        Serial.println(_lambda1);
        Serial.print("lambda2: ");
        Serial.println(_lambda2);
        Serial.print("lambda3: ");
        Serial.println(_lambda3);
        */

        if (_lambda1 != _lambda1 || _lambda2 != _lambda2 || _lambda3 != _lambda3)
        {
            Debug.Log("NaN error!");
            return null;
        }

        return targets;
    }
}