#include "quad.h"
#include "esShader.h"
#include <glm/gtc/type_ptr.hpp>

#define VERTEX_POS_LOCATION 0
#define VERTEX_UV_LOCATION  1
#define MVP_LOCATION        2
#define TEXTURE_LOCATION    3

Quad* quad;

Quad::Quad(void)
{
    initialized = false;
    
    const char vShaderStr[] =
    "#version 310 es \n"
    "layout(location = 0) in vec2 vertex_pos; \n"
    "layout(location = 1) in vec2 vertex_uv; \n"
    "layout(location = 2) uniform mat4 MVP; \n"
    "out vec2 frag_uv; \n"
    "void main() \n"
    "{\n"
    "   gl_Position = MVP * vec4(vertex_pos, 0.0, 1.0); \n"
    "   frag_uv = vertex_uv; \n"
    "}\n";
    
    const char fShaderStr[] =
    "#version 310 es \n"
    "precision mediump float; \n"
    "in vec2 frag_uv; \n"
    "out vec4 frag_color; \n"
    "layout(location = 3) uniform sampler2D TextureSampler; \n"
    "void main()\n"
    "{\n"
    "   frag_color = texture(TextureSampler, frag_uv); \n"
    "}\n";
    
    GLfloat vertices[2*2*4] =
    {
        -1.0f, -1.0f,
        0.0f, 0.0f,
        
        1.0f, -1.0f,
        1.0f, 0.0f,
        
        1.0f, 1.0f,
        1.0f, 1.0f,
        
        -1.0f, 1.0f,
        0.0f, 1.0f
    };
    
    GLushort indices[2*3] = {0, 1, 2, 0, 2, 3};
    
    programObject = esLoadProgram(vShaderStr, fShaderStr);
    
    if(programObject == 0){
        return;
    }
    
    glGenBuffers(1, &vertexbuffer);
    glGenBuffers(1, &elementbuffer);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
                 vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),
                 indices, GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &vertexarrayobject);
    glBindVertexArray(vertexarrayobject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glEnableVertexAttribArray( VERTEX_POS_LOCATION );
    glVertexAttribPointer( VERTEX_POS_LOCATION, 2,
                           GL_FLOAT, GL_FALSE, 4*sizeof(float),
                           (const void *)0);
    glEnableVertexAttribArray( VERTEX_UV_LOCATION );
    glVertexAttribPointer( VERTEX_UV_LOCATION, 2,
                           GL_FLOAT, GL_FALSE, 4*sizeof(float),
                           (const void *)(sizeof(float)*2));
    glBindVertexArray(0);
    initialized = true;
}

Quad::~Quad(void)
{
    glDeleteProgram( programObject );
    glDeleteBuffers(1, &vertexbuffer );
    glDeleteBuffers(1, &elementbuffer);
    glDeleteVertexArrays(1, &vertexarrayobject);
}

void Quad::Draw(glm::mat4 mvp, GLuint texture)
{
    if(!initialized)return;
    glUseProgram( programObject );
    glBindVertexArray(vertexarrayobject);
    glUniformMatrix4fv(MVP_LOCATION, 1, GL_FALSE,
                       glm::value_ptr(mvp));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(TEXTURE_LOCATION, 0);

    glDrawElements(GL_TRIANGLES, 6,
                   GL_UNSIGNED_SHORT, (const void *)0);
    glUseProgram(0);
    glBindVertexArray(0);
}

void InitQuad(void)
{
    quad = new Quad;
}

void DeleteQuad(void)
{
    delete quad;
}
