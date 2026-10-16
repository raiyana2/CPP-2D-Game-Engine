#include "Bezier.h"

#include <cmath>

namespace CMPUT350
{

Bezier::Bezier(const std::vector<Point2D>& pts)
    : mPoints(pts)
{
}

Point2D Bezier::GetPoint(float t) const
{
    return GetPoint(mPoints, t);
}

Point2D Bezier::GetSlope(float t) const
{
    return GetSlope(mPoints, t);
}

Point2D Bezier::GetPoint(
    const std::vector<Point2D>& pts,
    float t)
{
    if (pts.size() < 4)
    {
        return Point2D(0, 0);
    }

    int segmentCount =
        static_cast<int>((pts.size() - 1) / 3);

    if (t < 0.0f)
    {
        t = 0.0f;
    }

    if (t > segmentCount)
    {
        t = static_cast<float>(segmentCount);
    }

    int segment =
        static_cast<int>(std::floor(t));

    float localT = t - segment;

    if (segment == segmentCount)
    {
        segment--;
        localT = 1.0f;
    }

    int start = segment * 3;

    Point2D p0 = pts[start];
    Point2D p1 = pts[start + 1];
    Point2D p2 = pts[start + 2];
    Point2D p3 = pts[start + 3];

    float oneMinusT = 1.0f - localT;

    Point2D point =
        p0 * (oneMinusT * oneMinusT * oneMinusT)
        +
        p1 * (3.0f * oneMinusT * oneMinusT * localT)
        +
        p2 * (3.0f * oneMinusT * localT * localT)
        +
        p3 * (localT * localT * localT);

    return point;
}

Point2D Bezier::GetSlope(
    const std::vector<Point2D>& pts,
    float t)
{
    if (pts.size() < 4)
    {
        return Point2D(0, 0);
    }

    int segmentCount =
        static_cast<int>((pts.size() - 1) / 3);

    if (t < 0.0f)
    {
        t = 0.0f;
    }

    if (t > segmentCount)
    {
        t = static_cast<float>(segmentCount);
    }

    int segment =
        static_cast<int>(std::floor(t));

    float localT = t - segment;

    if (segment == segmentCount)
    {
        segment--;
        localT = 1.0f;
    }

    int start = segment * 3;

    Point2D p0 = pts[start];
    Point2D p1 = pts[start + 1];
    Point2D p2 = pts[start + 2];
    Point2D p3 = pts[start + 3];

    float oneMinusT = 1.0f - localT;

    Point2D slope =
        (p1 - p0) *
            (3.0f * oneMinusT * oneMinusT)
        +
        (p2 - p1) *
            (6.0f * oneMinusT * localT)
        +
        (p3 - p2) *
            (3.0f * localT * localT);

    return slope;
}

}