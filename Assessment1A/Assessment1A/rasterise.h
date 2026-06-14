#pragma once

void ClearColourBuffer(float col[4])
{
	glm::vec3 colour(col[0], col[1], col[2]);

	//for each pixel y
	for (int py = 0; py < PIXEL_H; py++)
	{
		//for each pixel x
		for (int px = 0; px < PIXEL_W; px++)
		{
			writeColToDisplayBuffer(colour, px, py);
		}
	}
}

void ClearDepthBuffer()
{
	//for each pixel y
	for (int py = 0; py < PIXEL_H; py++)
	{
		//for each pixel x
		for (int px = 0; px < PIXEL_W; px++)
		{
			int index = py * PIXEL_W + px;
			depth_buffer[index] = INFINITY;
		}
	}
}

void ApplyTransformationMatrix(glm::mat4 T, vector<triangle>& tris)
{
	//for each triangle
	for (int i = 0; i < tris.size(); i++)
	{
		tris[i].v1.pos = T * tris[i].v1.pos;
		tris[i].v2.pos = T * tris[i].v2.pos;
		tris[i].v3.pos = T * tris[i].v3.pos;
	}
}

void ApplyPerspectiveDivision(vector<triangle>& tris)
{
	//for each triangle
	for (int i = 0; i < tris.size(); i++)
	{
		//vertex 1
		float w1 = tris[i].v1.pos.w;
		tris[i].v1.pos.x /= w1;
		tris[i].v1.pos.y /= w1;
		tris[i].v1.pos.z /= w1;
		tris[i].v1.pos.w /= w1;

		//vertex 2
		float w2 = tris[i].v2.pos.w;
		tris[i].v2.pos.x /= w2;
		tris[i].v2.pos.y /= w2;
		tris[i].v2.pos.z /= w2;
		tris[i].v2.pos.w /= w2;

		//vertex 3
		float w3 = tris[i].v3.pos.w;
		tris[i].v3.pos.x /= w3;
		tris[i].v3.pos.y /= w3;
		tris[i].v3.pos.z /= w3;
		tris[i].v3.pos.w /= w3;
	}
}

void ApplyViewportTransformation(int w, int h, vector<triangle>& tris)
{
	float width = (float)w / 2.f;
	float hight = (float)w / 2.f;

	//for each triangle
	for (int i = 0; i < tris.size(); i++)
	{
		//vertex 1
		tris[i].v1.pos.x = (tris[i].v1.pos.x + 1.f) * width;
		tris[i].v1.pos.y = (tris[i].v1.pos.y + 1.f) * hight;
		//vertex 2
		tris[i].v2.pos.x = (tris[i].v2.pos.x + 1.f) * width;
		tris[i].v2.pos.y = (tris[i].v2.pos.y + 1.f) * hight;
		//vertex 3
		tris[i].v3.pos.x = (tris[i].v3.pos.x + 1.f) * width;
		tris[i].v3.pos.y = (tris[i].v3.pos.y + 1.f) * hight;
	}
}

float ImplicitLineEquation(float ax, float ay, float bx, float by, float px, float py)
{
	return	(by - ay) * px + 
			(ax - bx) * py + 
			(bx * ay) - 
			(ax * by);
}

void ComputeBarycentricCoordinates(int px, int py, triangle t, float& alpha, float& beta, float& gamma)
{
	//Vertex coordinates
	float Ax = t.v1.pos.x;
	float Ay = t.v1.pos.y;

	float Bx = t.v2.pos.x;
	float By = t.v2.pos.y;

	float Cx = t.v3.pos.x;
	float Cy = t.v3.pos.y;

	//Pixel coordinates
	float Px = (float)px;
	float Py = (float)py;

	//Numerators
	float alpha_numerator = ImplicitLineEquation(Bx, By, Cx, Cy, Px, Py);
	float beta_numerator = ImplicitLineEquation(Ax, Ay, Cx, Cy, Px, Py);
	float gamma_numerator = ImplicitLineEquation(Ax, Ay, Bx, By, Px, Py);

	//Denominators
	float alpha_denominator = ImplicitLineEquation(Bx, By, Cx, Cy, Ax, Ay);
	float beta_denominator = ImplicitLineEquation(Ax, Ay, Cx, Cy, Bx, By);
	float gamma_denominator = ImplicitLineEquation(Ax, Ay, Bx, By, Cx, Cy);

	//Barycentric coordinates
	alpha = alpha_numerator / alpha_denominator;
	beta = beta_numerator / beta_denominator;
	gamma = gamma_numerator / gamma_denominator;
}

void ShadeFragment(triangle tri, float& alpha, float& beta, float& gamma, glm::vec3& col, float& depth)
{
	//Interpolate colour
	col.r = alpha * tri.v1.col.r + beta * tri.v2.col.r + gamma * tri.v3.col.r;
	col.g = alpha * tri.v1.col.g + beta * tri.v2.col.g + gamma * tri.v3.col.g;
	col.b = alpha * tri.v1.col.b + beta * tri.v2.col.b + gamma * tri.v3.col.b;

	//Interpolate depth
	depth = alpha * tri.v1.pos.z + beta * tri.v2.pos.z + gamma * tri.v3.pos.z;
}

void Rasterise(vector<triangle> tris)
{
	//for each pixel y
	for (int py = 0; py < PIXEL_H; py++)
	{
		float percf = (float)py / (float)PIXEL_H;
		int perci = percf * 100;
		std::clog << "\rScanlines done: " << perci << "%" << ' ' << std::flush;

		//for each pixel x
		for (int px = 0; px < PIXEL_W; px++)
		{
			//for each triangle t
			for (int t = 0; t < tris.size(); t++)
			{
				//Check pixel xy is inside triangle t
				float alpha, beta, gamma;
				ComputeBarycentricCoordinates(px, py, tris[t], alpha, beta, gamma);
				if (alpha >= 0.0f && beta >= 0.0f && gamma >= 0.0f)
				{
					glm::vec3 col;
					float depth;
					ShadeFragment(tris[t], alpha, beta, gamma, col, depth);

					//Check depth test
					int index = py * PIXEL_W + px;
					if (depth < depth_buffer[index])
					{
						depth_buffer[index] = depth;
						writeColToDisplayBuffer(col, px, py);
					}
				}
			}
		}
	}
	std::clog << "\rFinish rendering.\n";
}

void render(vector<triangle>& tris)
{
	//0. Clear Buffers
	float bgd[4] = { 1.f, 1.f, 1.f, 1.f };
	ClearColourBuffer(bgd);
	ClearDepthBuffer();

	//1. Vertex Transformation - MVP to CS
	glm::mat4 model = glm::mat4(1.f);
	model = glm::translate(model, glm::vec3(0.1f, -2.5f, -6.f));
	glm::mat4 view = glm::mat4(1.f);
	glm::mat4 projection = glm::perspective(glm::radians(55.f), (float)PIXEL_W / (float)PIXEL_H, 0.1f, 10.f);
	//flip y axis
	projection[1][1] *= -1.f;
	glm::mat4 MVP = projection * view * model;
	ApplyTransformationMatrix(MVP, tris);

	//2. Perspective Division - CS to NDC
	ApplyPerspectiveDivision(tris);

	//3. Viewport Transformation - NDC to SS
	ApplyViewportTransformation(PIXEL_W, PIXEL_H, tris);

	//4. Rasterise
	Rasterise(tris);
}
