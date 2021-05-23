#include "tridisplay.h"
#include "projection.h"
#include "quad.h"
#include <cstdio>
#include <glm/gtc/matrix_transform.hpp>

#define D_VIEW 0.58f
#define Z_NEAR 0.10f
#define Z_FAR  10.0f
#define DPM    1000

Display::Display(float width, float height):
Mdisplay(1.0f),
Mview(1.0f),
Mproj(1.0f),
width(width),
height(height),
texWidth((GLsizei)(width*DPM)),
texHeight((GLsizei)(height*DPM))
{
    initialized = false;
    glGenFramebuffers(1, &FramebufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    
    glGenTextures(1, &renderedTexture);
    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight,
               0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24,
                          texWidth, texHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, depthrenderbuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           renderedTexture, 0);
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);
    
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE){
        printf("Framebuffer error:");
        const char *statusStr;
        switch(status){
            case 0:
                statusStr = "Error";
                break;
            case GL_FRAMEBUFFER_UNDEFINED:
                statusStr = "GL_FRAMEBUFFER_UNDEFINED";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                statusStr = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                statusStr = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
                break;
            case GL_FRAMEBUFFER_UNSUPPORTED:
                statusStr = "GL_FRAMEBUFFER_UNSUPPORTED";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
                statusStr = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
                break;
            default:
                statusStr = "Unkown Error";
                break;
        }
        printf("%s\n",statusStr);
    }else{
        initialized = true;
    }
}

Display::~Display(void)
{
    if(!initialized) return;
    glDeleteFramebuffers(1, &FramebufferName);
    glDeleteTextures(1, &renderedTexture);
    glDeleteRenderbuffers(1, &depthrenderbuffer);
}

void Display::SetMdisplay(glm::mat4 Mdisplay)
{
    Display::Mdisplay = Mdisplay;
    display_projection(
        Mdisplay,
        Mproj,
        Mview,
        width,
        height,
        Z_NEAR, Z_FAR );
}

void Display::RenderTexture(Scene* scene)
{
    if(!initialized) return;
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    glViewport(0,0,texWidth,texHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glm::mat4 Mview_projection = Mproj * glm::inverse(Mview);
    scene->Draw(Mview_projection);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Display::Draw(glm::mat4 Mview_projection)
{
    if(!initialized) return;
    glm::mat4 Mmodel = glm::scale(Mdisplay,
                                  glm::vec3(width/2.0f,height/2.0f,1.0f));
    glm::mat4 MVP = Mview_projection * Mmodel;
    quad->Draw(MVP, renderedTexture);
}

glm::mat4 rotate_about_point(glm::mat4 M,
                             glm::vec3 P, 
                             glm::vec3 N, 
                             float omega)
{
    glm::mat4 Mt = glm::translate(glm::mat4(1.0f), P);
    Mt = glm::rotate(Mt, omega, N);
    Mt = glm::translate(Mt, -P);
    return Mt*M;
}

TriDisplay::TriDisplay(float width, float height)
{
    for(int i=0;i<3;i++){
        displays[i] = new Display(width, height);
    }
    
    glm::vec3 vd0(0.0f, 0.0f, -D_VIEW);
    glm::vec3 vd1d0(-width, 0.0f, 0.0f);
    glm::vec3 vd2d0(width, 0.0f, 0.0f);
    
    glm::mat4 Mdisplay0 = glm::translate(glm::mat4(1.0f), vd0);
    glm::mat4 Mdisplay1 = glm::translate(glm::mat4(1.0f), vd0+vd1d0);
    glm::mat4 Mdisplay2 = glm::translate(glm::mat4(1.0f), vd0+vd2d0);
    
    Mdisplay1 = rotate_about_point(Mdisplay1,
                                   glm::vec3(-width/2.0f,0.0f,-D_VIEW),
                                   glm::vec3(0.0f,1.0f,0.0f),
                                   glm::radians(45.0f));
    Mdisplay2 = rotate_about_point(Mdisplay2,
                                   glm::vec3(width/2.0f,0.0f,-D_VIEW),
                                   glm::vec3(0.0f,1.0f,0.0f),
                                   glm::radians(-45.0f));
    
    displays[0]->SetMdisplay( Mdisplay0 );
    displays[1]->SetMdisplay( Mdisplay1 );
    displays[2]->SetMdisplay( Mdisplay2 );
}

TriDisplay::~TriDisplay(void)
{
    for(int i=0;i<3;i++){
        delete displays[i];
    }
}

void TriDisplay::RenderTextures(Scene *scene)
{
    for(int i=0;i<3;i++){
        displays[i]->RenderTexture(scene);
    }
}

void TriDisplay::Draw(glm::mat4 Mview_projection)
{
    for(int i=0;i<3;i++){
        displays[i]->Draw(Mview_projection);
    }
}

