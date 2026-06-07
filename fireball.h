#pragma once

void updateFireball(float dt);
void drawFireball(void);
void launchFireball(float x, float y, float angle, float speed);
bool isFireballActive(void);
int getFireballCount(void);
bool getFireballPosition(int index, float *x, float *y);
float getFireballRadius(void);
void deactivateFireball(int index);
