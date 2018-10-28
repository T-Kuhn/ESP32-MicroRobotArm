// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  EndPointWPLogger.cs
//  Author: T-Kuhn.
//  Sapporo, September, 2018. Released into the public domain.
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace RobotArm
{
    //End Point World Position Logger
    public class EndPointWPLogger : MonoBehaviour
    {
        void Update()
        {
            Debug.Log("Endeffector Position: " + transform.position);
        }
    }
}