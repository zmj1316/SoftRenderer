#pragma once
#include <cstdio>
#include <minwindef.h>
#include "math3d.hpp"

class sampler
{
public:

	sampler(const char* file_name)
	{
		tex = readBMP(file_name);
	};

	vec3 sample(float u, float v)
	{
		return get_bilinear_filtered_pixel_color(u, v);
	}

private:
	float* tex;
	float map_width_i, map_height_i;
#define u_opposite u_f[0]
#define u_ratio u_f[1]
#define v_opposite u_f[2]
#define v_ratio u_f[3]
	vec3 get_bilinear_filtered_pixel_color(float u, float v) const
	{
		vec3 ret;
		u *= map_width_i - 1;
		v *= map_height_i - 1;
		int x = u;
		int y = v;
		float u_f[4];
		u_ratio = u - x;
		v_ratio = v - y;
		u_opposite = 1 - u_ratio;
		v_opposite = 1 - v_ratio;
		x = max(0, x);
		y = max(0, y);
		x = min(x, map_width_i - 1);
		y = min(y, map_height_i - 1);
		int x_d, y_d;
		x_d = min(x + 1, map_width_i - 1);
		y_d = min(y + 1, map_height_i - 1);

		ret.x = (tex[int(x + y * map_height_i) * 3    ] * u_opposite + tex[int(x_d + y * map_height_i) * 3    ] * u_ratio) * v_opposite + (tex[int(x + (y_d)* map_height_i) * 3    ] * u_opposite + tex[int(x_d + (y_d)* map_height_i) * 3    ] * u_ratio) * v_ratio;
		ret.y = (tex[int(x + y * map_height_i) * 3 + 1] * u_opposite + tex[int(x_d + y * map_height_i) * 3 + 1] * u_ratio) * v_opposite + (tex[int(x + (y_d)* map_height_i) * 3 + 1] * u_opposite + tex[int(x_d + (y_d)* map_height_i) * 3 + 1] * u_ratio) * v_ratio;
		ret.z = (tex[int(x + y * map_height_i) * 3 + 2] * u_opposite + tex[int(x_d + y * map_height_i) * 3 + 2] * u_ratio) * v_opposite + (tex[int(x + (y_d)* map_height_i) * 3 + 2] * u_opposite + tex[int(x_d + (y_d)* map_height_i) * 3 + 2] * u_ratio) * v_ratio;
		return ret;
	}

	float* readBMP(const char* filename)
	{
		int i;
		FILE* f = fopen(filename, "rb");
		if (f == NULL)
			return NULL;
		unsigned char info[54];
		fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

												   // extract image height and width from header
		int width = *(int*)&info[18];
		int height = *(int*)&info[22];
		map_width_i = width;
		map_height_i = height;
		int size = 3 * width * height;
		unsigned char* data = new unsigned char[size]; // allocate 3 bytes per pixel
		float* data_f = new float[size * 3];
		fread(data, sizeof(unsigned char), size, f); // read the rest of the data at once
		fclose(f);

		for (i = 0; i < size; i += 3)
		{
			//unsigned char tmp = data[i];
			//data[i] = data[i + 2];
			//data[i + 2] = tmp;
			data_f[i] = data[i + 2];
			data_f[i + 1] = data[i + 1];
			data_f[i + 2] = data[i];
		}

		return data_f;
	}
};
