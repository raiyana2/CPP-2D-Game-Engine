#include "NotificationManager.h"

#include "GameObject.h"
#include <algorithm>

namespace CMPUT350
{

void NotificationManager::Register(
    std::weak_ptr<GameObject> object, const std::string& key)
{
    auto live = object.lock();
    if (!live || !live->IsAlive()) return;
    auto& listeners = mListeners[key];
    for (const auto& listener : listeners) {
        if (listener.lock() == live) return;
    }
    listeners.push_back(object);
}

void NotificationManager::Unregister(
    std::weak_ptr<GameObject> object, const std::string& key)
{
    auto it = mListeners.find(key);

    if (it == mListeners.end())
    {
        return;
    }

    auto objectShared = object.lock();

    auto& listeners = it->second;

    for (auto listener = listeners.begin();
         listener != listeners.end();)
    {
        auto listenerShared = listener->lock();

        if (!listenerShared)
        {
            listener = listeners.erase(listener);
        }
        else if (objectShared &&
                 listenerShared == objectShared)
        {
            listener = listeners.erase(listener);
        }
        else
        {
            listener++;
        }
    }
}

void NotificationManager::Notify(
    const std::string& message)
{
    auto it = mListeners.find(message);

    if (it == mListeners.end())
    {
        return;
    }

    auto& listeners = it->second;
    listeners.erase(std::remove_if(listeners.begin(), listeners.end(),
        [](const std::weak_ptr<GameObject>& listener) {
            auto object = listener.lock();
            return !object || !object->IsAlive();
        }), listeners.end());
    const auto snapshot = listeners;
    for (const auto& listener : snapshot) {
        auto object = listener.lock();
        if (!object || !object->IsAlive()) continue;
        // An earlier callback may have unregistered a later listener.
        auto current = mListeners.find(message);
        if (current == mListeners.end()) continue;
        bool registered = std::any_of(current->second.begin(), current->second.end(),
            [&object](const std::weak_ptr<GameObject>& entry) { return entry.lock() == object; });
        if (registered) object->ReceiveNotification(message);
    }
}

} // namespace CMPUT350
