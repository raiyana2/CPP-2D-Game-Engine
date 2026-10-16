#ifndef ENGINEVIEW_H
#define ENGINEVIEW_H

#include <memory>
#include <vector>
#include <string>

namespace CMPUT350 {

class GameObject;

class EngineView {

    using tGameObject = std::vector<std::shared_ptr<GameObject>>;

public:
    virtual ~EngineView() = default;

    /** Queues a game object for activation by the engine.
     * @param gameObject Shared ownership of the object to add.
     * @return No value.
     */
    virtual void AddGameObject(std::shared_ptr<GameObject> gameObject) = 0;

    /** Associates a scancode press with a notification message. */
    virtual void InstallKeyDownNotification(int key, const std::string &notification) = 0;
    /** Associates a scancode release with a notification message. */
    virtual void InstallKeyUpNotification(int key, const std::string &notification) = 0;

    using const_iterator = tGameObject::const_iterator;

    /** Returns the first active object for read-only iteration. */
    virtual const_iterator cbegin() const = 0;
    /** Returns the past-the-end active-object iterator. */
    virtual const_iterator cend() const = 0;
    /** Returns the first active object for range-based loops. */
    virtual const_iterator begin() const = 0;
    /** Returns the past-the-end iterator for range-based loops. */
    virtual const_iterator end() const = 0;

};

}  // namespace CMPUT350

#endif
