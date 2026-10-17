#ifndef COLLISION_OBJECT_H
#define COLLISION_OBJECT_H

#include <memory>
#include <vector>

#include "GraphicsObject.h"
#include "MathUtil.h"

namespace CMPUT350 {

enum class ShapeType { kLine, kCircle, kRect };

union ShapeUnion {
    /** Stores an active line member; Shape::t identifies it. */
    ShapeUnion(Line l) : line(l) {}
    /** Stores an active circle member. */
    ShapeUnion(Circle c) : circle(c) {}
    /** Stores an active local axis-aligned rectangle member. */
    ShapeUnion(Rect r) : rect(r) {}

    Line line;
    Circle circle;
    Rect rect;
};

struct Shape {
    /** Creates a tagged local line component. */
    Shape(const Line& l) : shape(l), t(ShapeType::kLine) {}

    /** Creates a tagged local circle component. */
    Shape(const Circle& c) : shape(c), t(ShapeType::kCircle) {}

    /** Creates a tagged local rectangle component. */
    Shape(const Rect& r) : shape(r), t(ShapeType::kRect) {}

    ShapeUnion shape;
    ShapeType t;
};

class CollisionObject : public GraphicsObject {
public:
    /** Receives one collision with another object. */
    virtual void CollisionEnter(const std::shared_ptr<CollisionObject>& obj) = 0;

    /** Receives a directed-line contact in this object's local space; defaults to the plain
     * callback. */
    virtual void CollisionEnter(const std::shared_ptr<CollisionObject>& obj,
                                const Point2D& collisionPoint) {
        CollisionEnter(obj);
    }

    /** Returns local broad-phase bounds enclosing every collision component. */
    virtual const Rect& GetBounds() = 0;

    /** Returns local shapes for detailed collision tests. */
    virtual const std::vector<Shape>& GetShapes() = 0;

    /** Reports whether the collision object remains stationary. */
    virtual bool IsStatic() const = 0;
};

}  // namespace CMPUT350

#endif
