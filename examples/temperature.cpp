#include <fstream>
#include <iostream>
#include <stdexcept>
#include <assert.h>
#include <boost/scoped_array.hpp>
#include <SDL.h>
#include "glew.h"
#include <ren/Compiler.h>
#include <ren/Input.h>


inline void throwSDLError(const std::string& prefix) {
    throw std::runtime_error(prefix + ": " + SDL_GetError());
}


const char* getErrorString(GLenum error) {
    switch (error) {
        case GL_INVALID_ENUM:      return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE:     return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
        case GL_STACK_OVERFLOW:    return "GL_STACK_OVERFLOW";
        case GL_STACK_UNDERFLOW:   return "GL_STACK_UNDERFLOW";
        case GL_OUT_OF_MEMORY:     return "GL_OUT_OF_MEMORY";
        case GL_TABLE_TOO_LARGE:   return "GL_TABLE_TOO_LARGE";
        default:                   return "Unknown error";
    }
}


void checkOpenGLErrors() {
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        throw std::runtime_error(getErrorString(error));
    }
}


void loadShader(GLhandleARB shader, const std::string& source) {
    const GLcharARB* sources[] = { source.data()   };
    GLint lengths[]            = { source.length() };
    glShaderSourceARB(shader, 1, sources, lengths);
    glCompileShaderARB(shader);

    GLint compiled;
    glGetObjectParameterivARB(shader, GL_OBJECT_COMPILE_STATUS_ARB, &compiled);
    if (!compiled) {
        GLint length;
        glGetObjectParameterivARB(shader, GL_OBJECT_INFO_LOG_LENGTH_ARB,
                                  &length);
        boost::scoped_array<GLcharARB> infoLog(new GLcharARB[length]);
        glGetInfoLogARB(shader, length, 0, infoLog.get());
        throw std::runtime_error(std::string("GLSL Compile error: ") +
                                 infoLog.get());
    }

    checkOpenGLErrors();
}


class Shader {
public:
    Shader(ren::ProgramPtr program)
    : _program(program) {
    }

    ren::ProgramPtr getProgram() const {
        return _program;
    }

    void bind() {
        if (!_program) {
            unbind();
        }

        const ren::Program::ValueList& constants = _program->getConstantValues();
        const ren::Program::ValueList& uniforms  = _program->getUniformValues();
        if (!_cache.count(constants)) {
            _cache[constants].program = compile();
        }
        _cache[constants].uniforms = uniforms;

        GLhandleARB programObject = _cache[constants].program;
        glUseProgramObjectARB(programObject);

        for (size_t i = 0; i < _program->getUniforms().size(); ++i) {
            const ren::Input& u = _program->getUniforms()[i];

            GLint location = glGetUniformLocationARB(
                programObject,
                u.getName().c_str());
            ren::ValuePtr uniformValue = _program->getUniformValue(
                u.getName());

            if (u.getType() == ren::VEC3) {
                glUniform3fvARB(location, 1, uniformValue->asFloatVec());
            }
        }
    }

    static void unbind() {
        glUseProgramObjectARB(0);
    }

private:
    GLhandleARB compile() {
        std::ostringstream log;
        ren::CompileResult cr(ren::compile(_program, log));
        if (!cr.success) {
            throw std::runtime_error("Error compiling shader:\n" + log.str());
        }
        std::cout << "Compile successful.\n"
                  << "Vertex Shader\n----\n"
                  << cr.vertexShader
                  << "----\n"
                  << "Fragment Shader\n----\n"
                  << cr.fragmentShader
                  << std::endl;

        GLhandleARB vertexShader = glCreateShaderObjectARB(
            GL_VERTEX_SHADER_ARB);
        loadShader(vertexShader, cr.vertexShader);

        GLhandleARB fragmentShader = glCreateShaderObjectARB(
            GL_FRAGMENT_SHADER_ARB);
        loadShader(fragmentShader, cr.fragmentShader);

        GLhandleARB program = glCreateProgramObjectARB();
        glAttachObjectARB(program, vertexShader);
        glAttachObjectARB(program, fragmentShader);
        glLinkProgramARB(program);

        GLint linked;
        glGetObjectParameterivARB(program, GL_OBJECT_LINK_STATUS_ARB, &linked);
        if (!linked) {
            GLint length;
            glGetObjectParameterivARB(program, GL_OBJECT_INFO_LOG_LENGTH_ARB, &length);
            boost::scoped_array<GLcharARB> infoLog(new GLcharARB[length]);
            glGetInfoLogARB(program, length, 0, infoLog.get());
            throw std::runtime_error(std::string("GLSL Link error: ")
                                     + infoLog.get());
        }

        checkOpenGLErrors();

        return program;
    }

    ren::ProgramPtr _program;

    /**
     * A compiled and linked program and its set of current uniforms.
     */
    struct CacheEntry {
        CacheEntry()
        : program(0) {
        }

