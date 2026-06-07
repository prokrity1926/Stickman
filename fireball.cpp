#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>
#include "fireball.h"

static const int maxFireballs = 8;

typedef struct
{
    bool active;
    float x;
    float y;
    float vx;
    float vy;
    float life;
} Fireball;

static Fireball fireballs[maxFireballs] = { 0 };
static const float fireballDuration = 4.5f;
static const float fireballRadius = 0.12f;
static const float fireballGravity = -9.8f;
static const float PI = 3.14159265358979323846f;
static float fireballScale = 0.4f;

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

static Fireball *findAvailableFireball(void)
{
    for (int i = 0; i < maxFireballs; ++i)
    {
        if (!fireballs[i].active)
            return &fireballs[i];
    }
    return NULL;
}

static Fireball *getActiveFireball(int activeIndex)
{
    int count = 0;
    for (int i = 0; i < maxFireballs; ++i)
    {
        if (!fireballs[i].active)
            continue;

        if (count == activeIndex)
            return &fireballs[i];

        ++count;
    }
    return NULL;
}

void launchFireball(float x, float y, float angle, float speed)
{
    Fireball *fireball = findAvailableFireball();
    if (!fireball)
        return;

    fireball->active = true;
    fireball->x = x;
    fireball->y = y;
    fireball->vx = cosf(angle) * speed;
    fireball->vy = sinf(angle) * speed;
    fireball->life = fireballDuration;
}

bool isFireballActive(void)
{
    return getActiveFireball(0) != NULL;
}

int getFireballCount(void)
{
    int count = 0;
    for (int i = 0; i < maxFireballs; ++i)
    {
        if (fireballs[i].active)
            ++count;
    }
    return count;
}

bool getFireballPosition(int index, float *x, float *y)
{
    Fireball *fireball = getActiveFireball(index);
    if (!fireball)
        return false;

    if (x) *x = fireball->x;
    if (y) *y = fireball->y;
    return true;
}

float getFireballRadius(void)
{
    return fireballRadius * fireballScale;
}

void deactivateFireball(int index)
{
    Fireball *fireball = getActiveFireball(index);
    if (fireball)
        fireball->active = false;
}

void updateFireball(float dt)
{
    for (int i = 0; i < maxFireballs; ++i)
    {
        Fireball *fireball = &fireballs[i];
        if (!fireball->active)
            continue;

        fireball->x += fireball->vx * dt;
        fireball->y += fireball->vy * dt + 0.5f * fireballGravity * dt * dt;
        fireball->vy += fireballGravity * dt;
        fireball->life -= dt;

        if (fireball->life <= 0.0f || fireball->x < -4.5f || fireball->x > 4.5f || fireball->y < -3.0f || fireball->y > 4.5f)
            fireball->active = false;
    }
}

void drawFireball(void)
{
    for (int i = 0; i < maxFireballs; ++i)
    {
        Fireball *fireball = &fireballs[i];
        if (!fireball->active)
            continue;

        const float glow = fmaxf(0.0f, fireball->life / fireballDuration);
        const float t = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

        glPushMatrix();
        glTranslatef(fireball->x, fireball->y, 0.0f);

        glScalef(fireballScale, fireballScale, 1.0f);

        // Outer glow
        glColor4f(1.0f, 0.3f, 0.0f, 0.2f);
        fillCircle(0.0f, 0.0f, fireballRadius * 2.2f, 30);

        // Mid glow
        glColor4f(1.0f, 0.6f, 0.0f, 0.4f);
        fillCircle(0.0f, 0.0f, fireballRadius * 1.6f, 30);

        // Core
        float flicker = 0.9f + 0.1f * sinf(t * 20.0f);
        glColor3f(1.0f, 0.5f + 0.5f * glow, 0.0f);
        fillCircle(0.0f, 0.0f, fireballRadius * flicker, 24);

        // Inner bright core
        glColor3f(1.0f, 1.0f, 0.6f);
        fillCircle(0.0f, 0.0f, fireballRadius * 0.4f, 20);

        // Flame spikes
        glBegin(GL_TRIANGLES);
        for (int j = 0; j < 8; ++j)
        {
            float angle = j * (2.0f * PI / 8.0f);
            float pulse = 0.2f * sinf(t * 10.0f + j);

            glColor3f(1.0f, 0.3f, 0.0f);
            glVertex2f(0.0f, 0.0f);

            glVertex2f(cosf(angle - 0.2f) * fireballRadius * 1.2f,
                       sinf(angle - 0.2f) * fireballRadius * 1.2f);

            glVertex2f(cosf(angle) * fireballRadius * (1.8f + pulse),
                       sinf(angle) * fireballRadius * (1.8f + pulse));
        }
        glEnd();

        // Trail
        float angle = atan2f(fireball->vy, fireball->vx);

        glBegin(GL_TRIANGLES);
            glColor4f(1.0f, 0.5f, 0.0f, 0.4f);
            glVertex2f(0.0f, 0.0f);

            glVertex2f(cosf(angle + 2.8f) * fireballRadius * 2.5f,
                       sinf(angle + 2.8f) * fireballRadius * 2.5f);

            glVertex2f(cosf(angle - 2.8f) * fireballRadius * 2.5f,
                       sinf(angle - 2.8f) * fireballRadius * 2.5f);
        glEnd();

        // Small sparks
        for (int j = 0; j < 5; ++j)
        {
            float a = j * 1.2f + t * 5.0f;
            float r = fireballRadius * (1.5f + 0.5f * sinf(t * 5.0f + j));

            glColor3f(1.0f, 1.0f, 0.3f);
            fillCircle(cosf(a) * r, sinf(a) * r, 0.02f, 8);
        }

        glPopMatrix();
    }
}
