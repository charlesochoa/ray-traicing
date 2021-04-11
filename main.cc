#include "color.h"
#include "ray.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/polar_coordinates.hpp>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "obj.h"
#include <iostream>

unsigned int size;

OBJ obj;

float dotProduct(glm::vec3 a, glm::vec3 b) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

bool RayIntersectsTriangle(ray* pixelRay,
                           glm::vec3* inTriangle,
                           glm::vec3* outIntersectionPoint)
{
    const float EPSILON = 0.0000001;
    glm::vec3 vertex0 = inTriangle[0];
    glm::vec3 vertex1 = inTriangle[1];
    glm::vec3 vertex2 = inTriangle[2];
    glm::vec3 edge1, edge2, h, s, q;
    float a,f,u,v;
    edge1 = vertex1 - vertex0;
    edge2 = vertex2 - vertex0;
    h = pixelRay->direction()*edge2;
    a = dotProduct(edge1, h);
    if (a > -EPSILON && a < EPSILON)
        return false;    // This ray is parallel to this triangle.
    f = 1.0/a;
    s = pixelRay->origin() - vertex0;
    u = f * dotProduct(s, h);
    if (u < 0.0 || u > 1.0)
        return false;
    q = s * edge1;
    v = f * dotProduct(pixelRay->direction(), q);
    if (v < 0.0 || u + v > 1.0)
        return false;
    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = f * dotProduct(edge2, q);
    if (t > EPSILON) // ray intersection
    {
        // outIntersectionPoint = pixelRay->at(t);
        return true;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return false;
}


// void interpole(glm:vec3* dir) {
//     dir[0] = (dir[0] - 172.0)/384.0;
//     dir[1] = (dir[1] - 100.0)/384.0;
//     dir[2] = -3.0;
// }




int main() {

    // Image
    const int image_width = 384;
    const int image_height = 256;

    const float ai = -4.0;
    const float as = 4.0;
    const float bi = -2.0;
    const float ci = -3.0;
    const float st = 8.0/384.0;
    
	glm::mat4 xf = glm::rotate(glm::radians(90.0f),glm::vec3(1.0f,0.0f,0.0f));

	obj.load("../model/bunny.obj",xf);
    const vector<glm::vec3> faces = obj.faces();
    size = obj.faces().size() ;
    
    ray *rayo = new ray();
    // Render
    ray *rays[image_height][image_width];
    glm::vec3 intersectionPoints[image_height][image_width];

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    glm::vec3 camera(0.0, 0.0, 8.0);
    for (int j = image_height-1; j >= 0; --j) {
        // std::cerr << "\rSize: " << size << ' ' ;
        // std::cin.ignore();
        for (int i = 0; i < image_width; ++i) {
            
            glm::vec3 direction(ai + float(i)*st, bi + float(j)*st, 3.0);
            rays[j][i] = new ray(camera, direction);
            bool intersects = false;
            glm::vec3 intersectionPoint(0.0, 0.0, 0.0);
            for ( unsigned int t = 0; t < size; t++ ) {
                // std::cerr << "\rt: " << t << ' ' ;
                // std::cin.ignore();
                glm::vec3 triangle[3] =  {faces[t],faces[t+1],faces[t+2]};
                intersects = RayIntersectsTriangle(rays[j][i], triangle, &intersectionPoint);
                // std::cerr << "ray: " << rays[j][i];

                if (intersects) {
                    break;
                }
            }
            if (intersects) {
                glm::vec3 pixel_color(255.0, 0.0, 0.0);
                write_color(std::cout, pixel_color);
            } else {
                glm::vec3 pixel_color(0.0, 0.0, 0.0);
                write_color(std::cout, pixel_color);
            }
        }
    }

    std::cerr << "\nDone.\n";
}