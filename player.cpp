#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>
#include "player.h"

static void fillCircle(float cx, float cy, float radius, int segments)
{
    glBegin(GL_TRIANGLE_FAN);
//    glVertex2f(cx, cy);
    for (int i = 0; i <= segments; ++i)
    {
        float theta = 2.0f * 3.14159265f * i / segments;
        glVertex2f(cx + cosf(theta) * radius, cy + sinf(theta) * radius);
    }
    glEnd();
}

void drawPlayer(float x, float y, float scale, float pose)
{
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);

    // Lean animation
    if (pose > 0.0f)
    {
        glTranslatef(pose * 0.05f, 0.0f, 0.0f);
        glRotatef(-pose * 10.0f, 0.0f, 0.0f, 1.0f);
    }

    // Head
    glColor3f(0.85f, 0.75f, 0.65f);
    fillCircle(0.12f, 0.75f, 0.16f, 24);

    // Helmet
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_POLYGON);
    glVertex2f(0.00f, 0.80f);
    glVertex2f(0.25f, 0.80f);
    glVertex2f(0.18f, 0.95f);
    glVertex2f(0.05f, 0.95f);
    glEnd();

    // Eye
    glColor3f(1.0f, 1.0f, 1.0f);
    fillCircle(0.18f, 0.78f, 0.035f, 12);

    // Pupil
    glColor3f(0.0f, 0.0f, 0.0f);
    fillCircle(0.21f, 0.78f, 0.015f, 12);

    // Eyebrow
    glLineWidth(2.5f);
    glBegin(GL_LINES);
    glVertex2f(0.14f, 0.83f);
    glVertex2f(0.23f, 0.80f);
    glEnd();

    // Torso
    glColor3f(0.1f, 0.1f, 0.3f);
    glBegin(GL_POLYGON);
    glVertex2f(-0.03f, 0.55f);
    glVertex2f( 0.25f, 0.55f);
    glColor3f(0.2f, 0.2f, 0.6f); // highlight
    glVertex2f( 0.22f,-0.10f);
    glVertex2f(-0.03f,-0.10f);
    glEnd();

    // Armor outline
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-0.03f, 0.55f);
    glVertex2f( 0.25f, 0.55f);
    glVertex2f( 0.22f,-0.10f);
    glVertex2f(-0.03f,-0.10f);
    glEnd();

    // Belt
    glColor3f(0.8f, 0.6f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(-0.02f, 0.05f);
    glVertex2f( 0.22f, 0.05f);
    glVertex2f( 0.22f, 0.08f);
    glVertex2f(-0.02f, 0.08f);
    glEnd();

    // Arm
    glLineWidth(7.0f);
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_LINES);
    glVertex2f(0.12f, 0.40f);
    glVertex2f(0.48f, 0.22f);
    glEnd();

    // Hand
    glColor3f(0.05f, 0.05f, 0.05f);
    fillCircle(0.48f, 0.22f, 0.05f, 12);

    // Legs
    glLineWidth(6.0f);
    glColor3f(0.1f, 0.1f, 0.3f);

    glBegin(GL_LINES);
    glVertex2f(0.00f, -0.10f);
    glVertex2f(-0.08f, -0.65f);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(0.10f, -0.10f);
    glVertex2f(0.20f, -0.65f);
    glEnd();

    glColor3f(0.05f, 0.05f, 0.05f);
    fillCircle(-0.08f, -0.65f, 0.06f, 12);
    fillCircle( 0.20f, -0.65f, 0.06f, 12);

    // Shoulder pad
    glColor3f(0.6f, 0.6f, 0.7f);
    fillCircle(0.05f, 0.50f, 0.07f, 12);

    glPopMatrix();
}
