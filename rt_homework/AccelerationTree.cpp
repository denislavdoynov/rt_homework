#include "AccelerationTree.h"
#include "Triangle.h"
#include <stack>

constexpr uint16_t MAX_TREE_DEPTH = 20;
constexpr uint16_t MAX_TRIANGLE_COUNT = 20;

AccelerationTree::AccelerationTree(const Triangles& triangles) : 
    _sceneTriangles(triangles) 
{
}

void AccelerationTree::clear() 
{
    _nodes.clear();
}

bool AccelerationTree::checkIntersection(const Ray& ray, TriangleSubset& triangles) const
{
    std::stack<const Node*> stack;
    stack.push(_nodes.begin()->get());
    while(!stack.empty()) {
        const auto node = stack.top();
        stack.pop();

        if(node->Box.checkIntersection(ray)) {
            // Leaf node
            if(node->TrianglesInside.size() > 0) {
                triangles.insert( triangles.end(), node->TrianglesInside.begin(), node->TrianglesInside.end() );
            } else { 
                // Continue checking the tree
                if(node->LeftChild) {
                    stack.push(node->LeftChild);
                }
                if(node->RightChild) {
                    stack.push(node->RightChild);
                }
            }
        }
    }

    return !triangles.empty();
}

void AccelerationTree::buildTree(const AABBox& rootBox)
{
    addNode(nullptr, 0, rootBox);
}

Node* AccelerationTree::addNode(Node* parent, int depth, const AABBox& box) 
{
    _nodes.emplace_back( std::make_unique<Node>(parent, box) );
    auto* currentNode = _nodes.back().get();

    TriangleSubset triangles;
    for (const auto& triangle : _sceneTriangles) {
        if(triangle.checkIntersaction(box)) {
            triangles.push_back(&triangle);
        }
    }

    // If this box have no triangles stop rescurision
    if(triangles.empty()) {
        return currentNode;
    }
    
    // Check if this is leaf and populate triangles
    if(depth >= MAX_TREE_DEPTH || triangles.size() <= MAX_TRIANGLE_COUNT) {
        currentNode->TrianglesInside = std::move(triangles);
        return currentNode;
    }

    // Continue splitting boxes and dig down
    AABBox box1, box2;
    box.split(depth % Vector::SIZE, box1, box2);
     
    depth++;
    currentNode->LeftChild = addNode(currentNode, depth, box1);
    currentNode->RightChild = addNode(currentNode, depth, box2);

    return currentNode;
}