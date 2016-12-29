#pragma once
#ifndef __MODEL_H__
#define __MODEL_H__

class Model {
	friend class Controller;
public:
	Model();
	~Model();

	bool Initialize();
	void Deinitialize();

	void SetTarget(int width, int height);

	void ConvertRgbToBgr();

	const int width() const;
	const int height() const;
	const unsigned char* data() const;

protected:
	int Render(double upper_latitude, double left_longitude, double lower_latitude, double right_longitude);

private:
	unsigned char * buffer_;
	int width_;
	int height_;
	int bytes_per_pixel_;
};

#endif