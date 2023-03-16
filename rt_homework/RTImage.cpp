#include "RTImage.h"
#include "Common.h"

RTImage::RTImage() :
    Shape(IMAGE_WIDTH, IMAGE_HEIGHT) 
{
}

Color RTImage::pixelColor(float x, float y) 
{
    Vector rayDirection(x, y, -1);
    rayDirection = rayDirection.getCenter().getNDC(_imageWidth, _imageHeight).getWorldSpace();
  
    // Consider the aspect ratio
    float aspectRatio = (float)_imageWidth / (float)_imageHeight;
    rayDirection.X *= aspectRatio;

    rayDirection = rayDirection.normalize();
    return rayDirection.toColor(MAX_COLOR_COMPONENT);
}