#pragma once

#include "Vector.h"
#include "AABBox.h"

struct UV
{
	float u;
	float v;
};

struct Vertex
{
	Vertex(const Vector& v) : Vert(v) {}
	Vector Vert;
	Vector Normal;

};

struct Material;

class Triangle {

public:
	Triangle() = delete;
	Triangle(Vertex& v0, Vertex& v1, Vertex& v2, const Material& material);
	
	const Vector& normal() const;
	const Vector hitNormal(const Point& p) const;
	void normalizeVertices();
	ColorRGB color() const;
	void setColor(const Color& color);
	float distance(const Point& origin) const;
	const Material& metrial() const { return _material;	}
	
	float area() const;
	UV uv(const Point& p) const;
	// Recalculate normal and edge vectors
	void recalc();
	bool checkIntersaction(const Point& point) const;
	bool checkIntersaction(const AABBox& AABBox) const;

	Vertex& V0;
	Vertex& V1;
	Vertex& V2;

private:
	const Vector smoothNormal(const Point& p) const;

private:
	Vector _normal;
	float _rectArea;
	Vector _V0V1;
	Vector _V1V2;
	Vector _V2V0;
	Vector _V0V2;
	Color _color{ 1.f, 1.f, 1.f };
	const Material& _material;
	AABBox _trignaleAABBox;
};
