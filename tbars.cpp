#include "tbars.h"
#include "esShader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define BAR_WIDTH            0.5
#define VERTEX_POS_LOCATION  0
#define MVP_LOCATION         1
#define COLOR_LOCATION       2

TBars::TBars(void)
{
    for(int i=0;i<N_BARS;i++){
        data[i] = 0.0f;
    }
    i_current = 0;
    
    program = 0;
    vboIds[0] = 0;
    vboIds[1] = 0;
    vaoId = 0;
}

TBars::~TBars(void)
{
    glDeleteProgram( program );
    glDeleteBuffers( 2, vboIds );
    glDeleteVertexArrays( 1, &vaoId );
}

bool TBars::Init(void)
{
    const char vShaderStr[] =
    "#version 310 es                                     \n"
    "layout(location = 0) in vec2 a_position;         \n"
    "layout(location = 1) uniform mat4 MVP;           \n"
    "void main()                                      \n"
    "{                                                \n"
    "   gl_Position = MVP * vec4(a_position,0.0,1.0); \n"
    "}\n";
    
    const char fShaderStr[] =
    "#version 310 es                             \n"
    "precision mediump float;                 \n"
    "layout(location = 2) uniform vec4 color; \n"
    "out vec4 o_fragColor;                    \n"
    "void main()                              \n"
    "{                                        \n"
    "   o_fragColor = color;                  \n"
    "}\n";
    
    GLfloat vertices[4*2] =
    {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };
    
    GLushort indices[2*3] = {0, 1, 2, 0, 2, 3};
    
    program = esLoadProgram( vShaderStr, fShaderStr );
    
    if( program == 0 ){
        return false;
    }
    
    glGenBuffers( 2, vboIds );
    
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0] );
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
                 vertices, GL_STATIC_DRAW );
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[1] );
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),
                 indices, GL_STATIC_DRAW );
    
    glGenVertexArrays( 1, &vaoId );
    
    glBindVertexArray( vaoId );
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0] );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[1] );
    glEnableVertexAttribArray( VERTEX_POS_LOCATION );
    glVertexAttribPointer( VERTEX_POS_LOCATION, 2,
                           GL_FLOAT, GL_FALSE, 2*sizeof(float),
                           (const void *)0);
    
    glBindVertexArray( 0 );
    
    return true;
}

void TBars::Draw(int width, int height)
{
    if(program == 0)return;

    glViewport(0, 0, width, height );
    
    glUseProgram( program );
    
    glBindVertexArray( vaoId );
    
    float aspect = (float)width/(float)height;
    glm::mat4 proj;
    if(aspect >= 1.0){
        float border = (aspect - 1.0f)/2.0f;
        proj = glm::ortho(-border, 1.0f+border,
                          0.0f, 1.0f, -1.0f, 1.0f );
    }else{
        float border = (1.0f/aspect - 1.0f)/2.0f;
        proj = glm::ortho(0.0f, 1.0f,
                          -border, 1.0f+border,
                          -1.0f, 1.0f);
    }
    
    int i = i_current - 1;
    if(i<0)i+=N_BARS;
    float bar_seperation = 1.0/N_BARS;
    float bar_height = bar_seperation*BAR_WIDTH;
    float bar_offset = (bar_seperation-bar_height)/2.0f;
    for(int n=0;n<N_BARS;n++){
        glm::mat4 model(1.0f);
        glm::vec3 trans_v(0.0f, n*bar_seperation+bar_offset, 0.0f);
        model = glm::translate( model, trans_v );
        glm::vec3 scale_v(data[i]*0.5f,bar_height,1.0f);
        model = glm::scale( model, scale_v );
        glUniformMatrix4fv(MVP_LOCATION,
                        1,
                        GL_FALSE,
                        glm::value_ptr(proj*model));
        glUniform4f(COLOR_LOCATION,
                    1.0f, 1.0f, 0.0f, 1.0f);
        glDrawElements( GL_TRIANGLES, 6,
                        GL_UNSIGNED_SHORT, (const void *)0 );
        i--;
        if(i<0)i+=N_BARS;
    }
    
    glUseProgram( 0 );
    
    glBindVertexArray( 0 );
    
}

void TBars::Update(float x)
{
    data[i_current] = x;
    i_current++;
    i_current%=N_BARS;
}
