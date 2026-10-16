#ifndef MATHUTIL_H
#define MATHUTIL_H

#include <algorithm>
#include <cmath>
#include <iostream>

namespace CMPUT350 {

struct Point2D {
    float x, y;

    /** Creates a point or vector from two coordinates.
     * @param x Horizontal coordinate; defaults to zero.
     * @param y Vertical coordinate; defaults to zero.
     */
    Point2D(float x = 0, float y = 0) : x(x), y(y) {}

    /** Measures the Euclidean distance to another point.
     * @param other Point to measure to.
     * @return Non-negative distance in coordinate units.
     */
    double Distance(const Point2D& other) const {
        //(DONE) TODO: write this code
        float dx = other.x - x;
        float dy = other.y - y;
        return std::sqrt(dx * dx + dy * dy);
    }

    /** Adds corresponding coordinates.
     * @param other Point to add.
     * @return The sum as a new point.
     */
    Point2D operator+(const Point2D& other) const {
        //(DONE) TODO: write this code
        return Point2D(x + other.x, y + other.y);
    }

    /** Adds a scalar to both coordinates.
     * @param other Scalar added to x and y.
     * @return The result as a new point.
     */
    Point2D operator+(const float& other) const {
        //(DONE) TODO: write this code
        return Point2D(x + other, y + other);  // here other is a single float
    }

    /** Subtracts corresponding coordinates.
     * @param other Point to subtract.
     * @return The difference as a new point.
     */
    Point2D operator-(const Point2D& other) const {
        //(DONE) TODO: write this code
        return Point2D(x - other.x, y - other.y);
    }

    /** Subtracts a scalar from both coordinates.
     * @param other Scalar subtracted from x and y.
     * @return The result as a new point.
     */
    Point2D operator-(const float& other) const {
        //(DONE) TODO: write this code
        return Point2D(x - other, y - other);
    }

    /** Multiplies both coordinates by a scalar.
     * @param scalar Multiplier.
     * @return The scaled point as a new value.
     */
    Point2D operator*(const float& scalar) const {
        //(DONE) TODO: write this code
        return Point2D(x * scalar, y * scalar);
    }

    /** Adds a scalar to both coordinates in place.
     * @param scalar Value added to x and y.
     * @return Reference to this modified point.
     */
    Point2D& operator+=(
        const float& scalar) {  // Point2D& means the function returns the same existing Point2D
                                // object, not a copy. so x = x + scalar, y = y + scalar, and then
                                // return the same object.
        // (DONE) TODO: write this code
        x += scalar;
        y += scalar;
        return *this;
    }

    /** Adds corresponding coordinates in place.
     * @param other Point to add.
     * @return Reference to this modified point.
     */
    Point2D& operator+=(const Point2D& other) {
        // (DONE) TODO: write this code
        x += other.x;
        y += other.y;
        return *this;
    }

    /** Subtracts corresponding coordinates in place.
     * @param other Point to subtract.
     * @return Reference to this modified point.
     */
    Point2D& operator-=(const Point2D& other) {
        // (DONE) TODO: write this code
        x -= other.x;
        y -= other.y;
        return *this;
    }

    /** Tests exact equality of both coordinates.
     * @param other Point to compare.
     * @return True if both coordinates are equal; otherwise false.
     */
    bool operator==(const Point2D& other) const {
        // (DONE) TODO: write this code
        return (x == other.x && y == other.y);
    }

    /** Multiplies both coordinates by an integer in place.
     * @param scalar Multiplier.
     * @return Reference to this modified point.
     */
    Point2D& operator*=(const int& scalar) {
        // (DONE) TODO: write this code
        x *= scalar;
        y *= scalar;
        return *this;
    }

    /** Divides both coordinates by an integer in place.
     * @param scalar Divisor; if zero, this point is reset to (0, 0) and an error is written to
     * stderr.
     * @return Reference to this modified point.
     */
    Point2D& operator/=(const int& scalar) {
        // (DONE) TODO: write this code

        if (scalar == 0) {
            std::cerr << "Division by zero is not allowed.\n";
            x = 0;
            y = 0;
            return *this;
        }
        x /= scalar;
        y /= scalar;
        return *this;
    }

