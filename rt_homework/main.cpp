#include "PPMFile.h"
#include "Shape.h"
#include "Scene.h"
#include "Common.h"

#include <iostream>

#define TASK_5

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
	Scene scene;
	scene.draw(task3);
#endif

#ifdef TASK_4
	Vector p2_a(3.5f, 0.f, 0.f);
	std::cout << "cross: " << p2_a.cross({1.75f, 3.5f, 0.f}) << std::endl;

	Vector p2_b(3.f, -3.f, 1.f);
	std::cout << "cross: " << p2_b.cross({4.f, 9.f, 3.f}) << std::endl;

	Vector p2_c(3.5f, 0.f, 0.f);
	std::cout << "cross: " << p2_c.cross({ 1.75f, 3.5f, 0.f }) << std::endl;

	Triangle p3_a({ -1.75, -1.75, -3 }, { 1.75, -1.75, -3 }, { 0, 1.75, -3 } );
	std::cout << "normal: " << p3_a.normal().normalize() << std::endl;
	std::cout << "area: " << p3_a.area() << std::endl;

	Triangle p3_b({ 0, 0, -1 }, { 1, 0, 1 }, { -1, 0, 1 });
	std::cout << "normal: " << p3_b.normal().normalize() << std::endl;
	std::cout << "area: " << p3_b.area() << std::endl;

	Triangle p3_c({ 0.56f, 1.11f, 1.23f }, { 0.44f, -2.368f, -0.54f }, { -1.56f, 0.15f, -1.92f });
	std::cout << "normal: " << p3_c.normal().normalize() << std::endl;
	std::cout << "area: " << p3_c.area() << std::endl;
#endif

#ifdef TASK_5
	{
		// Create triangle and add to the scene
		Triangle t1({ -1.75, -1.75, -3 }, { 3.75, -1.75, -3 }, { 0, 1.75, -3 });
		t1.setColor({ 3, 252, 11 });	
		Triangle t2({ 0.70, -1.75, -2 }, { 4.75, 1.75, -3 }, { 0, 1.75, -3 });
		t2.setColor({ 252, 3, 140 });
		
		Scene scene;
		scene.addGeometry(t1);
		scene.addGeometry(t2);

		PPMFile task5("output\\task5.ppm", IMAGE_WIDTH, IMAGE_HEIGHT, MAX_COLOR_COMPONENT);
		scene.draw(task5);
	}
	{
		Scene scene;
		scene.addGeometry({{ -1.75, -1.75, -3 }, { 1.75, -1.75, -30 }, { 0, 1.75, -20 }});

		PPMFile task5("output\\task5_a.ppm", IMAGE_WIDTH, IMAGE_HEIGHT, MAX_COLOR_COMPONENT);
		scene.draw(task5);
	}

#endif

	return 0;
}