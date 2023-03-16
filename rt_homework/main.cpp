#include "PPMFile.h"
#include "Shape.h"
#include "RTImage.h"
#include "Common.h"

int main() {
	srand((unsigned int)time(NULL));
/*		
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

	PPMFile task2("output\\task2.ppm", IMAGE_WIDTH, IMAGE_HEIGHT, MAX_COLOR_COMPONENT);
	Circle circle(220.f);
	circle.setPostion(IMAGE_WIDTH * 0.5f, IMAGE_HEIGHT * 0.5f);
	circle.draw(task2);
*/	
	PPMFile task3("output\\task3.ppm", IMAGE_WIDTH, IMAGE_HEIGHT, MAX_COLOR_COMPONENT);
	RTImage img;
	img.draw(task3);

	return 0;
}