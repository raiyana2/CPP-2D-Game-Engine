#include "NotificationManager.h"

#include "GameObject.h"

namespace CMPUT350
{

void NotificationManager::Register(
    std::weak_ptr<GameObject> object, const std::string& key)
{
    mListeners[key].push_back(object);
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

    for (auto listener = listeners.begin();
         listener != listeners.end();)
    {
        auto object = listener->lock();

        if (!object)
        {
            listener = listeners.erase(listener);
        }
        else
        {
            object->ReceiveNotification(message);
            listener++;
        }
    }
}

}