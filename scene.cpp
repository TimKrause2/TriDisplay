#include "scene.h"
#include "quad.h"
#include <glm/gtc/matrix_transform.hpp>

#define TEX_WIDTH  8
#define TEX_HEIGHT 8
#define QUAD_SCALE 5.0f

struct Texel
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
};


Scene::Scene(void)
{
    Texel *texels = new Texel[TEX_WIDTH*TEX_HEIGHT];
    Texel *texel = texels;
    for(int y=0;y<TEX_HEIGHT;y++){
        float v = (float)y/(TEX_HEIGHT-1);
        for(int x=0;x<TEX_WIDTH;x++){
            float u = (float)x/(TEX_WIDTH-1);
            switch((x+y)%2){
                case 0:
                    texel->r = (unsigned char)((1.0f-u)*255.0f);
                    texel->g = (unsigned char)(u*255.0f);
                    texel->b = (unsigned char)(v*255.0f);
                    break;
                case 1:
                    texel->r = (unsigned char)(0.25f*255.0f);
                    texel->g = (unsigned char)(0.25f*255.0f);
                    texel->b = (unsigned char)(0.25f*255.0f);
                    break;
            }
            texel++;
        }
    }
    
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEX_WIDTH, TEX_HEIGHT,
                 0, GL_RGB, GL_UNSIGNED_BYTE, texels);
    delete [] texels;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

Scene::~Scene(void)
{
    glDeleteTextures(1, &texture);
}

void Scene::SetModel(glm::mat4 M)
{
    Mmodel = M;
}

void Scene::Draw(glm::mat4 Mview_projection)
{
    glm::mat4 MVP = Mview_projection;
    MVP = MVP * Mmodel;
    MVP = glm::scale(MVP, glm::vec3(QUAD_SCALE, QUAD_SCALE, 1.0f));
    quad->Draw(MVP, texture);
}
