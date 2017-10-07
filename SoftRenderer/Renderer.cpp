#include "Renderer.hpp"


Renderer::Renderer(): width_(0), height_(0)
{
}


Renderer::~Renderer()
{
}

void Renderer::Clear()
{
	for (auto&& value : zbuffer)
	{
		value = 1.0f;
	}
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
	class EmptyPASS
	{
	public:
		static void shading(vertex_output& x)
		{
		}
	};

	for (int i = 0; i < triangles_.size(); ++i)
	{
		auto empry = EmptyPASS();
		RasterizeTriangle<EmptyPASS>(i, empry);
	}
}



