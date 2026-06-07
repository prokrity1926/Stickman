#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <math.h>
#include "player.h"
#include "staff.h"
#include "fireball.h"
#include "fire.h"
#include "enemy.h"

static bool dragging = false;
static int dragStartX = 0;
static float dragAmount = 0.0f;

static void launchFireballFromThrow(float throwPose);
static bool isEnemyHitByFireball(void);

static void drawSceneBackground(void)
{
    static int lastTime = 0;
    const int currentTime = glutGet(GLUT_ELAPSED_TIME);
    float dt = lastTime == 0 ? 0.0f : (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;
    if (dt > 0.05f)
        dt = 0.05f;

    const float t = currentTime / 1000.0f;
    const float breath = sinf(t * 2.5f) * 0.03f;
    const float throwPose = dragAmount;
    const float sway = dragging ? 0.0f : sinf(t * 2.0f) * 4.0f;
    const float bodyOffset = -throwPose * 0.02f;
    const float staffRotation = sway - throwPose * 35.0f;
    const float baseX = -3.0f;
    const float baseY = -1.20f + breath + bodyOffset;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-4, 4, -3, 3, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glBegin(GL_QUADS);
        glColor3f(0.08f, 0.18f, 0.50f);
        glVertex2f(-4, -3);
        glVertex2f( 4, -3);
        glColor3f(0.50f, 0.78f, 0.96f);
        glVertex2f( 4,  3);
        glVertex2f(-4,  3);
    glEnd();

    glColor3f(0.16f, 0.40f, 0.16f);
    glBegin(GL_QUADS);
        glVertex2f(-4, -3);
        glVertex2f( 4, -3);
        glVertex2f( 4, -1.4f);
        glVertex2f(-4, -1.4f);
    glEnd();

    glColor3f(0.14f, 0.34f, 0.12f);
    glBegin(GL_TRIANGLES);
        glVertex2f(-4, -1.4f); glVertex2f(-1.8f, -1.4f); glVertex2f(-2.7f, 0.1f);
        glVertex2f(-2.2f, -1.4f); glVertex2f(-0.2f, -1.4f); glVertex2f(-0.9f, 0.2f);
        glVertex2f( 0.8f, -1.4f); glVertex2f( 3.6f, -1.4f); glVertex2f( 2.2f, 0.15f);
    glEnd();

    drawPlayer(baseX, baseY, 0.4f, throwPose);
    drawStaff(baseX, baseY, 0.4f, staffRotation, throwPose);
    updateFireball(dt);
    updateEnemy(dt);
    updateFire(dt);

    if (isEnemyHitByFireball())
    {
        // impact handled by enemy and fireball state
    }

    drawFireball();
    drawEnemy();
    drawFire();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

static void resize(int width, int height)
{
    const float ar = (float) width / (float) height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    drawSceneBackground();
    glutSwapBuffers();
}


static void key(unsigned char key, int x, int y)
{
    if (key == 27 || key == 'q')
    {
        exit(0);
    }

    glutPostRedisplay();
}

static void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            dragging = true;
            dragStartX = x;
            dragAmount = 0.0f;
        }
        else if (state == GLUT_UP)
        {
            dragging = false;
            if (dragAmount > 0.0f)
                launchFireballFromThrow(dragAmount);
            dragAmount = 0.0f;
            glutPostRedisplay();
        }
    }
}

static void launchFireballFromThrow(float throwPose)
{
    if (throwPose <= 0.0f)
        return;

    const float baseX = -3.0f;
    const float baseY = -1.20f + -throwPose * 0.02f;
    const float scale = 0.7f;
    const float handX = 0.40f;
    const float handY = 0.20f;
    const float tipY = 0.80f;
    const float staffRotation = -throwPose * 35.0f;
    const float radians = staffRotation * 3.14159265f / 180.0f;
    const float tipDx = -sinf(radians) * tipY;
    const float tipDy = cosf(radians) * tipY;
    const float startX = baseX + scale * (handX + tipDx);
    const float startY = baseY + scale * (handY + tipDy);
    const float launchAngle = radians * 1.5f + 1.57079633f;
    const float speed = 3.5f + throwPose * 5.5f;

    launchFireball(startX, startY, launchAngle, speed);
}

static bool isEnemyHitByFireball(void)
{
    if (!isFireballActive() || !enemyIsAlive())
        return false;

    const float combinedRadius = getFireballRadius() + getEnemyRadius();
    float fx = 0.0f;
    float fy = 0.0f;
    const int count = getFireballCount();

    for (int i = 0; i < count; ++i)
    {
        if (!getFireballPosition(i, &fx, &fy))
            continue;

        const float dx = fx - getEnemyX();
        const float dy = fy - getEnemyY();

        if (dx * dx + dy * dy <= combinedRadius * combinedRadius)
        {
            hitEnemy(25);
            deactivateFireball(i);
            return true;
        }
    }

    return false;
}

static void motion(int x, int y)
{
    if (!dragging)
        return;

    float dx = (float)(dragStartX - x) / 200.0f;
    if (dx < 0.0f)
        dx = 0.0f;
    if (dx > 1.0f)
        dx = 1.0f;
    dragAmount = dx;
    glutPostRedisplay();
}

static void idle(void)
{
    glutPostRedisplay();
}


int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(640,480);
    glutInitWindowPosition(50,50);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

    glutCreateWindow("Stickman Wizard Archer");
    glutFullScreen();

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutIdleFunc(idle);

    glutMainLoop();

    return EXIT_SUCCESS;
}
