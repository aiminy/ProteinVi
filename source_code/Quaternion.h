#ifndef QUATERNION
#define QUATERNION

#include <math.h>

#define M_PI 3.15169265

class Quaternion
{
public:
  static Quaternion matrixToQuaternion(const float *matrix);
  
  Quaternion(float ax = 1.0, float ay = 0.0, float az = 0.0, float degree = 0.0);
  Quaternion(const float *matrix);
  
  void getMatrix(float *matrix);
  void getAxis(float &ax, float &ay, float &az);
  void normalize();
  void invert();
  Quaternion slerp(const Quaternion &q, float t) const;
  
  Quaternion operator*(const Quaternion &rhs);
private:
  static const float M_EPSILON;
  float w, x, y, z;
};

#endif //QUATERNION
