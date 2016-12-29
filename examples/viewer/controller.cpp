#include "controller.h"

#include "model.h"

#include <cmath>

namespace {
	const double kMinScaleIndex = 0.0;
	const double kMaxScaleIndex = 16.0;
}

Controller::Controller(Model * model)
: model_(model)
, center_latitude_(34.890285)
, center_longitude_(-118.596199)
, half_size_latitude_()
, half_size_longitude_()
, scale_index_(0.0)
{
	CalculateParameters();
}
Controller::~Controller()
{
}
int Controller::Render()
{
	double upper_latitude = center_latitude_ + half_size_latitude_;
	double left_longitude = center_longitude_ - half_size_longitude_;
	double lower_latitude = center_latitude_ - half_size_latitude_;
	double right_longitude = center_longitude_ + half_size_longitude_;
	return model_->Render(upper_latitude, left_longitude, lower_latitude, right_longitude);
}
void Controller::CalculateParameters()
{
	double width = static_cast<double>(model_->width());
	double height = static_cast<double>(model_->height());
	double size_longitude = 90.0 / pow(2.0, scale_index_);
	half_size_longitude_ = size_longitude * 0.5;
	half_size_latitude_ = half_size_longitude_ * (height / width);
}
void Controller::ZoomIn()
{
	if (scale_index_ < kMaxScaleIndex)
		scale_index_ += 1.0;
	CalculateParameters();
}
void Controller::ZoomOut()
{
	if (scale_index_ > kMinScaleIndex)
		scale_index_ -= 1.0;
	CalculateParameters();
}
void Controller::Move(int dx, int dy)
{
	double delta_x = static_cast<double>(dx);
	double delta_y = static_cast<double>(dy);
	double width = static_cast<double>(model_->width());
	double height = static_cast<double>(model_->height());
	center_longitude_ += delta_x * half_size_longitude_ * 2.0 / width;
	center_latitude_ += delta_y * half_size_latitude_ * 2.0 / height;
}