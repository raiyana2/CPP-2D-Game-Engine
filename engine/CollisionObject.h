#ifndef COLLISION_OBJECT_H
#define COLLISION_OBJECT_H

#include <memory>
#include <vector>

#include "GraphicsObject.h"
#include "MathUtil.h"

namespace CMPUT350 {

enum class ShapeType { kLine, kCircle, kRect };

union ShapeUnion {
    ShapeUnion(Line l) : line(l) {}
    ShapeUnion(Circle c) : circle(c) {}
    ShapeUnion(Rect r) : rect(r) {}

    Line line;
    Circle circle;
    Rect rect;
};

struct Shape {
    Shape(const Line& l) : shape(l), t(ShapeType::kLine) {}
    Shape(const Circle& c) : shape(c), t(ShapeType::kCircle) {}
    Shape(const Rect& r) : shape(r), t(ShapeType::kRect) {}

    ShapeUnion shape;
    ShapeType t;
};

class CollisionObject : public GraphicsObject {
public:
    /** Notifies this object that it overlaps another collision object.
     * The engine calls this on both objects in a colliding pair.
     * @param obj The other object in the pair.
     * @return No value.
     */
    virtual void CollisionEnter(const std::shared_ptr<CollisionObject>& obj) = 0;
    virtual void CollisionEnter(const std::shared_ptr<CollisionObject>& obj,
                                const Point2D& collisionPoint) {
        CollisionEnter(obj);
    }

    /** Gets the current axis-aligned bounds used for collision checks.
     * @return Reference to this object's bounds; the referenced rectangle must remain valid.
     */
    virtual const Rect& GetBounds() = 0;
    virtual const std::vector<Shape>& GetShapes() = 0;
};

}  // namespace CMPUT350

#endif
