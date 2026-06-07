#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>
#include "staff.h"

void drawStaff(float x, float y, float scale, float rotation, float pose)
{
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);

    if (pose > 0.0f)
    {
        glTranslatef(pose * 0.05f, 0.0f, 0.0f);
        glRotatef(-pose * 10.0f, 0.0f, 0.0f, 1.0f);
    }

    const float handX = 0.40f;
    const float handY = 0.20f;

    // Pivot at hand
    glTranslatef(handX, handY, 0.0f);
    glRotatef(rotation, 0.0f, 0.0f, 1.0f);

    const float topY = 0.85f;
    const float bottomY = -0.45f;

    float t = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

    // Shaft
    glLineWidth(5.0f);
    glBegin(GL_LINES);
        glColor3f(0.10f, 0.06f, 0.02f);
        glVertex2f(0.0f, bottomY);
        glColor3f(0.25f, 0.15f, 0.05f);
        glVertex2f(0.0f, topY);
    glEnd();

    // Grip
    glColor3f(0.3f, 0.2f, 0.1f);
    for (int i = 0; i < 5; i++)
    {
        float y = -0.1f - i * 0.05f;
        glBegin(GL_LINES);
            glVertex2f(-0.03f, y);
            glVertex2f( 0.03f, y + 0.02f);
        glEnd();
    }

    // Magic glow
    glColor4f(0.2f, 0.6f, 1.0f, 0.25f);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0.0f, topY + 0.25f);
        for (int i = 0; i <= 24; ++i)
        {
            float theta = 2.0f * 3.14159265f * i / 24;
            float r = 0.18f;
            glVertex2f(cosf(theta) * r, topY + 0.25f + sinf(theta) * r);
        }
    glEnd();

    // Crystal
    glColor3f(0.3f, 0.8f, 1.0f);
    glBegin(GL_TRIANGLES);
        glVertex2f(-0.08f, topY + 0.10f);
        glVertex2f( 0.08f, topY + 0.10f);
        glVertex2f( 0.0f,  topY + 0.45f);
    glEnd();

    // Inner crystal shine
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLES);
        glVertex2f(-0.03f, topY + 0.20f);
        glVertex2f( 0.03f, topY + 0.20f);
        glVertex2f( 0.0f,  topY + 0.38f);
    glEnd();

    // Floating orb
    float floatY = topY + 0.55f + sinf(t * 3.0f) * 0.05f;

    glColor3f(0.4f, 0.9f, 1.0f);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0.0f, floatY);
        for (int i = 0; i <= 20; ++i)
        {
            float theta = 2.0f * 3.14159265f * i / 20;
            glVertex2f(cosf(theta) * 0.07f, floatY + sinf(theta) * 0.07f);
        }
    glEnd();

    // Energy particles
    for (int i = 0; i < 4; i++)
    {
        float angle = i * 1.5f + t * 2.0f;
        float r = 0.12f;

        glColor3f(0.6f, 0.9f, 1.0f);
        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(cosf(angle) * r, floatY + sinf(angle) * r);
            for (int j = 0; j <= 10; ++j)
            {
                float th = 2.0f * 3.14159265f * j / 10;
                glVertex2f(cosf(angle) * r + cosf(th)*0.015f,
                           floatY + sinf(angle) * r + sinf(th)*0.015f);
            }
        glEnd();
    }

    glPopMatrix();
}
