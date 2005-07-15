#include <iostream>
#include <assert.h>
#include "example.h"


class BrickApp : public ExampleApp {
public:
    BrickApp() {
        _shader = loadShader("demo.rs");

        /*
        ren::Vec3 LightPosition(_shader->getProgram(), "LightPosition");
        ren::Vec3 BrickColor   (_shader->getProgram(), "BrickColor");
        ren::Vec3 MortarColor  (_shader->getProgram(), "MortarColor");
        ren::Vec2 BrickSize    (_shader->getProgram(), "BrickSize");
        ren::Vec2 BrickPct     (_shader->getProgram(), "BrickPct");
        
        BrickColor.set(1.0f, 0.3f, 0.2f);
        MortarColor.set(0.85f, 0.86f, 0.84f);
        BrickSize.set(1.0f, 0.5f);
        BrickPct.set(0.9f, 0.85f);
        */
    }

    void onKeyPress(SDLKey key, bool down) {
        if (down && key == SDLK_ESCAPE) {
            quit();
        }
    }

    void update(float dt) {
    }

    void draw() {
        glClear(GL_COLOR_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(90, float(width) / height, 0.1, 1000.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(0, 0, 10,
                  0, 0, 0,
                  0, 1, 0);

        GLUquadricObj* quadric = gluNewQuadric();

        _shader->bind();

        glBegin(GL_QUADS);
        glColor3f(1, 1, 0);
        glVertex2f(-4, -4);

        glColor3f(1, 0, 1);
        glVertex2f(-4,  4);

        glColor3f(0, 1, 1);
        glVertex2f( 4,  4);

        glColor3f(0, 1, 0);
        glVertex2f( 4, -4);
        glEnd();

        //gluSphere(quadric, 5, 12, 12);
        _shader->unbind();

        gluDeleteQuadric(quadric);
    }

private:
    boost::shared_ptr<Shader> _shader;
};


// Circumvent SDL.
#ifdef main
#undef main
#endif


int main() {
    return run<BrickApp>();
}