    /** Computes the dot product with another point treated as a vector.
     * @param other Other vector.
     * @return Scalar dot product.
     */
    float operator*(
        const Point2D& other) const {  // dot product of two vectors, returns a scalar value`
        // (DONE) TODO: write this code
        return x * other.x + y * other.y;
    }

    /** Computes the dot product with another vector.
     * @param b Other vector.
     * @return Scalar dot product.
     */
    float Dot(Point2D b) const {
        // (DONE) TODO: write this code
        return x * b.x + y * b.y;
    }

    /** Computes the dot product of two vectors.
     * @param a First vector.
     * @param b Second vector.
     * @return Scalar dot product.
     */
    static float Dot(Point2D a, Point2D b) {
        // (DONE) TODO: write this code
        return a.x * b.x + a.y * b.y;
    }

    /** Computes the 2D determinant of two vectors.
     * @param a First vector.
     * @param b Second vector.
     * @return Signed scalar cross product.
     */
    static float Cross(Point2D a, Point2D b) {
        // (DONE) TODO: write this code
        return a.x * b.y - a.y * b.x;
    }

    /** Converts this vector to unit length when it is nonzero.
     * A zero vector is left unchanged.
     * @return No value.
     */
    void Normalize() {
        // (DONE) TODO: write this code
        float length = std::sqrt(x * x + y * y);
        if (length != 0) {
            x /= length;
            y /= length;
        }
    }
};

/** Writes a point in `(x, y)` form.
 * @param os Output stream to write to.
 * @param p Point to format.
 * @return Reference to the output stream.
 */
static std::ostream& operator<<(std::ostream& os, const Point2D& p) {
    // (DONE) TODO: write this code
    os << "(" << p.x << ", " << p.y << ")";
    return os;
}

/** Multiplies both point coordinates by a scalar.
 * @param number Scalar multiplier.
 * @param rhs Point to scale.
 * @return The scaled point.
 */
static Point2D operator*(float number, const Point2D& rhs) {
    // (DONE) TODO: write this code
    return Point2D(number * rhs.x, number * rhs.y);
}

struct Line {
    Point2D p1, p2;

    /** Creates a segment from two endpoints.
     * @param p1 Start point; defaults to the origin.
     * @param p2 End point; defaults to the origin.
     */
    Line(Point2D p1 = {0, 0}, Point2D p2 = {0, 0}) : p1(p1), p2(p2) {}

    /** Creates a segment from four endpoint coordinates.
     * @param x1 Start x-coordinate.
     * @param y1 Start y-coordinate.
     * @param x2 End x-coordinate.
     * @param y2 End y-coordinate.
     */
    Line(float x1, float y1, float x2, float y2) : p1(x1, y1), p2(x2, y2) {}

    /** Measures the segment length.
     * @return Distance between p1 and p2.
     */
    float Length() const {
        // (DONE) TODO: write this code
        return p1.Distance(p2);
    }

    /** Finds the closest point on this segment to a query point.
     * @param p Query point.
     * @return Closest point, clamped to the segment endpoints.
     */
    Point2D ClosestPoint(const Point2D& p) const {
        // (DONE) TODO: write this code
        Point2D direction = p2 - p1;
        float lengthSquared = direction * direction;  // dot product of direction with itself

        // p1 and p2 are the same point, so return p1
        if (lengthSquared == 0) {
            return p1;
        }
        float t = ((p - p1) * direction) /
                  lengthSquared;  // dot product of (p - p1) and direction, divided by lengthSquared

        // Closest point would be before p1
        if (t < 0) {
            return p1;
        }
        // Closest point would be after p2
        if (t > 1) {
            return p2;
        }

        return p1 + direction * t;  // Closest point is on the line segment
    }

