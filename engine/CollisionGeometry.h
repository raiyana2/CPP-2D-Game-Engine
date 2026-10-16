#ifndef COLLISION_GEOMETRY_H
#define COLLISION_GEOMETRY_H

#include <array>
#include "CollisionObject.h"

namespace CMPUT350 {

/** Rigid object transform; angles are radians and scale is always one. */
struct ObjectPose {
    Point2D location;
    float rotation;

    /** Converts an object-local point into world space. */
    Point2D World(Point2D p) const {
        float c = std::cos(rotation), s = std::sin(rotation);
        return location + Point2D(c * p.x - s * p.y, s * p.x + c * p.y);
    }

    /** Converts a world point into object-local space. */
    Point2D Local(Point2D p) const {
        p -= location;
        float c = std::cos(rotation), s = std::sin(rotation);
        return Point2D(c * p.x + s * p.y, -s * p.x + c * p.y);
    }
};

/** Returns the four world corners of a local rectangle in perimeter order. */
inline std::array<Point2D, 4> RectangleCorners(const Rect& r, const ObjectPose& pose) {
    return {pose.World(r.topLeft), pose.World(r.topLeft + Point2D(r.width, 0)),
            pose.World(r.topLeft + Point2D(r.width, r.height)),
            pose.World(r.topLeft + Point2D(0, r.height))};
}

/** Tests oriented rectangles using their four separating axes, including touching edges. */
inline bool RotatedRectCollision(const Rect& a, const ObjectPose& ap,
                                 const Rect& b, const ObjectPose& bp) {
    auto ac = RectangleCorners(a, ap), bc = RectangleCorners(b, bp);
    for (float angle : {ap.rotation, bp.rotation}) {
        for (Point2D axis : {Point2D(std::cos(angle), std::sin(angle)),
                             Point2D(-std::sin(angle), std::cos(angle))}) {
            float amin = ac[0].Dot(axis), amax = amin;
            float bmin = bc[0].Dot(axis), bmax = bmin;
            for (int i = 1; i < 4; ++i) {
                amin = std::min(amin, ac[i].Dot(axis));
                amax = std::max(amax, ac[i].Dot(axis));
                bmin = std::min(bmin, bc[i].Dot(axis));
                bmax = std::max(bmax, bc[i].Dot(axis));
            }
            if (amax < bmin - 0.00001f || bmax < amin - 0.00001f) return false;
        }
    }
    return true;
}

/** Tests local component shapes with independent poses; line contacts are returned in world space.
 * Rectangles retain their orientation. A line/rectangle or circle/rectangle test is
 * performed in the rectangle's local coordinates, where the existing AABB math is exact.
 */
inline bool ComponentCollision(const Shape& a, const ObjectPose& ap,
                               const Shape& b, const ObjectPose& bp,
                               Point2D& point, bool& hasPoint) {
    hasPoint = false;
    if (a.t == ShapeType::kRect && b.t == ShapeType::kRect)
        return RotatedRectCollision(a.shape.rect, ap, b.shape.rect, bp);
    if (a.t == ShapeType::kRect)
        return ComponentCollision(b, bp, a, ap, point, hasPoint);
    if (b.t == ShapeType::kRect) {
        if (a.t == ShapeType::kCircle)
            return CircleRectCollision(Circle(bp.Local(ap.World(a.shape.circle.center)),
                                               a.shape.circle.radius), b.shape.rect);
        Line line(bp.Local(ap.World(a.shape.line.p1)), bp.Local(ap.World(a.shape.line.p2)));
        hasPoint = LineRectCollision(line, b.shape.rect, point);
        if (hasPoint) point = bp.World(point);
        return hasPoint;
    }
    if (a.t == ShapeType::kCircle && b.t == ShapeType::kCircle)
        return CircleCircleCollision(Circle(ap.World(a.shape.circle.center), a.shape.circle.radius),
                                     Circle(bp.World(b.shape.circle.center), b.shape.circle.radius));
    if (a.t == ShapeType::kCircle)
        return ComponentCollision(b, bp, a, ap, point, hasPoint);
    Line line(ap.World(a.shape.line.p1), ap.World(a.shape.line.p2));
    if (b.t == ShapeType::kCircle)
        hasPoint = LineCircleCollision(line, Circle(bp.World(b.shape.circle.center),
                                                    b.shape.circle.radius), point);
    else
        hasPoint = LineLineCollision(line, Line(bp.World(b.shape.line.p1), bp.World(b.shape.line.p2)), point);
    return hasPoint;
}

} // namespace CMPUT350
#endif
