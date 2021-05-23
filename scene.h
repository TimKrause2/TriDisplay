#ifndef SCENE_H
#define SCENE_H

#include <glm/glm.hpp>
#include <GLES3/gl31.h>

class Scene
{
    glm::mat4 Mmodel;
    GLuint texture;
public:
    Scene(void);
    ~Scene(void);
    void SetModel(glm::mat4 Mmodel);
    void Draw(glm::mat4 Mview_projection);
};

#endif
