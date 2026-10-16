#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <string>

namespace CMPUT350 {

class GameContext;

class GameObject {
public:
    /** Destroys the object through its dynamic type. */
    virtual ~GameObject() = default;

    /** Initializes the object when the engine activates it.
     * @param context Engine and drawing services for this callback.
     * @return No value.
     */
    virtual void Initialize(GameContext* context);

    /** Updates the object once per frame.
     * @param context Engine and drawing services for this callback.
     * @return No value.
     */
    virtual void Update(GameContext* context);

    /** Performs per-frame work after collisions have been processed.
     * @param context Engine and drawing services for this callback.
     * @return No value.
     */
    virtual void LateUpdate(GameContext* context);

    /** Renders optional user-interface content; the base implementation does nothing.
     * @param context Engine and drawing services for this callback.
     * @return No value.
     */
    virtual void RenderUI(GameContext* context);

    /** Receives an immediate message from the notification manager; default does nothing. */
    virtual void ReceiveNotification(const std::string& key);

    /** Reports whether the engine should keep this object active.
     * @return True while alive; false causes removal at the start of a later frame.
     */
    virtual bool IsAlive() const;

    /** Marks this object dead so the engine removes it at the start of a later frame.
     * @return No value.
     */
    virtual void Kill();




private:
    bool mIsAlive = true;
};

}  // namespace CMPUT350

#endif  // GAMEOBJECT_H
