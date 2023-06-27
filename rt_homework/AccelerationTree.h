#pragma once

#include "AABBox.h"
#include "Utils.h"
#include <memory>

constexpr uint16_t MAX_TREE_DEPTH = 20;
constexpr uint16_t MAX_TRIANGLE_COUNT = 20;

struct Node;

class AccelerationTree
{
public: 
	using NodeList = std::vector<std::unique_ptr<Node>>;

	AccelerationTree(const Triangles& triangles);

	void buildTree(const AABBox& rootBox);
	bool checkIntersection(const Ray& ray, Triangles& triangles) const;
	void clear();

private:
	Node* addNode(Node* parent, int depth, const AABBox& box);

private:
	NodeList _nodes;
	const Triangles& _sceneTriangles;

	std::vector<const Node*> stack;
};

struct Node
{
	Node(Node* parent, const AABBox& box) : Parent(parent), Box(box) {};

	Node* Parent = nullptr;
	Node* Child1 = nullptr;
	Node* Child2 = nullptr;
	AABBox Box;
	Triangles TrianglesInside;
};