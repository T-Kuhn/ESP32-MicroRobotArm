/*
  RobotArmIK library
  Author: T-Kuhn.
  Sapporo, September, 2018. Released into the public domain.
  */

#include "Arduino.h"
#include "RobotArmIK.h"
#include "Constants.h"
#include "MoveBatch.h"

// - - - - - - - - - - - - - - -
// - - - - CONSTRUCTOR - - - - -
// - - - - - - - - - - - - - - -
RobotArmIK::RobotArmIK(double link1, double link2, double link3, double link4)
{
    // Setup link length.
    _link1 = link1;
    _link2 = link2;
    _link3 = link3;
    _link4 = link4;

    nanErrorOccured = false;
    pinMode(NAN_ALERT_LED, OUTPUT);
}

// - - - - - - - - - - - - - - -
// - - - - - RUN IK  - - - - - -
// - - - - - - - - - - - - - - -
MoveBatch RobotArmIK::runIK(double x, double y, double ohm, MoveBatch mb, bool elbowUp)
{
    Point2D _P_A, _P_B, _P_C;
    double _g, _f;
    double _phi, _c, _d, _e;
    double _lambda1, _lambda2, _lambda3;
    double _gamma, _alpha, _beta;
    // Note how there's no z-coordinate.

    // 1. Set up P_endeffector and the angle "ohm"
    Point2D _P_endeffector = {elbowUp ? x : -x, y};
    double _ohm = ohm;

    // 2. Calculate point A
    Point2D P_origin = {0.0, 0.0};

    _P_A = {P_origin.x,
            P_origin.y + _link1};

    // 3. Calculate point B
    _g = cos(_ohm) * _link4;
    _f = sin(_ohm) * _link4;
    _P_B = {_P_endeffector.x - _f,
            _P_endeffector.y + _g};

    // 4. Calculate angle phi
    _d = _P_B.x - _P_A.x;
    _e = _P_B.y - _P_A.y;
    _phi = atan2(_e, _d);

    // 5. Calculate the length of side c
    _c = sqrt(_d * _d + _e * _e);

    // 6. Calculate angles gamma, alpha and beta
    _gamma = acos((_link2 * _link2 + _link3 * _link3 - _c * _c) / (2 * _link2 * _link3));
    _alpha = acos((_link2 * _link2 + _c * _c - _link3 * _link3) / (2 * _link2 * _c));
    _beta = M_PI - _gamma - _alpha;

    // 7. Calculate angles lambda1, lambda2 and lambda3 in the case of "elbow up."
    _lambda1 = M_PI_2 - (_phi + _alpha);
    _lambda2 = M_PI - _gamma;
    _lambda3 = M_PI - ((M_PI_2 - _phi) + _beta) - _ohm;

    // 8. Calculate angles lambda1, lambda2 and lambda3 in the case of "elbow down."
    if (!elbowUp)
    {
        _lambda1 = -_lambda1;
        _lambda2 = -_lambda2;
        _lambda3 = -_lambda3;
    }

    mb.addMove(/*id:*/ 0, /*pos:*/ (int32_t)(PULSES_PER_REVOLUTION * _lambda1 / M_PI));
    mb.addMove(/*id:*/ 1, /*pos:*/ (int32_t)(PULSES_PER_REVOLUTION * _lambda2 / M_PI));
    mb.addMove(/*id:*/ 2, /*pos:*/ (int32_t)(PULSES_PER_REVOLUTION * _lambda3 / M_PI));

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
        digitalWrite(NAN_ALERT_LED, HIGH);
        nanErrorOccured = true;
        Serial.println("NaN error!");
    }

    return mb;
}
