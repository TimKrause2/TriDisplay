#include "projection.h"
#include <glm/gtc/matrix_transform.hpp>

void display_projection(
    glm::mat4 Mdisplay,     // basis and translation of the display
    glm::mat4 &Mproj,       // projection matrix for this display
    glm::mat4 &Mview,       // view matrix for this display
    float width,            // width of the display in meters
    float height,           // height of the display in meters
    float z_near,           // near clipping plane in meters
    float z_far)            // far clippling plane in meters
{
    // build the view matrix
    glm::mat3 Mview_rot(Mdisplay);
    Mview = glm::mat4(Mview_rot);
    glm::mat4 Mview_inv = glm::inverse(Mview);
    
    // upper right corner vector
    glm::vec4 Vur(width/2.0f,height/2.0f,0.0,1.0);
    // lower left corner vector
    glm::vec4 Vll(-width/2.0f,-height/2.0f,0.0,1.0);
    
    // resolve the corner vectors in the view space
    glm::vec4 Vur_p = Mview_inv*Mdisplay*Vur;
    glm::vec4 Vll_p = Mview_inv*Mdisplay*Vll;
    float d = -Vur_p.z;
    float right = Vur_p.x * z_near / d;
    float top   = Vur_p.y * z_near / d;
    float left  = Vll_p.x * z_near / d;
    float bottom= Vll_p.y * z_near / d;
    
    Mproj = glm::frustum(left,right,bottom,top,z_near,z_far);
}