    /** Finds an intersection between this finite segment and another.
     * Collinear overlaps return the first overlapping point along this directed segment.
     * @param other Segment to test against.
     * @param crossingPoint Receives the intersection point when one exists; unchanged otherwise.
     * @return True if the segments intersect; otherwise false.
     */
    bool Crosses(const Line& other, Point2D& crossingPoint) const {
        Point2D firstDirection = p2 - p1;
        Point2D secondDirection = other.p2 - other.p1;

        float denominator = Point2D::Cross(firstDirection, secondDirection);

        if (std::abs(denominator) < 0.000001f) {
            float lengthSquared = firstDirection.Dot(firstDirection);
            if (lengthSquared == 0) {
                if (other.ClosestPoint(p1).Distance(p1) > 0.00001f) return false;
                crossingPoint = p1;
                return true;
            }
            if (std::abs(Point2D::Cross(other.p1 - p1, firstDirection)) > 0.00001f) return false;
            float t1 = (other.p1 - p1).Dot(firstDirection) / lengthSquared;
            float t2 = (other.p2 - p1).Dot(firstDirection) / lengthSquared;
            float entry = std::max(0.0f, std::min(t1, t2));
            float exit = std::min(1.0f, std::max(t1, t2));
            if (entry > exit) return false;
            crossingPoint = p1 + firstDirection * entry;
            return true;
        }

        Point2D difference = other.p1 - p1;

        float firstT = Point2D::Cross(difference, secondDirection) / denominator;

        float secondT = Point2D::Cross(difference, firstDirection) / denominator;

        // Both parameters must be within their finite segments.
        if (firstT < 0.0f || firstT > 1.0f || secondT < 0.0f || secondT > 1.0f) {
            return false;
        }

        crossingPoint = p1 + firstDirection * firstT;

        return true;
    }
};

/** Writes a line segment as `Line(p1, p2)`.
 * @param os Output stream to write to.
 * @param l Segment to format.
 * @return Reference to the output stream.
 */
static std::ostream& operator<<(std::ostream& os, const Line& l) {
    // TODO: write this code
    os << "Line(" << l.p1 << ", " << l.p2 << ")";
    return os;
}

struct Circle {
    Point2D center;
    float radius;

    /** Creates a circle from a center point and radius.
     * @param c Center point; defaults to the origin.
     * @param r Radius; defaults to zero.
     */
    Circle(Point2D c = {0, 0}, float r = 0) : center(c), radius(r) {}

    /** Creates a circle from center coordinates and radius.
     * @param x Center x-coordinate.
     * @param y Center y-coordinate.
     * @param r Radius.
     */
    Circle(float x, float y, float r) : center(x, y), radius(r) {}
};

struct Rect {
    Point2D topLeft;
    float width, height;

    /** Creates a rectangle from its top-left coordinate and dimensions.
     * @param left Top-left x-coordinate.
     * @param top Top-left y-coordinate.
     * @param width Rectangle width.
     * @param height Rectangle height.
     */
    Rect(float left, float top, float width, float height)
        : topLeft(Point2D(left, top)), width(width), height(height) {}

    /** Creates a rectangle from a top-left point and integer dimensions.
     * @param tl Top-left point; defaults to the origin.
     * @param w Width; defaults to zero.
     * @param h Height; defaults to zero.
     */
    Rect(Point2D tl = {0, 0}, float w = 0, float h = 0) : topLeft(tl), width(w), height(h) {}

    /** Creates the smallest axis-aligned rectangle containing two points.
     * @param p1 First point.
     * @param p2 Second point.
     */
    Rect(Point2D p1, Point2D p2)
        : topLeft(std::min(p1.x, p2.x), std::min(p1.y, p2.y)),
          width(fabs(p1.x - p2.x)),
          height(fabs(p1.y - p2.y)) {}

    /** Creates an axis-aligned square around a center point.
     * @param center Rectangle center.
     * @param radius Half of the square's width and height.
     */
    Rect(Point2D center, float radius)
        : topLeft(center.x - radius, center.y - radius), width(2 * radius), height(2 * radius) {}

    /** Expands this rectangle to include another rectangle.
     * @param other Rectangle to include.
     * @return Reference to this modified rectangle.
     */

    // Took help from ChatGPT to implement the following
    Rect& operator|=(const Rect& other) {
        //(Done) TODO: write this code
        float left = std::min(topLeft.x, other.topLeft.x);
        float top = std::min(topLeft.y, other.topLeft.y);

        float right = std::max(topLeft.x + width, other.topLeft.x + other.width);
        float bottom = std::max(topLeft.y + height, other.topLeft.y + other.height);

        topLeft = Point2D(left, top);
        width = right - left;
        height = bottom - top;

        return *this;
    }

    /** Expands this rectangle to include a point.
     * @param other Point to include.
     * @return Reference to this modified rectangle.
     */
    Rect& operator|=(const Point2D& other) {
        //(Done) TODO: write this code
        float left = std::min(topLeft.x, other.x);
        float top = std::min(topLeft.y, other.y);
        float right = std::max(topLeft.x + width, other.x);
        float bottom = std::max(topLeft.y + height, other.y);

        topLeft = Point2D(left, top);
        width = right - left;
        height = bottom - top;

        return *this;
    }

