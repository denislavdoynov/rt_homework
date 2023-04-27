#include "Shape.h"
#include "Scene.h"
#include "Common.h"
#include "Renderer.h"

#include <iostream>
#include <sstream>

#define TASK_8

int main() {
	srand((unsigned int)time(NULL));

#ifdef TASK_1		
	Rectangles rect(7, 5);
	rect.draw("output\\task1.ppm");

	Rectangles rect2(3, 0);
	rect2.setColors({
		std::make_pair(0, Color(255, 255, 255)),
		std::make_pair(1 << 16, Color(0, 233, 0)),
		std::make_pair(2 << 16, Color(233, 0, 0)),
		});
	rect2.draw("output\\task1_2.ppm");
#endif

#ifdef TASK_2
	Circle circle(220.f);
	circle.setPostion(IMAGE_WIDTH * 0.5f, IMAGE_HEIGHT * 0.5f);
	circle.draw("output\\task2.ppm");
#endif

#ifdef TASK_3
	Scene scene;
	scene.draw("output\\task3.ppm");
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
		scene.draw("output\\task5.ppm");
	}
	{
		Scene scene;
		scene.addGeometry({{ -1.75, -1.75, -3 }, { 1.75, -1.75, -30 }, { 0, 1.75, -20 }});
		scene.draw("output\\task5_a.ppm");
	}

#endif

#ifdef TASK_6
	{
		// Create triangle and add to the scene
		Scene scene(IMAGE_WIDTH, IMAGE_HEIGHT);
		Renderer renderer(scene);
		Triangle t1({ -1.75, -1.75, -3 }, { 3.75, -1.75, -3 }, { 0, 1.75, -3 });
		t1.setColor({ 3, 252, 11 });
		Triangle t2({ 0.70, -1.75, -2 }, { 4.75, 1.75, -3 }, { 0, 1.75, -3 });
		t2.setColor({ 252, 3, 140 });

		scene.addGeometry(t1);
		scene.addGeometry(t2);

		scene.camera().pan(30);
		renderer.renderScene("output\\task6_1.ppm");
		
		scene.camera().pan(-30);
		renderer.renderScene("output\\task6_2.ppm");

		scene.camera().truck({-2, 0, 0});
		renderer.renderScene("output\\task6_2_1.ppm");
			
		float panRotation = 0.f;
		float step = 2.f;
		for(int i = 0; i < 50; i++) {
			panRotation += step;			
			if(abs(panRotation) > 20.f )
				step *= -1;

			std::stringstream output;
			output << "output\\task6_3_" << i << ".ppm";
			scene.camera().pan(panRotation);
			renderer.renderScene(output.str());
		}
	}

#endif

#ifdef TASK_7
	Scene scene;
	Renderer renderer(scene);
	if(scene.loadScene("input\\task7\\scene0.crtscene")) {
		renderer.renderScene("output\\task7_0.ppm");
	}
	if (scene.loadScene("input\\task7\\scene1.crtscene")) {
		renderer.renderScene("output\\task7_1.ppm");
	}

	if (scene.loadScene("input\\task7\\scene2.crtscene")) {
		renderer.renderScene("output\\task7_2.ppm");
	}

	if (scene.loadScene("input\\task7\\scene3.crtscene")) {
		renderer.renderScene("output\\task7_3.ppm");
	}

	if (scene.loadScene("input\\task7\\scene4.crtscene")) {
		renderer.renderScene("output\\task7_4.ppm");
	}
#endif

#ifdef TASK_8
	Scene scene;
	Renderer renderer(scene);
	if (scene.loadScene("input\\task8\\scene0.crtscene")) {
		double passedTime = renderer.renderScene("output\\task8\\task8_0.ppm");
		fprintf(stderr, "\rDone: %.2f (sec)\n", passedTime / 1000);
	}
	/*if (scene.loadScene("input\\task8\\scene1.crtscene")) {
		renderer.renderScene("output\\task8\\task8_1.ppm");
	}
	/*
	if (scene.loadScene("input\\task8\\scene2.crtscene")) {
		renderer.renderScene("output\\task8\\task8_2.ppm");
	}

	/*if (scene.loadScene("input\\task8\\scene3.crtscene")) {
		renderer.renderScene("output\\task8\\task8_3.ppm");
	}*/
#endif

	return 0;
}