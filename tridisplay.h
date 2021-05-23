#include <glm/glm.hpp>
#include <GLES3/gl31.h>
#include "scene.h"
#include "quad.h"

class Display
{
    glm::mat4 Mdisplay;
    glm::mat4 Mview;
    glm::mat4 Mproj;
    float width;
    float height;
    GLsizei texWidth;
    GLsizei texHeight;
    bool initialized;
    GLuint FramebufferName;
    GLuint renderedTexture;
    GLuint depthrenderbuffer;
public:
    Display(float width, float height);
    ~Display();
    void SetMdisplay(glm::mat4 Mdisplay);
    void RenderTexture(Scene *scene);
    void Draw(glm::mat4 Mview_projection);
};

class TriDisplay
{
    Display *displays[3];
public:
    TriDisplay(float width, float height);
    ~TriDisplay(void);
    void RenderTextures(Scene *scene);
    void Draw(glm::mat4 Mview_projection);
};
