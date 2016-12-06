#ifndef LIGHT_H
#define LIGHT_H

class Light
{
public:
  Light();
  ~Light();

  void update();
  void activateRendering();
  void deactivateRendering();
  
  float *getProjMatrix();
  float *getViewMatrix();

  void setModelViewMatrix(float* in);
  float* getModelViewMatrix();

private:
  void initProjMatrix();
  void updateViewMatrix();

  static const float defaultSpeed, defaultRadius, defaultHeight;

  float lastUpdate;
  float speed;
  float angle, radius, height;
  float pos[4];
  float projMatrix[16], viewMatrix[16], modelViewMatrix[16];
};

#endif //LIGHT_H