    /** Expands this rectangle to include both endpoints of a line segment.
     * @param other Segment to include.
     * @return Reference to this modified rectangle.
     */
    Rect& operator|=(const Line& other) {
        //(Done) TODO: write this code
        *this |= other.p1;
        *this |= other.p2;

        return *this;
    }

    /** Replaces this rectangle with its intersection with another rectangle.
     * If they are disjoint, stores an empty rectangle using width and height -1.
     * @param other Rectangle to intersect with.
     * @return Reference to this modified rectangle.
     */

    // Took help from ChatGPT to implement the following
    Rect& operator&=(const Rect& other) {
        //(Done) TODO: write this code
        float left = std::max(topLeft.x, other.topLeft.x);
        float top = std::max(topLeft.y, other.topLeft.y);

        float right = std::min(topLeft.x + width, other.topLeft.x + other.width);
        float bottom = std::min(topLeft.y + height, other.topLeft.y + other.height);

        if (right < left || bottom < top) {
            // No intersection

            // Prof said "To help you think this through, we need to represent a null rectangle. If
            // our rectangles were open, then having a 0 height/width would be fine. But, we aren't
            // using open rectangles.
            //  So, we need another option. One option is to allow negative heights/widths to
            //  represent null rectangles. This is one option. A second option is to use NAN or
            //  ±INF. These have slightly different semantics

            topLeft = Point2D(0, 0);
            width = -1;
            height = -1;
        } else {
            topLeft = Point2D(left, top);
            width = right - left;
            height = bottom - top;
        }
        return *this;
    }

    /** Translates this rectangle by a vector.
     * @param other Offset added to the top-left point.
     * @return Reference to this modified rectangle.
     */
    Rect& operator+=(const Point2D& other) {
        //(Done) TODO: write this code
        topLeft += other;
        return *this;
    }

    /** Returns this rectangle translated by a vector.
     * @param other Offset added to the top-left point.
     * @return Translated copy; this rectangle is unchanged.
     */
    Rect operator+(const Point2D& other) const {
        //(Done) TODO: write this code
        Rect result = *this;
        result += other;
        return result;
    }

    /** Insets or expands this rectangle symmetrically.
     * Positive values shrink it; negative values expand it.
     * @param inset Amount applied to each edge.
     * @return No value.
     */
    void Inset(int inset) {
        //(Done) TODO: write this code
        topLeft.x += inset;
        topLeft.y += inset;

        width -= 2 * inset;
        height -= 2 * inset;
    }

    /** Tests whether a point is inside this rectangle.
     * The left and top edges are included; the right and bottom edges are excluded.
     * @param p Point to test.
     * @return True if p is inside; otherwise false.
     */
    bool IsInside(const Point2D& p) const {
        //(Done) TODO: write this code
        return p.x >= topLeft.x && p.x < topLeft.x + width && p.y >= topLeft.y &&
               p.y < topLeft.y + height;
    }

