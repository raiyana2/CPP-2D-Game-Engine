#ifndef GRAPHICS_OBJECT_H
#define GRAPHICS_OBJECT_H

#include "GameObject.h"
#include "MathUtil.h"

namespace CMPUT350 {

class GameContext;

class GraphicsObject : public GameObject
{
public:
    /** Draws background content using this object's local transform. */
    virtual void RenderBackground(GameContext* context);
    /** Draws foreground content using this object's local transform. */
    virtual void RenderForeground(GameContext* context);

    /** Returns the world position of the object's rotation origin. */
    virtual Point2D GetLocation() const = 0;
    /** Returns rotation in radians (positive appears clockwise with screen y increasing downward). */
    virtual float GetRotation() const = 0;
};

}

#endif