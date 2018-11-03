// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  AssamblyAnimator.cs
//  Author: T-Kuhn.
//  Sapporo, September, 2018. Released into the public domain.
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UniRx;

namespace RobotArm
{
    public class AssamblyAnimator : MonoBehaviour
    {
        [SerializeField] bool _start;
        [SerializeField] bool _reset;
        [SerializeField] GameObject[] animGOs;
        CompositeDisposable animDisposable = new CompositeDisposable();

        void Update()
        {
            if (Input.GetKeyDown(KeyCode.S))
            {
                _start = true;
            }
            else if (Input.GetKeyDown(KeyCode.R))
            {
                _reset = true;
            }


            if (_start)
            {
                Observable.Timer(System.TimeSpan.FromMilliseconds(800), System.TimeSpan.FromMilliseconds(800))
                .Zip(animGOs.ToObservable(), (x, y) => y)
                    .Subscribe(x =>
                    {
                        var animComponents = x.GetComponents<IAnim>();
                        foreach (var a in animComponents)
                        {
                            a.StartAnim = true;
                        }
                    }).AddTo(animDisposable);

                _start = false;
            }

            if (_reset)
            {
                animDisposable.Clear();
                foreach (var go in animGOs)
                {
                    var animComponents = go.GetComponents<IAnim>();
                    foreach (var a in animComponents)
                    {
                        a.StartAnim = false;
                    }
                }
                _reset = false;
            }
        }
    }

}