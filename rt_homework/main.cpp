#include "PPMFile.h"
#include "Shape.h"
#include "RTImage.h"
#include "Common.h"

#include <iostream>

#define TASK_4

int main() {
	srand((unsigned int)time(NULL));

#ifdef TASK_1		
	PPMFile task1("output\\task1.ppm", IMAGE_WIDTH, IMAGE_HEIGHT, MAX_COLOR_COMPONENT);
	Rectangles rect(7, 5);
	rect.draw(task1);

	PPMFile task1_2("output\\task1_2.ppm", IMAGE_WIDTH, IMAGE_HEIGHT, MAX_COLOR_COMPONENT);
	Rectangles rect2(3, 0);
	rect2.setColors({
		std::make_pair(0, Color(255, 255, 255)),
		std::make_pair(1 << 16, Color(0, 233, 0)),
		std::make_pair(2 << 16, Color(233, 0, 0)),
		});
	rect2.draw(task1_2);
#endif

#ifdef TASK_2
	PPMFile task2("output\\task2.ppm", IMAGE_WIDTH, IMAGE_HEIGHT, MAX_COLOR_COMPONENT);
	Circle circle(220.f);
	circle.setPostion(IMAGE_WIDTH * 0.5f, IMAGE_HEIGHT * 0.5f);
	circle.draw(task2);
#endif

#ifdef TASK_3
	PPMFile task3("output\\task3.ppm", IMAGE_WIDTH, IMAGE_HEIGHT, MAX_COLOR_COMPONENT);
	RTImage img;
	img.draw(task3);
#endif

#ifdef TASK_4
	Vector p2_a(3.5f, 0.f, 0.f);
	std::cout << "cross: " << p2_a.cross({1.75f, 3.5f, 0.f}) << std::endl;

	Vector p2_b(3.f, -3.f, 1.f);
	std::cout << "cross: " << p2_b.cross({4.f, 9.f, 3.f}) << std::endl;

	Vector p2_c(3.5f, 0.f, 0.f);
	std::cout << "cross: " << p2_c.cross({ 1.75f, 3.5f, 0.f }) << std::endl;

	Triangle p3_a({ -1.75, -1.75, -3 }, { 1.75, -1.75, -3 }, { 0, 1.75, -3 } );
	std::cout << "cross: " << p3_a.cross().normalize() << std::endl;
	std::cout << "area: " << p3_a.area() << std::endl;

	Triangle p3_b({ 0, 0, -1 }, { 1, 0, 1 }, { -1, 0, 1 });
	std::cout << "cross: " << p3_b.cross().normalize() << std::endl;
	std::cout << "area: " << p3_b.area() << std::endl;

	Triangle p3_c({ 0.56, 1.11, 1.23 }, { 0.44, -2.368, -0.54 }, { -1.56, 0.15, -1.92 });
	std::cout << "cross: " << p3_c.cross().normalize() << std::endl;
	std::cout << "area: " << p3_c.area() << std::endl;
#endif

	return 0;
}