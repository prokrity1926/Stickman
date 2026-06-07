#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <math.h>
#include "fire.h"

static bool burning = false;
static bool ashesActive = false;
static float fireX = 0.0f;
static float fireY = -1.20f;
static float burnTimer = 0.0f;
static const float burnDuration = 3.0f;
static const float ashGroundY = -1.4f;
static const int ashCount = 24;

struct AshParticle
{
    float x;
    float y;
    float vx;
    float vy;
    float alpha;
    float size;
};

static AshParticle ashParticles[ashCount];

static float randomFloat(float minValue, float maxValue)
{
    return minValue + (float)rand() / (float)RAND_MAX * (maxValue - minValue);
}

static void spawnAshParticles(void)
{
    ashesActive = true;
    float bodyOffsets[][2] =
    {
        {0.0f, 0.3f},
        {-0.2f, 0.2f},
        {0.2f, 0.2f},
        {0.0f, 0.0f}
    };

    for (int i = 0; i < ashCount; ++i)
    {
        int idx = rand() % 4;

        ashParticles[i].x = fireX + bodyOffsets[idx][0];
        ashParticles[i].y = fireY + bodyOffsets[idx][1];

        ashParticles[i].vx = randomFloat(-0.3f, 0.3f);
        ashParticles[i].vy = randomFloat(-1.0f, -0.2f); // falling
        ashParticles[i].alpha = randomFloat(0.6f, 1.0f);
        ashParticles[i].size = randomFloat(0.03f, 0.08f);
    }
}

void igniteEnemy(float x, float y)
{
    burning = true;
    ashesActive = false;
    fireX = x;
    fireY = y;
    burnTimer = burnDuration;
}

void updateFire(float dt)
{
    if (burning)
    {
        burnTimer -= dt;
        if (burnTimer <= 0.0f)
        {
            burning = false;
            spawnAshParticles();
        }
    }

    if (ashesActive)
    {
        bool anyAlive = false;
        for (int i = 0; i < ashCount; ++i)
        {
            AshParticle *p = &ashParticles[i];

            // 🌬️ Gravity + air resistance
            p->vy -= 6.0f * dt;
            p->vx *= 0.98f;

            // 💨 Wind drift (random motion)
            p->vx += sinf(p->y * 3.0f) * 0.2f * dt;

            p->x += p->vx * dt;
            p->y += p->vy * dt;

            // 🪨 Ground interaction
            if (p->y <= ashGroundY)
            {
                p->y = ashGroundY;
                p->vy = 0.0f;
                p->vx *= 0.9f;
            }

            // 👻 Fade out + shrink
            p->alpha -= dt * 0.15f;
            p->size *= 0.995f;

            if (p->alpha > 0.0f)
                anyAlive = true;
        }

        if (!anyAlive)
            ashesActive = false;
    }
}

static void fillCircle(float cx, float cy, float radius, int segments)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segments; ++i)
    {
        float theta = 2.0f * 3.14159265358979323846f * i / segments;
        glVertex2f(cx + cosf(theta) * radius, cy + sinf(theta) * radius);
    }
    glEnd();
}

void drawFire(void)
{
    if (burning)
    {
        float t = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

        // 🔥 Flame positions relative to enemy body
        float offsets[][2] =
        {
            { 0.0f,  0.3f},   // chest
            {-0.2f,  0.2f},   // left shoulder
            { 0.2f,  0.2f},   // right shoulder
            {-0.15f, 0.0f},   // left torso
            { 0.15f, 0.0f},   // right torso
            { 0.0f, -0.3f}    // lower body
        };

        for (int i = 0; i < 6; i++)
        {
            float ox = offsets[i][0];
            float oy = offsets[i][1];

            float fx = fireX + ox;
            float fy = fireY + oy;

            float flicker = sinf(t * 8.0f + i) * 0.05f;

            // 🔥 Flame shape (upward)
            glBegin(GL_TRIANGLE_FAN);

            // Core (bright)
            glColor4f(1.0f, 1.0f, 0.7f, 1.0f);
            glVertex2f(fx, fy);

            for (int j = 0; j <= 20; j++)
            {
                float a = (j / 20.0f) * 3.1416f;

                float x = fx + cosf(a) * (0.12f + flicker);
                float y = fy + sinf(a) * (0.25f + flicker * 2.0f);

                glColor4f(1.0f, 0.5f, 0.0f, 0.5f);
                glVertex2f(x, y);
            }

            glEnd();
        }

        // 💨 Smoke above enemy
        for (int i = 0; i < 6; i++)
        {
            float drift = sinf(t * 2.0f + i) * 0.15f;
            float rise = fmodf(t * 0.6f + i * 0.3f, 1.5f);

            glColor4f(0.2f, 0.2f, 0.2f, 0.3f - rise * 0.2f);

            fillCircle(
                fireX + drift + ((i % 2 == 0) ? -0.2f : 0.2f), // spread
                fireY + 0.4f + rise,
                0.12f,
                16
            );
        }
    }

    if (ashesActive)
    {
        for (int i = 0; i < ashCount; ++i)
        {
            AshParticle *p = &ashParticles[i];
            if (p->alpha <= 0.0f)
                continue;

            glColor4f(0.25f, 0.25f, 0.25f, p->alpha);
            fillCircle(p->x, p->y, p->size, 12);
        }
    }
}
