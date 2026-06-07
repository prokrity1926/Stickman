#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>
#include "enemy.h"
#include "fire.h"

static float enemyX = 2.5f;
static float enemyYBase = -1.0f;
static float enemyY = -1.0f;
static float enemyScale = 0.4f;
static float enemyAnimTime = 0.0f;
static int enemyHealth = 100;
static const int enemyMaxHealth = 100;
static float impactTime = 0.0f;
static const float impactDuration = 0.18f;
static const float enemyRadius = 0.85f;
static bool enemyDead = false;
static bool enemyBurning = false;
static float burnTime = 0.0f;
static const float burnDuration = 3.0f;
static const float PI = 3.14159265358979323846f;

static void fillCircle(float cx, float cy, float radius, int segments)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segments; ++i)
    {
        float theta = 2.0f * PI * i / segments;
        glVertex2f(cx + cosf(theta) * radius, cy + sinf(theta) * radius);
    }
    glEnd();
}

void updateEnemy(float dt)
{
    enemyAnimTime += dt;
    enemyY = enemyYBase + sinf(enemyAnimTime * 2.5f) * 0.08f;
    if (impactTime > 0.0f)
        impactTime -= dt;

    if (enemyBurning)
    {
        burnTime -= dt;
        if (burnTime <= 0.0f)
        {
            enemyBurning = false;
            enemyDead = true;
        }
    }
}

