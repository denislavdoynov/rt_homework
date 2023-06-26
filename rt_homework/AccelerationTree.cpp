#include "AccelerationTree.h"
#include "Triangle.h"
#include <stack>

AccelerationTree::AccelerationTree(const Triangles& triangles) : 
    _sceneTriangles(triangles) 
{
}

void AccelerationTree::clear() 
{
    _nodes.clear();
}

bool AccelerationTree::checkIntersection(const Ray& ray, Triangles& triangles) const
{
    std::vector<const Node*> stack;
    stack.push_back(_nodes.begin()->get());
    while(!stack.empty()) {
        const auto node = stack.back();
        stack.pop_back();

        if(node->Box.checkIntersection(ray)) {
            // Leaf node
            if(node->OwnedTriangles.size() > 0) {
                triangles.insert( triangles.end(), node->OwnedTriangles.begin(), node->OwnedTriangles.end() );
            } else { 
                // Continue checking the tree
                if(node->Child1) {
                    stack.push_back(node->Child1);
                }
                if(node->Child2) {
                    stack.push_back(node->Child2);
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

    Triangles list;
    for (const auto triangle : _sceneTriangles) {
        if(triangle->checkIntersaction(box)) {
            list.push_back(triangle);
        }
    }

    // If this box have no triangles stop rescurision
    if(list.empty()) {
        return currentNode;
    }

    // Check if this is leaf and populate triangles
    if(depth >= MAX_TREE_DEPTH || list.size() <= MAX_TRIANGLE_COUNT) {
        currentNode->OwnedTriangles.swap(list);
        return currentNode;
    }

    // Continue splitting boxes and dig down
    AABBox box1, box2;
    box.split(depth % Vector::SIZE, box1, box2);
     
    currentNode->Child1 = addNode(currentNode, depth+1, box1);
    currentNode->Child2 = addNode(currentNode, depth+1, box2);

    return currentNode;
}