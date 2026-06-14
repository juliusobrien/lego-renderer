#include <list>
#include <vector>
#include <algorithm>

#include "point.h"

point evaluate(float t, std::list<point> P)
{
	std::list<point> Q = P;

	while (Q.size() > 1) 
	{
		std::list<point> R;

		std::list<point>::iterator p1 = Q.begin();
		std::list<point>::iterator p2 = Q.begin();
		p2++;

		for (; p2 != Q.end(); p1++, p2++) 
		{
			point p = ((1.f - t) * (*p1)) + (t * (*p2));
			R.push_back(p);
		}
		Q.clear();
		Q = R;
	}
	return Q.front();
}

std::vector<point> EvaluateBezierCurve(std::vector<point>ctrl_points, int num_evaluations)
{
	float offset = 1.f / num_evaluations;
	std::list<point> ps(ctrl_points.begin(), ctrl_points.end());
	std::vector<point> curve;

	curve.push_back(ctrl_points.front());

	for (int e = 0; e < num_evaluations; e++) 
	{
		point p = evaluate(offset * (e + 1), ps);
		curve.push_back(p);
	}
	return curve;
}

float* MakeFloatsFromVector(std::vector<point> curve, int& num_verts, int& num_floats, float r, float g, float b)
{
	num_verts = curve.size();
	if (num_verts == 0) 
	{
		return NULL;
	}
	num_floats = num_verts * 6;

	int i = 0;
	float* vertices = (float*)malloc(num_floats * sizeof(float));

	for (auto it = curve.begin(); it != curve.end(); it++)
	{
		const point& p = *it;

		vertices[i++] = p.x;
		vertices[i++] = p.y;
		vertices[i++] = p.z;

		vertices[i++] = r;
		vertices[i++] = g;
		vertices[i++] = b;
	}

	return vertices;
}
