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

vector<glm::vec3> faces;

float vec3Module(glm::vec3 o, glm::vec3 v) {
    return float(sqrt(pow(o[0] - v[0], 2) + pow(o[1] - v[1], 2) + pow(o[2] - v[2], 2)));
}


float dotProduct(glm::vec3 a, glm::vec3 b) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

glm::vec3 crossProduct(glm::vec3 a, glm::vec3 b) {
    glm::vec3 c(a[0] * b[0], a[1] * b[1], a[2] * b[2]);
    return c;
}



glm::vec3 render_ecuation ( glm::vec3 intersectionPoint, 
                            glm::vec3 light, 
                            glm::vec3 normal,
                            glm::vec3 lightColor,
                            glm::vec3 materialColor,
                            glm::vec3 radiant_color
                            ) {
    float distance = vec3Module(light, intersectionPoint);
    // 
    return lightColor/(distance*distance) * materialColor/float(3.141592653) * radiant_color;
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

bool find_intersection( ray* ray, 
                        GLuint ignoreT) {
    
    glm::vec3 intersectionPoint(0,0,0);
    for ( GLuint t = 0; t < size; t+= 3 ) {
        if (t != ignoreT) {
            float tmp = 0.0;
            glm::vec3 triangle[3] = { faces[t], faces[t+1], faces[t+2] };
            bool interTemp = RayIntersectsTriangle(ray, triangle, intersectionPoint, &tmp);
            if (interTemp) {
                return true;
            }

        }
    }
    return false;
}

glm::vec3    light_source(glm::vec3 normal,
                glm::vec3 light,
                glm::vec3 intersectionPoint,
                glm::vec3 lightColor,
                GLuint tri
                ) {
    
    float cos = glm::dot(normal, light - intersectionPoint);
    if (cos > 0) {
        ray *rayo = new ray(intersectionPoint, light);
        bool shadow = find_intersection(rayo, tri);
        if (shadow) {
            return glm::vec3(0,0,0);
        }
        return lightColor*cos/vec3Module(light, intersectionPoint);
    }
    return glm::vec3(0,0,0);
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
    int image_count = 0;
    for(float z_iter = -20.0 ; z_iter <= 500.0 ; z_iter+= 5) {
        image_count += 1;
        const float st = 0.003;
        const float x_init = -1.0;
        const float x_end = 1.0;
        const float y_init = -1.0;
        const float y_end = 1.0;
        const int image_width = int((x_end - x_init)/st);
        const int image_height = int((y_end - y_init)/st);
        float aspect = float(image_width)/float(image_height);
        PNG png_img = PNG(image_width, image_height); 
        glm::vec3 light(0,2,0);
        glm::vec3 lightColor(1,1,1);
        glm::vec3 materialColor(0.4,0.0,7);
        float world_ph =  0.0;
        float world_th = 30.0;
        float world_ro =  1.0;
        float ph = glm::radians(world_ph);
        float th = glm::radians(world_th);
        glm::vec3 axis(cos(ph)*cos(th),sin(ph)*cos(th),sin(th));
        glm::vec3 to(0,0,0);
        glm::vec3 eye = to + world_ro * axis;
        glm::mat4 camera4 = glm::lookAt(eye,to,glm::vec3(0,0,1));
        glm::mat4 pers = glm::perspective(45.0f,aspect,0.01f,1000.0f);
        glm::mat4 xf = glm::rotate(glm::radians(z_iter),glm::vec3(0.3f,1.0f,0.5f));
        glm::mat4 view = pers*camera4;
        view = view*xf;

        obj.load("model/bunny.obj",view);
        faces = obj.faces();
        const vector<glm::vec3> vertices = obj.vertices();
        const vector<glm::vec3> normals = obj.normals();
        size = obj.faces().size() ;
        glm::vec3 triangles[size][3];
        
        glm::vec3 camera(0,0,-4);
        ray *rays[image_height][image_width];
        glm::vec3 intersectionPoints[image_height][image_width];

        // std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
        float max_m = -1.0;
        for (int j = 0; j < image_height; ++j) {

            for (int i = 0; i < image_width; ++i) {
                glm::vec3 direction(x_init + float(i)*st, y_init + float(j)*st,3);
                rays[j][i] = new ray(camera, direction);
                bool intersects = false;
                bool interTemp = false;
                float tri = 0.0;
                float m = 5000.0;
                glm::vec3 intersectionPoint(-100000.0, -100000.0, -100000.0);
                glm::vec3 bestInter(-100000.0, -100000.0, -100000.0);
                for ( GLuint t = 0; t < size-2; t+= 3 ) {
                    float tmp = 0.0;
                    glm::vec3 triangle[3] = { faces[t], faces[t+1], faces[t+2] };
                    interTemp = RayIntersectsTriangle(rays[j][i], triangle, intersectionPoint, &tmp);
                    if (interTemp) {
                        intersects = true;
                        tmp = vec3Module(camera, intersectionPoint);
                        if (tmp < m) {
                            m = tmp;
                            tri = t;
                            bestInter[0] = intersectionPoint[0];
                            bestInter[1] = intersectionPoint[1];
                            bestInter[2] = intersectionPoint[2];
                        }
                    }
                    

                }
                if (intersects ) {
                    max_m = std::max(max_m, m);
                    
                    glm::vec3 radiant_color = light_source(normals[tri], light, bestInter, lightColor, tri);
                    glm::vec3 pixel_color = render_ecuation ( bestInter, light, normals[tri],lightColor,materialColor, radiant_color);
                    png_img.set(i, image_height - j -1, pixel_color[0], pixel_color[1], pixel_color[2]);
                } else {
                    png_img.set(i, image_height - j - 1,47.0/255, 54.0/255, 64.0/255);
                }
            }
        }
        // std::cerr << "max_m: " << max_m << "\n";
        std::cerr << "z_iter: " << z_iter << "\n";
        
        png_img.save("./images/res" + to_string(image_count) +".png");
        
        // Sleep(150);
    }
    std::cerr << "\nDone.\n";
}