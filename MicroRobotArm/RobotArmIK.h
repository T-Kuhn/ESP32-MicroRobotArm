/*
  RobotArmIK library
  Author: T-Kuhn.
  Sapporo, September, 2018. Released into the public domain.
  */

#ifndef RobotArmIK_h
#define RobotArmIK_h
#include "Constants.h"
#include "Arduino.h"
#include "MoveBatch.h"

struct Point2D
{
    double x;
    double y;
};

class RobotArmIK
{
  public:
    RobotArmIK(double link1, double link2, double link3, double link4);
    MoveBatch runIK(double x, double y, double ohm, MoveBatch mb, bool elbowUp = true);
    bool nanErrorOccured;

  private:
    double _link1, _link2, _link3, _link4;
};

#endif