        GLhandleARB program;
        ren::Program::ValueList uniforms;
    };

    /**
     * Compare the *values* of the constants, not the pointers.
     */
    struct ValueListComparator {
        bool operator()(const ren::Program::ValueList& a,
                        const ren::Program::ValueList& b) const {
            // The Program shouldn't have any new constants added as
            // this is running...  This would be better enforced in
            // the type system somewhere.
            assert(a.size() == b.size());

            /*
            if (a.size() < b.size()) { return true;  }
            if (b.size() < a.size()) { return false; }
            */
            
            for (size_t i = 0; i < a.size(); ++i) {
                if (*a[i] < *b[i]) { return true;  }
                if (*b[i] < *a[i]) { return false; }
            }
            return false;
        }
    };

    typedef std::map<ren::Program::ValueList, CacheEntry,
                     ValueListComparator> ShaderCache;
    ShaderCache _cache;
};
typedef boost::shared_ptr<Shader> ShaderPtr;


ShaderPtr loadShader(const std::string& filename) {
    std::ifstream is(filename.c_str());
    if (!is) {
        throw std::runtime_error("Could not open shader: " + filename);
    }
    return ShaderPtr(new Shader(ren::parse(is)));
}


class TemperatureApp {
public:
    static const int width = 1024;
    static const int height = 768;

    TemperatureApp()
    : _shouldQuit(false) {
        int initflags = SDL_INIT_NOPARACHUTE | SDL_INIT_VIDEO |
                        SDL_INIT_TIMER | SDL_INIT_JOYSTICK;
        if (SDL_Init(initflags) < 0) {
            throwSDLError("SDL initialization failed");
        }

        // Define our minimum requirements for the GL window.
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE,     5);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,   5);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,    5);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,   16);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        SDL_WM_SetCaption("Renaissance Example", 0);

        int mode = SDL_OPENGL | SDL_ANYFORMAT;
        if (!SDL_SetVideoMode(width, height, 32, mode)) {
            SDL_Quit();
            throwSDLError("Setting video mode failed");
        }

        GLenum glewError = glewInit();
        if (glewError != GLEW_OK) {
            SDL_Quit();
            std::string error = "glewInit() failed: ";
            error += reinterpret_cast<const char*>(glewGetErrorString(glewError));
            throw std::runtime_error(error.c_str());
        }

        if (!GLEW_ARB_shader_objects ||
            !GLEW_ARB_shading_language_100 ||
            !GLEW_ARB_vertex_shader ||
            !GLEW_ARB_fragment_shader
        ) {
            SDL_Quit();
            throw std::runtime_error("OpenGL Shading Language not supported.");
        }

        _shader = loadShader("temperature.rs");
        ren::Vec3 CoolestColor(_shader->getProgram(), "CoolestColor");
        CoolestColor.set(0, 0, 1);
    }

    ~TemperatureApp() {
        SDL_Quit();
    }

    int run() {
        float last_time = getNow();
        while (!_shouldQuit) {
            float now = getNow();

            if (handleEvents()) {
                break;
            }

            // Ignore timer wraparound.
            if (now >= last_time) {
                float dt = now - last_time;
                update(dt);
                draw();
                SDL_GL_SwapBuffers();
            }

            last_time = now;
        }
        
        return 0;
    }

    void onKeyPress(SDLKey key, bool down) {
        if (down && key == SDLK_ESCAPE) {
            _shouldQuit = true;
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



        glColor3f(1.0f, 0, 0);

        float array[] = {
            1.0f, 2.0f, -2,
            3, 4, -1,
            7, 1, 7,
            8, 2, -4,
            2, 3, 7 };
            

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, array);
        glDrawArrays(GL_LINE_STRIP, 0, 5);
        glDisableClientState(GL_VERTEX_ARRAY);

#if 0
        GLUquadricObj* quadric = gluNewQuadric();

        _shader->bind();
        gluSphere(quadric, 5, 12, 12);
        _shader->unbind();

        gluDeleteQuadric(quadric);
#endif
    }

private:
    static float getNow() {
        return SDL_GetTicks() / 1000.0f;
    }

    bool handleEvents() {
        bool should_quit = false;

        SDL_Event event;
        int result = SDL_PollEvent(&event);
        while (result == 1) {
            switch (event.type) {
                case SDL_VIDEORESIZE:
                    assert(!"SDL_VIDEORESIZE should never occur.");
                    break;

                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    onKeyPress(
                        event.key.keysym.sym,
                        event.key.state == SDL_PRESSED);
                    break;

                case SDL_QUIT:
                    should_quit = true;
                    break;
            }

            result = SDL_PollEvent(&event);
        }

        return result < 0 || should_quit;
    }

    bool _shouldQuit;
    boost::shared_ptr<Shader> _shader;
};


template<typename App>
int run() {
    try {
        return App().run();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return -1;
}


// Circumvent SDL.
#ifdef main
#undef main
#endif


int main() {
    return run<TemperatureApp>();
}