    /** Tests whether this rectangle overlaps another.
     * Rectangles that only touch at an edge are considered overlapping.
     * @param other Rectangle to test against.
     * @return True if the rectangles overlap or touch; otherwise false.
     */
    bool overlaps(const Rect& other) const {
        bool notOverlap =
            topLeft.x > other.topLeft.x + other.width || topLeft.x + width < other.topLeft.x ||
            topLeft.y > other.topLeft.y + other.height || topLeft.y + height < other.topLeft.y;
        return !notOverlap;
    }
};

/** Writes a rectangle as `Rect(topLeft, width, height)`.
 * @param os Output stream to write to.
 * @param l Rectangle to format.
 * @return Reference to the output stream.
 */
static std::ostream& operator<<(std::ostream& os, const Rect& l) {
    // (Done)TODO: write this code
    os << "Rect(" << l.topLeft << ", " << l.width << ", " << l.height << ")";
    return os;
}

/**
 * Tests whether two circles overlap.
 *
 * @param first First circle.
 * @param second Second circle.
 * @return True if the circles overlap or touch.
 */
inline bool CircleCircleCollision(const Circle& first, const Circle& second) {
    float differenceX = first.center.x - second.center.x;

    float differenceY = first.center.y - second.center.y;

    float distanceSquared = differenceX * differenceX + differenceY * differenceY;

    float combinedRadius = first.radius + second.radius;

    return distanceSquared <= combinedRadius * combinedRadius;
}

/**
 * Tests whether two axis-aligned rectangles overlap.
 *
 * @param first First rectangle.
 * @param second Second rectangle.
 * @return True if the rectangles overlap or touch.
 */
inline bool RectRectCollision(const Rect& first, const Rect& second) {
    return first.overlaps(second);
}

/**
 * Tests whether a circle overlaps an axis-aligned rectangle.
 *
 * @param circle Circle to test.
 * @param rect Rectangle to test.
 * @return True if the shapes overlap or touch.
 */
inline bool CircleRectCollision(const Circle& circle, const Rect& rect) {
    float left = rect.topLeft.x;
    float right = rect.topLeft.x + rect.width;
    float top = rect.topLeft.y;
    float bottom = rect.topLeft.y + rect.height;

    float closestX = std::max(left, std::min(circle.center.x, right));

    float closestY = std::max(top, std::min(circle.center.y, bottom));

    float differenceX = circle.center.x - closestX;

    float differenceY = circle.center.y - closestY;

    float distanceSquared = differenceX * differenceX + differenceY * differenceY;

    return distanceSquared <= circle.radius * circle.radius;
}

/**
 * Tests whether two line segments cross.
 *
 * @param first First line.
 * @param second Second line.
 * @param collisionPoint Receives the crossing point.
 * @return True if the lines cross.
 */
inline bool LineLineCollision(const Line& first, const Line& second, Point2D& collisionPoint) {
    return first.Crosses(second, collisionPoint);
}

/**
 * Finds the first point where a directed line segment intersects a circle.
 *
 * @param line Directed line segment from p1 to p2.
 * @param circle Circle to test.
 * @param collisionPoint Receives the first collision point.
 * @return True if the line intersects the circle.
 */
inline bool LineCircleCollision(const Line& line, const Circle& circle, Point2D& collisionPoint) {
    Point2D direction = line.p2 - line.p1;
    Point2D offset = line.p1 - circle.center;
    float c = offset.Dot(offset) - circle.radius * circle.radius;
    if (c <= 0) { collisionPoint = line.p1; return true; }
    float a = direction.Dot(direction);
    if (a == 0) return false;
    float b = 2 * offset.Dot(direction);
    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0) return false;
    float t = (-b - std::sqrt(discriminant)) / (2 * a);
    if (t < 0 || t > 1) return false;
    collisionPoint = line.p1 + direction * t;
    return true;
}

/**
 * Finds the first point where a directed line intersects a rectangle.
 *
 * @param line Directed line segment.
 * @param rect Rectangle to test.
 * @param collisionPoint Receives the first collision point.
 * @return True if the line intersects the rectangle.
 */
inline bool LineRectCollision(const Line& line, const Rect& rect, Point2D& collisionPoint) {
    float left = rect.topLeft.x;
    float right = rect.topLeft.x + rect.width;
    float top = rect.topLeft.y;
    float bottom = rect.topLeft.y + rect.height;

    // The line begins inside the rectangle.
    if (line.p1.x >= left && line.p1.x <= right && line.p1.y >= top && line.p1.y <= bottom) {
        collisionPoint = line.p1;
        return true;
    }

    Line edges[] = {Line(left, top, right, top), Line(right, top, right, bottom),
                    Line(right, bottom, left, bottom), Line(left, bottom, left, top)};

    bool foundCollision = false;
    float shortestDistanceSquared = 0.0f;

    for (const Line& edge : edges) {
        Point2D candidate;

        if (line.Crosses(edge, candidate)) {
            float differenceX = candidate.x - line.p1.x;

            float differenceY = candidate.y - line.p1.y;

            float distanceSquared = differenceX * differenceX + differenceY * differenceY;

            if (!foundCollision || distanceSquared < shortestDistanceSquared) {
                foundCollision = true;
                shortestDistanceSquared = distanceSquared;
                collisionPoint = candidate;
            }
        }
    }

    return foundCollision;
}

}  // namespace CMPUT350

#endif  // MATHUTIL_H