void drawEnemy(void)
{
    if (enemyDead && !enemyBurning)
        return;

    glPushMatrix();
    glTranslatef(enemyX, enemyY, 0.0f);
    glScalef(enemyScale, enemyScale, 1.0f);

    if (impactTime > 0.0f)
        glTranslatef(sinf(impactTime * 60.0f) * 0.03f, 0.0f, 0.0f);

    {
        const float healthRatio = enemyHealth / (float)enemyMaxHealth;
        const float barWidth = 1.2f;
        const float barHeight = 0.12f;
        const float barY = 1.65f;

        glColor3f(0.1f, 0.1f, 0.1f);
        glBegin(GL_QUADS);
            glVertex2f(-barWidth * 0.5f, barY - barHeight * 0.5f);
            glVertex2f( barWidth * 0.5f, barY - barHeight * 0.5f);
            glVertex2f( barWidth * 0.5f, barY + barHeight * 0.5f);
            glVertex2f(-barWidth * 0.5f, barY + barHeight * 0.5f);
        glEnd();

        if (healthRatio > 0.0f)
        {
            if (healthRatio > 0.66f)
                glColor3f(0.2f, 0.8f, 0.2f);
            else if (healthRatio > 0.33f)
                glColor3f(1.0f, 0.8f, 0.0f);
            else
                glColor3f(0.8f, 0.2f, 0.2f);

            glBegin(GL_QUADS);
                glVertex2f(-barWidth * 0.5f + 0.04f, barY - barHeight * 0.5f + 0.02f);
                glVertex2f(-barWidth * 0.5f + 0.04f + (barWidth - 0.08f) * healthRatio, barY - barHeight * 0.5f + 0.02f);
                glVertex2f(-barWidth * 0.5f + 0.04f + (barWidth - 0.08f) * healthRatio, barY + barHeight * 0.5f - 0.02f);
                glVertex2f(-barWidth * 0.5f + 0.04f, barY + barHeight * 0.5f - 0.02f);
            glEnd();
        }

        glColor3f(1.0f, 1.0f, 1.0f);
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(-barWidth * 0.5f, barY - barHeight * 0.5f);
            glVertex2f( barWidth * 0.5f, barY - barHeight * 0.5f);
            glVertex2f( barWidth * 0.5f, barY + barHeight * 0.5f);
            glVertex2f(-barWidth * 0.5f, barY + barHeight * 0.5f);
        glEnd();
    }

    // Body
    glColor3f(0.4f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
    glVertex2f(-0.28f, 0.55f);
    glVertex2f( 0.05f, 0.55f);
    glColor3f(0.7f, 0.1f, 0.1f);
    glVertex2f( 0.08f,-0.10f);
    glVertex2f(-0.20f,-0.10f);
    glEnd();

    // Neck
    glColor3f(0.5f, 0.05f, 0.05f);
    glBegin(GL_QUADS);
    glVertex2f(-0.10f, 0.35f);
    glVertex2f( 0.05f, 0.35f);
    glVertex2f( 0.05f, 0.55f);
    glVertex2f(-0.10f, 0.55f);
    glEnd();

    // Head
    glColor3f(0.8f, 0.2f, 0.2f);
    fillCircle(-0.12f, 0.75f, 0.20f, 24);

    // Eyebrow
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(-0.25f, 0.85f);
    glVertex2f(-0.10f, 0.80f);
    glEnd();

    // Eye
    glColor3f(1.0f, 1.0f, 1.0f);
    fillCircle(-0.18f, 0.78f, 0.05f, 12);

    // Glowing pupil
    glColor3f(1.0f, 0.0f, 0.0f);
    fillCircle(-0.21f, 0.78f, 0.025f, 12);

    // Mouth
    glColor3f(0.2f, 0.0f, 0.0f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(-0.22f, 0.65f);
    glVertex2f(-0.08f, 0.68f);
    glEnd();

    // Teeth
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.18f, 0.65f);
    glVertex2f(-0.16f, 0.60f);
    glVertex2f(-0.14f, 0.65f);
    glEnd();

    // Horns
    glColor3f(1.0f, 0.9f, 0.3f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.12f, 0.90f);
    glVertex2f(-0.35f, 1.40f);
    glVertex2f(-0.02f, 1.00f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex2f(-0.05f, 0.90f);
    glVertex2f( 0.15f, 1.30f);
    glVertex2f(-0.10f, 1.00f);
    glEnd();

    // Arm
    glColor3f(0.6f, 0.1f, 0.1f);
    glLineWidth(6.0f);
    glBegin(GL_LINES);
    glVertex2f(-0.18f, 0.40f);
    glVertex2f(-0.55f, 0.18f);
    glEnd();

    // Claw
    glColor3f(1.0f, 0.9f, 0.5f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.55f, 0.18f);
    glVertex2f(-0.60f, 0.25f);
    glVertex2f(-0.50f, 0.22f);
    glEnd();

    // Legs
    glLineWidth(6.0f);
    glColor3f(0.5f, 0.08f, 0.08f);

    glBegin(GL_LINES);
    glVertex2f(-0.05f, -0.10f);
    glVertex2f(-0.15f, -0.70f);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(0.00f, -0.10f);
    glVertex2f(0.12f, -0.70f);
    glEnd();

    glColor3f(0.2f, 0.02f, 0.02f);
    fillCircle(-0.15f, -0.70f, 0.06f, 12);
    fillCircle( 0.12f, -0.70f, 0.06f, 12);

    // Back spikes
    glColor3f(1.0f, 0.8f, 0.2f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.28f, 0.40f);
    glVertex2f(-0.40f, 0.60f);
    glVertex2f(-0.20f, 0.55f);

    glVertex2f(-0.25f, 0.20f);
    glVertex2f(-0.38f, 0.35f);
    glVertex2f(-0.18f, 0.30f);
    glEnd();

    glPopMatrix();
}

void hitEnemy(int damage)
{
    if (enemyDead || enemyBurning)
        return;

    enemyHealth -= damage;
    if (enemyHealth <= 0)
    {
        enemyHealth = 0;
        enemyBurning = true;
        burnTime = burnDuration;
        igniteEnemy(enemyX, enemyY);
    }

    impactTime = impactDuration;
}

bool enemyIsAlive(void)
{
    return !enemyDead;
}

float getEnemyX(void)
{
    return enemyX;
}

float getEnemyY(void)
{
    return enemyY;
}

float getEnemyRadius(void)
{
    return enemyRadius * enemyScale;
}
