#ifndef GRAPHICS_OBJECT_H
#define GRAPHICS_OBJECT_H

#include "GameObject.h"
#include "MathUtil.h"

namespace CMPUT350 {

class GameContext;


/** A game object with background and foreground rendering callbacks. */
class GraphicsObject : public GameObject {
public:
    /** Renders content behind foreground objects; the base implementation does nothing.
     * @param context Engine and drawing services for this callback.
     * @return No value.
     */
    virtual void RenderBackground(GameContext *context);

    /** Renders foreground content; the base implementation does nothing.
     * @param context Engine and drawing services for this callback.
     * @return No value.
     */
    virtual void RenderForeground(GameContext *context);

    virtual Point2D GetLocation() const = 0;
    virtual float GetRotation() const = 0;
};

}  // namespace CMPUT350

#endif
