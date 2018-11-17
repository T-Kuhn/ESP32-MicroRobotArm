using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using DG.Tweening;


namespace RobotArm
{
    public class MovementAnimator : MonoBehaviour
    {
        List<PositionAndEndeffectorAngle> _targetPositions;
        private RobotArm _robotArm;
        private TargetPositions _targetAngles;
        private RobotArmIK _ik;
        private Sequence sequence;

        void Start()
        {
            _robotArm = GetComponent<RobotArm>();
            _ik = GetComponent<RobotArmIK>();
            _targetAngles = new TargetPositions();

            _targetPositions = new List<PositionAndEndeffectorAngle>();

            _targetPositions.Add(new PositionAndEndeffectorAngle {
                Position = new Vector3(0f, 210f, 0f),
                EndeffectorAngle = Mathf.PI,
                ElbowUp = true
            });

            _targetPositions.Add(new PositionAndEndeffectorAngle {
                Position = new Vector3(0f, 210f, 0f),
                EndeffectorAngle = Mathf.PI,
                ElbowUp = false
            });

            /*
            // look up/down
            _targetPositions.Add(new PositionAndEndeffectorAngle {
                Position = new Vector3(134f, 60f, 0f),
                EndeffectorAngle = Mathf.PI / 2f + 0.4f,
                ElbowUp = true
            });

            _targetPositions.Add(new PositionAndEndeffectorAngle {
                Position = new Vector3(144f, 94f, 0f),
                EndeffectorAngle = Mathf.PI / 2f,
                ElbowUp = true
            });

            _targetPositions.Add(new PositionAndEndeffectorAngle {
                Position = new Vector3(144f, 94f, 0f),
                EndeffectorAngle = Mathf.PI / 2f - 0.4f,
                ElbowUp = true
            });

            _targetPositions.Add(new PositionAndEndeffectorAngle {
                Position = new Vector3(144f, 94f, 0f),
                EndeffectorAngle = Mathf.PI / 2f + 0.4f,
                ElbowUp = true
            });
            */

            // drill left
            /*
            _targetPositions.Add(new PositionAndEndeffectorAngle {
                Position = new Vector3(124f, 74f, 0f),
                EndeffectorAngle = Mathf.PI / 2f,
                ElbowUp = true
            });

            _targetPositions.Add(new PositionAndEndeffectorAngle {
                Position = new Vector3(144f, 74f, 0f),
                EndeffectorAngle = Mathf.PI / 2f,
                ElbowUp = true
            });

            _targetPositions.Add(new PositionAndEndeffectorAngle {
                Position = new Vector3(124f, 74f, 0f),
                EndeffectorAngle = Mathf.PI / 2f,
                ElbowUp = true
            });
            */
        }

        private void SetupSequence()
        {
            sequence = DOTween.Sequence();

            var targetAngles = _ik.RunIK(
                102f,
                0f,
                0f,
                true);

            /*
            sequence.Append(
                DOTween.To(() => _robotArm.Joint3Rot.z, x =>
                {
                    var currentRot = _robotArm.Joint3Rot;
                    currentRot.z = x;
                    _robotArm.Joint3Rot = currentRot;
                }, targetAngles.Link3 / 2, 1.7f).SetEase(Ease.InOutSine));

            sequence.Append(
                DOTween.To(() => _robotArm.Joint3Rot.z, x =>
                {
                    var currentRot = _robotArm.Joint3Rot;
                    currentRot.z = x;
                    _robotArm.Joint3Rot = currentRot;
                }, 0f, 1.7f).SetEase(Ease.InOutSine));

            */
            int count = 0;

            foreach (var target in _targetPositions)
            {
                targetAngles = _ik.RunIK(
                target.Position.x,
                target.Position.y,
                target.EndeffectorAngle,
                target.ElbowUp
                );

                sequence.Append(
                    DOTween.To(() => _robotArm.Joint1Rot.z, x =>
                    {
                        var currentRot = _robotArm.Joint1Rot;
                        currentRot.z = x;
                        _robotArm.Joint1Rot = currentRot;
                    }, targetAngles.Link1, 2.5f).SetEase(Ease.InOutSine));

                sequence.Join(
                    DOTween.To(() => _robotArm.Joint2Rot.z, x =>
                    {
                        var currentRot = _robotArm.Joint2Rot;
                        currentRot.z = x;
                        _robotArm.Joint2Rot = currentRot;
                    }, targetAngles.Link2, 2.5f).SetEase(Ease.InOutSine));

                sequence.Join(
                    DOTween.To(() => _robotArm.Joint3Rot.z, x =>
                    {
                        var currentRot = _robotArm.Joint3Rot;
                        currentRot.z = x;
                        _robotArm.Joint3Rot = currentRot;
                    }, targetAngles.Link3, 2.5f).SetEase(Ease.InOutSine));

                if (count == 0)
                {
                    sequence.Join(
                        DOTween.To(() => _robotArm.Joint4Rot.y, x =>
                        {
                            var currentRot = _robotArm.Joint4Rot;
                            currentRot.y = x;
                            _robotArm.Joint4Rot = currentRot;
                        }, Mathf.PI, 2.5f).SetEase(Ease.InOutSine));
                }

                if (count == 1)
                {
                    sequence.Join(
                        DOTween.To(() => _robotArm.Joint4Rot.y, x =>
                        {
                            var currentRot = _robotArm.Joint4Rot;
                            currentRot.y = x;
                            _robotArm.Joint4Rot = currentRot;
                        }, 0, 2.5f).SetEase(Ease.InOutSine));
                }

                count++;
            }

            sequence.Append(
                DOTween.To(() => _robotArm.Joint1Rot.z, x =>
                {
                    var currentRot = _robotArm.Joint1Rot;
                    currentRot.z = x;
                    _robotArm.Joint1Rot = currentRot;
                }, 0f, 2.5f).SetEase(Ease.InOutSine));

            sequence.Join(
                DOTween.To(() => _robotArm.Joint2Rot.z, x =>
                {
                    var currentRot = _robotArm.Joint2Rot;
                    currentRot.z = x;
                    _robotArm.Joint2Rot = currentRot;
                }, 0f, 2.5f).SetEase(Ease.InOutSine));

            sequence.Join(
                DOTween.To(() => _robotArm.Joint3Rot.z, x =>
                {
                    var currentRot = _robotArm.Joint3Rot;
                    currentRot.z = x;
                    _robotArm.Joint3Rot = currentRot;
                }, 0f, 2.5f).SetEase(Ease.InOutSine));

            sequence.Rewind();
        }

        private void Update()
        {
            if (Input.GetKeyDown(KeyCode.M))
            {
                SetupSequence();
                sequence.Rewind(true);
                sequence.Play();
            }
        }
    }


    [System.Serializable]
    public class PositionAndEndeffectorAngle
    {
        public Vector3 Position;
        public float EndeffectorAngle;
        public bool ElbowUp;
    }
}