// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  TransparenceSetter.cs
//  Author: T-Kuhn.
//  Sapporo, September, 2018. Released into the public domain.
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using DG.Tweening;

namespace RobotArm
{
    public class TransparenceSetter : MonoBehaviour
    {
        public bool MakeTransparent { set { _makeTransparent = value; } }
        [SerializeField] bool _makeTransparent;

        [SerializeField] Renderer[] _targets;

        void Update()
        {
            if (_makeTransparent)
            {


            }
        }
    }
}