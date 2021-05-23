#include <glm/glm.hpp>

void display_projection(
    glm::mat4 Mdisplay,     // basis and translation of the display
    glm::mat4 &Mproj,       // projection matrix for this display
    glm::mat4 &Mview,       // view matrix for this display
    float width,            // width of the display in meters
    float height,           // height of the display in meters
    float z_near,           // near clipping plane in meters
    float z_far);           // far clippling plane in meters


