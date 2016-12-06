#include "Quaternion.h"

// Public part:

Quaternion Quaternion::matrixToQuaternion(const float *matrix)
{
  Quaternion result(matrix);
  return result;
}

Quaternion::Quaternion(float ax, float ay, float az, float degree)
{
  float halfTheta = M_PI * degree / 360.0;
  float sinHalfTheta = sin(halfTheta);
  w = cos(halfTheta);
  x = ax * sinHalfTheta;
  y = ay * sinHalfTheta;
  z = az * sinHalfTheta;
}

Quaternion::Quaternion(const float *matrix)
{
  float trace = matrix[0] + matrix[5] + matrix[10] + matrix[15];
  float scale;
  if(trace > M_EPSILON)
    {
      scale = 0.5 / sqrt(trace);
      w = 0.25 / scale;
      x = (matrix[6] - matrix[9]) * scale;
      y = (matrix[8] - matrix[2]) * scale;
      z = (matrix[1] - matrix[4]) * scale;
    }
  else if((matrix[0] > matrix[5]) && (matrix[0] > matrix[10]))
    { 
      scale = sqrt(matrix[15] + matrix[0] - matrix[5] - matrix[10]) * 2;
      w = (matrix[9] - matrix[6]) / scale;
      x = 0.25 * scale;
      y = (matrix[4] + matrix[1]) / scale;
      z = (matrix[8] + matrix[2]) / scale;
      
    }
  else if (matrix[5] > matrix[10])
    {
      scale = sqrt(matrix[15] + matrix[5] - matrix[0] - matrix[10]) * 2;
      w = (matrix[8] - matrix[2]) / scale;
      x = (matrix[4] + matrix[1]) / scale;;
      y = 0.25 * scale;
      z = (matrix[9] + matrix[6]) / scale;
    }
  else
    { 
      scale = sqrt(matrix[15] + matrix[10] - matrix[0] - matrix[5]) * 2;
      w = (matrix[4] - matrix[1]) / scale;
      x = (matrix[8] + matrix[2]) / scale;
      y = (matrix[9] + matrix[6]) / scale;
      z = 0.25 * scale;
    }
}

void Quaternion::getAxis(float &ax, float &ay, float &az)
{
  float scale = sqrt(x*x + y*y + z*z);
  ax = x / scale;
  ay = y / scale;
  az = z / scale;
}

void Quaternion::normalize()
{
  float magn = sqrt(w*w + x*x + y*y + z*z);
  w /= magn;
  x /= magn;
  y /= magn;
  z /= magn;
}

void Quaternion::invert()
{
  float magn = w*w + x*x + y*y + z*z;
  if(magn > 0.0)
    {
      magn = 1.0 / magn;
      w *= magn;
      x = -x * magn;
      y = -y * magn;
      z = -z * magn;
    }
}

Quaternion Quaternion::slerp(const Quaternion &q, float t) const
{
  float dot = w*q.w + x*q.x + y*q.y + z*q.z;
  int factor = 1;
  Quaternion result;
  if(dot < 0.0)
    factor = -1;
    
  result.w = (1-t) * w + t * q.w * factor;
  result.x = (1-t) * x + t * q.x * factor;
  result.y = (1-t) * y + t * q.y * factor;
  result.z = (1-t) * z + t * q.z * factor;
  result.normalize();
  return result;
}

void Quaternion::getMatrix(float *matrix)
{
  matrix[0] = 1 - 2*y*y - 2*z*z;
  matrix[1] = 2*x*y + 2*w*z;
  matrix[2] = 2*x*z - 2*w*y;

  matrix[4] = 2*x*y - 2*w*z;
  matrix[5] = 1 - 2*x*x - 2*z*z;
  matrix[6] = 2*y*z + 2*w*x;
  
  matrix[8] = 2*x*z + 2*w*y;
  matrix[9] = 2*y*z - 2*w*x;
  matrix[10] = 1 - 2*x*x - 2*y*y;
  
  matrix[3] = matrix[7] = matrix[11] = matrix[12] = matrix[13] 
    = matrix[14] = 0.0;
  matrix[15] = 1.0;
}

Quaternion Quaternion::operator*(const Quaternion &rhs)
{
  Quaternion result;
  result.w = w*rhs.w - x*rhs.x - y*rhs.y - z*rhs.z;
  result.x = w*rhs.x + x*rhs.w + y*rhs.z - z*rhs.y;
  result.y = w*rhs.y + y*rhs.w + z*rhs.x - x*rhs.z;
  result.z = w*rhs.z + z*rhs.w + x*rhs.y - y*rhs.x;

  return result;
}

// Private part:

const float Quaternion::M_EPSILON = 0.0000001;
