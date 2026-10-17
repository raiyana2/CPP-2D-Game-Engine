#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H


#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace CMPUT350
{

class GameObject;

class NotificationManager {
public:
    /** Registers a live weak listener once per key; ownership stays with the engine. */
    void Register(std::weak_ptr<GameObject> object, const std::string& key);
    /** Removes this listener and expired registrations for the given key. */
    void Unregister(std::weak_ptr<GameObject> object, const std::string& key);
    /** Immediately notifies live subscribers; subscription changes are safe during callbacks. */
    void Notify(const std::string& message);
private:
    std::unordered_map<std::string, std::vector<std::weak_ptr<GameObject>>> mListeners;
};


}

#endif