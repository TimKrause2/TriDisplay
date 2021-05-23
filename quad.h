#ifndef QUAD_H
#define QUAD_H

#include <glm/glm.hpp>
#include <GLES3/gl31.h>

class Quad
{
    bool initialized;
    GLuint programObject;
    GLuint vertexbuffer;
    GLuint elementbuffer;
    GLuint vertexarrayobject;
public:
    Quad(void);
    ~Quad(void);
    void Draw(glm::mat4 mvp, GLuint texture);
};

extern Quad* quad;

void InitQuad(void);
void DeleteQuad(void);

#endif // QUAD_H
