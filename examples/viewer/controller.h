#pragma once
#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

class Model;

class Controller {
public:
	Controller(Model * model);
	~Controller();

	int Render();

	void ZoomIn();
	void ZoomOut();

	void Move(int dx, int dy);

protected:
	void CalculateParameters();

private:
	Model * model_; // pointer to model object
	double center_latitude_;
	double center_longitude_;
	double half_size_latitude_;
	double half_size_longitude_;
	double scale_index_;
};

#endif