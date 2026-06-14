#pragma once

glm::vec3 DoNothing(triangle* tri, int depth, glm::vec3 p, glm::vec3 dir)
{
    return vec3(0);
}

glm::vec3 Shade(triangle* tri, int depth, glm::vec3 p, glm::vec3 dir)
{
	glm::vec3 v12 = tri->v2.pos - tri->v1.pos;
	glm::vec3 v13 = tri->v3.pos - tri->v1.pos;
	//normal of the triangle plane
	glm::vec3 N = glm::normalize(glm::cross(v12, v13));

	//ambient
	glm::vec3 col = 0.1f * tri->v1.col;

	//diffuse
	glm::vec3 L_vec = light_pos - p;
	float dist = glm::length(L_vec);
	glm::vec3 L = glm::normalize(L_vec);

	//shadow ray
	float t_shadow;
	glm::vec3 shadow_col = vec3(0);
	trace(p + (N * 0.001f), L, t_shadow, shadow_col, depth, DoNothing);

	//if not in shadow
    if (t_shadow > dist) {
        float i_diff = glm::dot(N, L);
        if (i_diff < 0.f) 
        {
			i_diff = 0.f;
        }
		col += i_diff * tri->v1.col;
    }

	//reflection
    if (tri->reflect && depth < max_recursion_depth)
    {
		glm::vec3 R = glm::reflect(dir, N);
        float t_reflect;
        glm::vec3 reflect_col = vec3(0);
        trace(p + (N * 0.0001f), R, t_reflect, reflect_col, depth + 1, Shade);
        col += reflect_col;
	}

    return col;
}

bool PointInTriangle(glm::vec3 pt, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3)
{
    glm::vec3 v12 = v2 - v1;
    glm::vec3 v13 = v3 - v1;
	//normal of the plane
    glm::vec3 N = glm::normalize(glm::cross(v12, v13));

    //edge Vector
    glm::vec3 edge1 = v2 - v1;
    //point Vector
	glm::vec3 vp1 = pt - v1;
    //perpendicular Vector
    glm::vec3 c1 = glm::cross(edge1, vp1);
	//inside Test
    if (glm::dot(N, c1) < 0) 
    { 
        return false; 
    }

    //edge Vector
    glm::vec3 edge2 = v3 - v2;
    //point Vector
    glm::vec3 vp2 = pt - v2;
    //perpendicular Vector
    glm::vec3 c2 = glm::cross(edge2, vp2);
    //inside Test
    if (glm::dot(N, c2) < 0)
    {
        return false;
    }

    //edge Vector
    glm::vec3 edge3 = v1 - v3;
    //point Vector
    glm::vec3 vp3 = pt - v3;
    //perpendicular Vector
    glm::vec3 c3 = glm::cross(edge3, vp3);
    //inside Test
    if (glm::dot(N, c3) < 0)
    {
        return false;
	}

	//if inside all 3 edges, return true
    return true;
}

float RayTriangleIntersection(glm::vec3 o, glm::vec3 dir, triangle* tri, glm::vec3& point)
{
    //calculate the v12, v13 vectors connecting the vertices
	glm::vec3 v12 = tri->v2.pos - tri->v1.pos;
	glm::vec3 v13 = tri->v3.pos - tri->v1.pos;
    //calculate the normal of the plane N, the cross product of v12, v13
	glm::vec3 N = glm::cross(v12, v13);

    //calcuate intersection distance
    glm::vec3 p = tri->v1.pos;
    float adjacent = glm::dot(N, p - o);
    float denom = glm::dot(N, dir);
	float t = adjacent / denom;
	if (t < 0)
    {
        return FLT_MAX;
    }

    //calcuate point q at which the ray intersects the plane
	point = o + dir * t;

	//test if point of intersection is inside triangle
    if (PointInTriangle(point, tri->v1.pos, tri->v2.pos, tri->v3.pos))
    {
        return t; // hit
	}

	return FLT_MAX; // miss
}

void trace(glm::vec3 o, glm::vec3 dir, float& t, glm::vec3& io_col, int depth, closest_hit p_hit)
{
    //initialization
    t = FLT_MAX; //very large number
	triangle* closest_tri = nullptr; //no tri has been hit
	glm::vec3 closest_point = glm::vec3(0); //stores intersection point of closest hit

    //for each tri in triangles
    for (int i = 0; i < tris.size(); ++i)
    {
        //check if ray hits tri, returns distance
		glm::vec3 point;
        float t_current = RayTriangleIntersection(o, dir, &tris[i], point);

        //save closest_tri if closer than previous
        if (t_current < t)
        {
            t = t_current; //update smallest t
            closest_tri = &tris[i]; //store the tri
            closest_point = point; //store the hit point
        }
    }

    //hit case
    if (closest_tri != nullptr)
    {
        //call shade function p_hit, adds returned colour to io_col
        io_col += p_hit(closest_tri, depth, closest_point, dir);
    }
    //miss case
    else
    {
        //return background colour
        io_col += bkgd;
    }
}

vec3 GetRayDirection(float px, float py, int W, int H, float aspect_ratio, float fov)
{
    //set cameras direction vectors R, U, F
    glm::vec3 right = glm::vec3(1.f, 0.f, 0.f);
    glm::vec3 up = glm::vec3(0.f, -1.f, 0.f);
    glm::vec3 forward = glm::vec3(0.f, 0.f, -1.f);

    //f is a function of the vertical field of view
	float f = tan(fov / 2.f);

    //map pixel px, py to NDC, sample at pixel center
    float nx = (px + 0.5f) / (float)W; //[0, 1]
    float ny = (py + 0.5f) / (float)H; //[0, 1]

	float sx = 2.f * nx - 1.f; //[-1, 1]
	float sy = 2.f * ny - 1.f; //[-1, 1]
    
    //right & up are scaled to the camera forward direction in a linear combination
    glm::vec3 ray_direction =  (aspect_ratio * f * sx) * right + (f * sy) * up + forward;

    //return normalised direction vector
    return glm::normalize(ray_direction);
}

void raytrace()
{
    //for each pixel_y in PIXEL_H
    for (int pixel_y = 0; pixel_y < PIXEL_H; ++pixel_y)
    {
        float percf = (float)pixel_y / (float)PIXEL_H;
        int perci = percf * 100;
        std::clog << "\rScanlines done: " << perci << "%" << ' ' << std::flush;

        //for each pixel_x in PIXEL_W
        for (int pixel_x = 0; pixel_x < PIXEL_W; ++pixel_x)
        {
            //create ray for pixel_x, pixel_y
            float vfov = radians(90.f);
			float aspect_ratio = (float)PIXEL_W / (float)PIXEL_H;
            glm::vec3 ray = GetRayDirection(pixel_x, pixel_y, PIXEL_W, PIXEL_H, aspect_ratio, vfov);

            //trace ray for colour of pixel
            float t = 0.f;
            glm::vec3 pixel_col = glm::vec3(0);
            glm::vec3 origin = eye; //camera pos in world space, the origin of the ray o
			trace(origin, ray, t, pixel_col, 0, Shade);

            //write pixel_col to pixelBuffer
			writeCol(pixel_col, pixel_x, pixel_y);
        }
    }
    std::clog << "\rFinish rendering.           \n";
}
