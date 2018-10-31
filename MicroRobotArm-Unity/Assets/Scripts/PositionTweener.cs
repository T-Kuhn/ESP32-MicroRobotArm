// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  PositionTweener.cs
//  Author: T-Kuhn.
//  Sapporo, September, 2018. Released into the public domain.
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using DG.Tweening;
using UniRx;

namespace RobotArm
{

    [System.Serializable]
    public struct GameObjectAndStartPosDelta
    {
        public GameObject gameObject;
        public Vector3 startPosDelta;
    }

    public class PositionTweener : MonoBehaviour, IAnim
    {
        public bool StartAnim { set { _startTween.Value = value; } }
        ReactiveProperty<bool> _startTween;
        [SerializeField] bool _startTweenToggle;
        [SerializeField] float _tweenDuration = 1f;
        [SerializeField] GameObjectAndStartPosDelta[] _goAndStartPosDeltaArr;
        Tween _movementTween;

        void Start()
        {
            _startTween = new ReactiveProperty<bool>(false);

            _startTween
            .Where(x => x == true)
            .Subscribe(x => StartTweensOnAll());
        }

        void Update()
        {
            if (_startTweenToggle)
            {
                _startTween.Value = false;
                _startTween.Value = _startTweenToggle;
                _startTweenToggle = false;
            }
        }

        void StartTweensOnAll()
        {
            foreach (var data in _goAndStartPosDeltaArr)
            {
                var endPos = data.gameObject.transform.position;
                data.gameObject.transform.position = endPos + data.startPosDelta;
                data.gameObject.transform.DOMove(endPos, _tweenDuration);
            }
        }
    }
}
