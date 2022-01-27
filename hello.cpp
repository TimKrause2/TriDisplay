#include "SDL.h"
#include <GLES3/gl31.h>
#include <stdio.h>
#include <time.h>
#include "tbars.h"
#include "scene.h"
#include "quad.h"
#include "tridisplay.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define D_MODEL 5.0f
#define Z_NEAR  0.1f
#define Z_FAR   10.0f
#define FOV     90.0f

int main(int argc, char * argv[])
{
    if(SDL_Init(SDL_INIT_VIDEO)!=0){
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }
    
    printf("Platform:%s\n",SDL_GetPlatform());
    
    printf("Current video driver:%s\n",SDL_GetCurrentVideoDriver());
    
    SDL_Window *window;
    window = SDL_CreateWindow(
        "Hello",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        320,
        240,
        SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    
    if(window==NULL){
        printf("Couldn't create window: %s\n",SDL_GetError());
        SDL_Quit();
        return 1;
    }
    
    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
    
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);
    
    if(glcontext==NULL){
        printf("Couldn't create GL context: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    if(SDL_GL_SetSwapInterval(1)==-1){
        printf("Swap Interval not supported.\n");
    }
    
    SDL_ShowWindow(window);
    
    int width;
    int height;
    SDL_GetWindowSize(window, &width, &height);
    
    int running=1;
    SDL_Event event;
    int frame=0;
    struct timespec t_last;
    struct timespec t_now;
    TBars *tbars = new TBars;
    InitQuad();
    Scene *scene = new Scene;
    glm::mat4 Mcamera(1.0f);
    scene->SetModel(glm::translate(glm::mat4(1.0f),glm::vec3(0.0f,0.0f,-D_MODEL)));

    float hdpi;
    float vdpi;
    SDL_GetDisplayDPI(0, NULL, &hdpi, &vdpi);
    float hdpm = hdpi / 0.0254f;
    float vdpm = vdpi / 0.0254f;
    SDL_Rect rect;
    SDL_GetDisplayBounds(0, &rect);
    float d_width = (float)rect.w/hdpm;
    float d_height = (float)rect.h/vdpm;
    TriDisplay* tridisplay = new TriDisplay(d_width,d_height);

    if(!tbars->Init()){
        printf("Couldn't initialize TBars.\n");
        SDL_GL_DeleteContext(glcontext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    clock_gettime( CLOCK_MONOTONIC, &t_last );
    
    while(running){
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_KEYDOWN:
                    if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) running=0;
                    break;
                case SDL_QUIT:
                    running=0;
                    break;
                case SDL_WINDOWEVENT:
                    switch(event.window.event){
                        case SDL_WINDOWEVENT_RESIZED:
                        case SDL_WINDOWEVENT_SIZE_CHANGED:
                            width = event.window.data1;
                            height = event.window.data2;
                            break;
                        default:
                            break;
                    }
                default:
                    break;
            }
        }
        tridisplay->RenderTextures(scene);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        tbars->Draw(width, height);
        glViewport(0, 0, width, height );
        float aspect = (float)width/height;
        glm::mat4 Mproj;
        float fov = glm::radians(FOV);
        if(aspect>=1.0){
            Mproj = glm::perspective(fov,aspect,Z_NEAR,Z_FAR);
        }else{
            fov = glm::atan(glm::tan(fov/2.0f)/aspect)*2.0f;
            Mproj = glm::perspective(fov,aspect,Z_NEAR,Z_FAR);
        }
        glm::mat4 Mview = glm::inverse(Mcamera);
        glm::mat4 Mview_projection = Mproj * Mview;
        if(frame&32)scene->Draw(Mview_projection);
        tridisplay->Draw(Mview_projection);
        
        SDL_GL_SwapWindow(window);
        clock_gettime( CLOCK_MONOTONIC, &t_now );
        long dt_sec = t_now.tv_sec - t_last.tv_sec;
        long dt_nsec = t_now.tv_nsec - t_last.tv_nsec;
        if(dt_nsec < 0 ){
            dt_nsec += 1000000000;
            dt_sec--;
        }
        float x = dt_sec + (float)dt_nsec*1e-9;
        x*=60.0;
        tbars->Update(x);
        //printf("delta t:%lf\n",(double)(dt_sec)+(double)(dt_nsec)*1e-9);
        //printf("frame:%d\n",frame);
        frame++;
        t_last = t_now;
    }
    delete tbars;
    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
