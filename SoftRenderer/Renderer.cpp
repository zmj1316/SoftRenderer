#include "Renderer.h"



Renderer::Renderer()
{
}


Renderer::~Renderer()
{
}

void Renderer::IaStage(const std::vector<int>& ib)
{
	MY_ASSERT((ib.size() % 3) == 0);
	triangles_.resize(ib.size() / 3);
	for (int i = 0; i < triangles_.size(); ++i)
	{
		for (int r = 0; r < 3; ++r)
		{
			triangles_[i].indices[r] = ib[i * 3 + r];
		}
	}
}

void Renderer::EarlyZ()
{

}

void Renderer::RasterizeTriangle(int index)
{
	auto& tri = triangles_[index];
	vertex_output vertices[3];
	vec2 screen_cords[3];
	for (int i = 0; i < 3; ++i)
	{
		vertices[i] = vb_after_vs_[tri.indices[i]];
		screen_cords[i].x = (vertices[i].pos.x + 1) / 2 * width_;
		screen_cords[i].y = (vertices[i].pos.y + 1) / 2 * height_;
	}
	struct Rect
	{
		float bottom, top, left, right;
	};

	Rect rect;
	rect.bottom = 0;
	rect.top = height_;
	rect.left = width_;
	rect.right = 0;

	for (int i = 0; i < 3; ++i)
	{
		if (screen_cords[i].x < rect.left) rect.left = screen_cords[i].x;
		if (screen_cords[i].x > rect.right) rect.right = screen_cords[i].x;
		if (screen_cords[i].y < rect.top) rect.top = screen_cords[i].y;
		if (screen_cords[i].x > rect.bottom) rect.bottom = screen_cords[i].y;
	}

	rect.bottom = min(rect.bottom, height_);
	rect.top = max(rect.top, 0);
	rect.left = min(rect.left, width_);
	rect.right = max(rect.right, 0);

	std::vector<uint8_t> in_line;
	std::vector<uint8_t> out_line;

}
