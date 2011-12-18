#include <iostream>
#include <assert.h>
#include "example.h"


class TemperatureApp : public ExampleApp {
public:
    TemperatureApp() {
        _shader = loadShader("temperature.rs");
        ren::Vec3 CoolestColor(_shader->getProgram(), "CoolestColor");
        CoolestColor.set(0, 0, 1);
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
        gluSphere(quadric, 5, 12, 12);
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
    return run<TemperatureApp>();
}
