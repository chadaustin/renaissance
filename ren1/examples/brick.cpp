#include <iostream>
#include <assert.h>
#include "example.h"


class BrickApp : public ExampleApp {
public:
    BrickApp() {
        _down = false;
        _rotX = 0;
        _rotY = 0;
        _wall = true;

        _shader = loadShader("brick.rs");

        ren::Vec3 LightPosition(_shader->getProgram(), "LightPosition");
        ren::Vec3 BrickColor   (_shader->getProgram(), "BrickColor");
        ren::Vec3 MortarColor  (_shader->getProgram(), "MortarColor");
        ren::Vec2 BrickSize    (_shader->getProgram(), "BrickSize");
        ren::Vec2 BrickPct     (_shader->getProgram(), "BrickPct");
        
        BrickColor.set(1.0f, 0.3f, 0.2f);
        MortarColor.set(0.85f, 0.86f, 0.84f);
        BrickSize.set(1.0f, 0.5f);
        BrickPct.set(0.9f, 0.85f);
    }

    void onKeyPress(SDLKey key, bool down) {
        if (down) {
            if (key == SDLK_ESCAPE) {
                quit();
            } else if (key == SDLK_l) {
                ren::Bool EnableLighting(_shader->getProgram(), "EnableLighting");
                EnableLighting = !EnableLighting;
            } else if (key == SDLK_m) {
                _wall = !_wall;
            }
        }
    }

    void onMouseDown(float x, float y) {
        _lastX = x;
        _lastY = y;
        _down = true;
    }

    void onMouseUp(float x, float y) {
        _down = false;
    }

    void onMouseMove(float x, float y) {
        if (_down) {
            _rotX += (x - _lastX);
            _rotY += (y - _lastY);
            _lastX = x;
            _lastY = y;
        }
    }

    void update(float dt) {
    }

    void draw() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(90, float(width) / height, 0.1, 1000.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(0, 0, 10,
                  0, 0, 0,
                  0, 1, 0);

        glEnable(GL_DEPTH_TEST);

        glRotatef(_rotX / 4, 0, 1, 0);
        glRotatef(_rotY / 4, 1, 0, 0);

        GLUquadricObj* quadric = gluNewQuadric();

        _shader->bind();

        if (_wall) {
            glBegin(GL_QUADS);
            glNormal3f(0, 0, 1);
            glVertex2f(-8, -8);
            glVertex2f(-8,  8);
            glVertex2f( 8,  8);
            glVertex2f( 8, -8);
            glEnd();
        } else {
            gluSphere(quadric, 5, 12, 12);
        }

        _shader->unbind();

        glDisable(GL_DEPTH_TEST);

        gluDeleteQuadric(quadric);
    }

private:
    boost::shared_ptr<Shader> _shader;

    bool _down;
    float _lastX;
    float _lastY;

    float _rotX;
    float _rotY;

    bool _wall;
};


// Circumvent SDL.
#ifdef main
#undef main
#endif


int main() {
    return run<BrickApp>();
}
