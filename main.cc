#define GLM_ENABLE_EXPERIMENTAL
#include "color.h"
#include "ray.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/polar_coordinates.hpp>
#include <glm/gtx/string_cast.hpp>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "obj.h"
#include "png.h"
#include <iostream>
#include<windows.h>

GLuint size;

OBJ obj;

float dotProduct(glm::vec3 a, glm::vec3 b) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

glm::vec3 crossProduct(glm::vec3 a, glm::vec3 b) {
    glm::vec3 c(a[0] * b[0], a[1] * b[1], a[2] * b[2]);
    return c;
}




float vec3Module(glm::vec3 o, glm::vec3 v) {
    return float(sqrt(pow(o[0] - v[0], 2) + pow(o[1] - v[1], 2) + pow(o[2] - v[2], 2)));
}


bool RayIntersectsTriangle(ray* pixelRay,
                           glm::vec3* inTriangle,
                           glm::vec3& outIntersectionPoint,
                           float* module)
{
    const float EPSILON = 0.0000001;
    glm::vec3 vertex0 = inTriangle[0];
    glm::vec3 vertex1 = inTriangle[1];
    glm::vec3 vertex2 = inTriangle[2];
    glm::vec3 edge1, edge2, h, s, q;
    float a,f,u,v;
    edge1 = vertex1 - vertex0;
    edge2 = vertex2 - vertex0;
    h = glm::cross(pixelRay->direction(),edge2);
    a = glm::dot(edge1, h);
    if (a > -EPSILON && a < EPSILON)
        return false;    // This ray is parallel to this triangle.
    f = 1.0/a;
    s = pixelRay->origin() - vertex0;
    u = f * glm::dot(s, h);
    if (u < 0.0 || u > 1.0)
        return false;
    q = glm::cross(s, edge1);
    v = f * glm::dot(pixelRay->direction(), q);
    if (v < 0.0 || u + v > 1.0)
        return false;
    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = f * glm::dot(edge2, q);
    if (t > EPSILON) // ray intersection
    {
        outIntersectionPoint = pixelRay->at(t);
        return true;
    }
    else {// This means that there is a line intersection but not a ray intersection.
        return false;
    }
}

void print_triangle( glm::vec3* triangle) {
    std::cerr << "p[0]" << glm::to_string(triangle[0]) << "\n";
    std::cerr << "p[1]" << glm::to_string(triangle[1]) << "\n";
    std::cerr << "p[2]" << glm::to_string(triangle[2]) << "\n\n\n";
}
void print_faces( vector<glm::vec3> faces ) {
    GLuint size = faces.size()/3;

    for (unsigned int x = 0; x < size; x++) {
        glm::vec3 triangle[3] = {faces[x*3], faces[x*3 + 1], faces[x*3 + 2] };
        print_triangle(triangle);
    }
}


int main() {

    for(float z_iter = -2.0 ; z_iter <= 5.0 ; z_iter+= 0.2) {
    const float st = 0.005;
    const float x_init = -1.0;
    const float x_end = 1.0;
    const float y_init = -1.0;
    const float y_end = 1.0;
    const int image_width = int((x_end - x_init)/st);
    const int image_height = int((y_end - y_init)/st);
	float aspect = float(image_width)/float(image_height);
    PNG png_img = PNG(image_width, image_height); 
    glm::vec3 light(0,10,0)
    float world_ph =  10.0;
    float world_th = 45.0;
    float world_ro =  1.0;
	float ph = glm::radians(world_ph);
	float th = glm::radians(world_th);
	glm::vec3 axis(cos(ph)*cos(th),sin(ph)*cos(th),sin(th));
	glm::vec3 to(0,0,0);
	glm::vec3 eye = to + world_ro * axis;
	glm::mat4 camera4 = glm::lookAt(eye,to,glm::vec3(0,0,1));
	glm::mat4 pers = glm::perspective(45.0f,aspect,0.01f,1000.0f);
	glm::mat4 view = pers*camera4;
    view = glm::translate(view, glm::vec3(-2,-0.5,-2));

	obj.load("model/cube.obj",view);
    const vector<glm::vec3> faces = obj.faces();
    const vector<glm::vec3> vertices = obj.vertices();
    const vector<glm::vec3> normals = obj.normals();
    size = obj.faces().size() ;
    glm::vec3 triangles[size][3];
    
    glm::vec3 camera(0,0,0);
    for (unsigned int x = 0; x < size; x++) {
        for (unsigned int y = 0; y < 3; y++) {
            triangles[x][y] = faces[x + y];
        }
    }
    ray *rayo = new ray();
    ray *rays[image_height][image_width];
    glm::vec3 intersectionPoints[image_height][image_width];

    // std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    float max_m = -1.0;
    for (int j = 0; j < image_height; ++j) {
        for (int i = 0; i < image_width; ++i) {
            glm::vec3 direction(x_init + float(i)*st, y_init + float(j)*st,2);
            // std::cerr << "direction" << to_string(direction) << "\n";
            rays[j][i] = new ray(camera, direction);
            bool intersects = false;
            float tri = 0.0;
            float m = 5000.0;
            glm::vec3 intersectionPoint(-100000.0, -100000.0, -100000.0);
            for ( GLuint t = 0; t < size; t+= 3 ) {
                float tmp = 0.0;
                bool interTemp = false;
                interTemp = RayIntersectsTriangle(rays[j][i], triangles[t], intersectionPoint, &tmp);
                if (interTemp) {
                    intersects = true;
                    m = std::min(m, vec3Module(camera, intersectionPoint));
                }
            }
            if (intersects ) {
                max_m = std::max(max_m, m);
                png_img.set(i, image_height - j -1, 0, 3.0/(m*m), 10.0/(m*m));
            } else {
                glm::vec3 pixel_color(0.0, 0.0, 0.0);
                png_img.set(i, image_height - j - 1, 0.0, 0.0, 0.0);
            }
        }
    }
    // std::cerr << "max_m: " << max_m << "\n";
    std::cerr << "z_iter: " << z_iter << "\n";
    
    png_img.save("./res.png");
    
    Sleep(150);
    }
    std::cerr << "\nDone.\n";
}