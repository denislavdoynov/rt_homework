#pragma once

#include "AABBox.h"
#include "Utils.h"
#include <memory>

struct Node;

class AccelerationTree
{
public: 
	using NodeList = std::vector<std::unique_ptr<Node>>;
	using TriangleSubset = std::vector<const Triangle*>;

	AccelerationTree(const Triangles& triangles);

	void buildTree(const AABBox& rootBox);
	bool checkIntersection(const Ray& ray, TriangleSubset& triangles) const;
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
	Node* LeftChild = nullptr;
	Node* RightChild = nullptr;
	AABBox Box;
	AccelerationTree::TriangleSubset TrianglesInside;
};


