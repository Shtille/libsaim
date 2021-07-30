#include "model.h"

#include "saim.h"

Model::Model()
: instance_(nullptr)
, buffer_(nullptr)
, width_(640)
, height_(480)
, bytes_per_pixel_(3)
{
}
Model::~Model()
{
	if (buffer_)
		delete[] buffer_;
}
bool Model::Initialize()
{
	int result;
	instance_ = saim_init("", nullptr, 0, 1, &result);
	return result == 0;
}
void Model::Deinitialize()
{
	saim_cleanup(instance_);
	instance_ = nullptr;
}
void Model::SetTarget(int width, int height)
{
	width_ = width;
	height_ = height;
	if (buffer_)
		delete[] buffer_;
	buffer_ = new unsigned char[width_ * height_ * bytes_per_pixel_];
	saim_set_target(instance_, buffer_, width_, height_, bytes_per_pixel_);
	saim_set_bitmap_cache_size(instance_, 50);
}
void Model::ConvertRgbToBgr()
{
	unsigned char *r,*b,temp;
	unsigned char * data = buffer_;
	for (int i = 0; i < width_ * height_; ++i)
	{
		r = data;
		b = data + 2;
		data += 3;
		temp = *r;
		*r = *b;
		*b = temp;
	}
}
int Model::Render(double upper_latitude, double left_longitude, double lower_latitude, double right_longitude)
{
	return saim_render_aligned(instance_, upper_latitude, left_longitude, lower_latitude, right_longitude);
}
const int Model::width() const
{
	return width_;
}
const int Model::height() const
{
	return height_;
}
const unsigned char* Model::data() const
{
	return buffer_;
}