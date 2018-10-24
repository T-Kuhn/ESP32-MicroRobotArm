using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// End Point World Position Logger
public class EndPointWPLogger : MonoBehaviour
{
    void Update()
    {
        Debug.Log("Endeffector Position: " + transform.position);
    }
}
