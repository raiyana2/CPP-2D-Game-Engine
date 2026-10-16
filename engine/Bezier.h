#ifndef BEZIER_H
#define BEZIER_H

#include <vector>

#include "MathUtil.h"

namespace CMPUT350
{

class Bezier
{
public:


    /** Stores control points and samples an arc-length table (128 intervals per segment). */
    Bezier(const std::vector<Point2D>& pts);


    /** Evaluates position for segment time t, clamped to the available segments. */
    Point2D GetPoint(float t) const;

    /** Evaluates the derivative with respect to segment time t. */
    Point2D GetSlope(float t) const;

    /** Evaluates cubic position without constructing a curve; fewer than four points return zero. */
    static Point2D GetPoint(
        const std::vector<Point2D>& pts,
        float t
    );

    /** Evaluates the cubic derivative without constructing a curve. */
    static Point2D GetSlope(
        const std::vector<Point2D>& pts,
        float t
    );

    /** Returns approximate total path length in pixels. */
    float GetLength() const;

    /** Converts traveled pixel distance to segment time using the sampled arc-length table. */
    float GetTimeAtDistance(float distance) const;

private:

    std::vector<Point2D> mPoints;
    std::vector<float> mLengths;
    std::vector<float> mTimes;
};

}

#endif