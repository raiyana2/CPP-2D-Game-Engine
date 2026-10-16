#include "GameEngine.h"

#include "CollisionObject.h"
#include "DrawContext.h"
#include "GameContext.h"
#include "GraphicsObject.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

/// @brief
namespace CMPUT350 {
#include "FontData.h"

GameEngine::GameEngine(unsigned int width, unsigned int height, const std::string& name) {
    mWindow = std::make_shared<sf::RenderWindow>(sf::VideoMode({width, height}), name);
    mFont = std::make_shared<sf::Font>();

    // Sample font loading code
    if (!mFont->openFromMemory(&_font, _font_len)) {
        fprintf(stderr, "WARNING: Font did not load.\n");
    }

    mWindow->setFramerateLimit(30);  // Limit the framerate to 30 frames per second
}

GameEngine::~GameEngine() {
    // Cleanup resources
    if (mWindow && mWindow->isOpen()) {
        mWindow->close();
    }
}

void GameEngine::AddGameObject(std::shared_ptr<GameObject> gameObject) {
    if (gameObject != nullptr) {
        mPendingGameObjects.push_back(std::move(gameObject));
    }
}

EngineView::const_iterator GameEngine::cbegin() const { return mGameObjects.cbegin(); }

EngineView::const_iterator GameEngine::cend() const { return mGameObjects.cend(); }

EngineView::const_iterator GameEngine::begin() const { return mGameObjects.cbegin(); }

EngineView::const_iterator GameEngine::end() const { return mGameObjects.cend(); }

void GameEngine::InstallKeyDownNotification(int key, const std::string& notification) {
    mKeyDownNotifications[key].push_back(notification);
}

void GameEngine::InstallKeyUpNotification(int key, const std::string& notification) {
    mKeyUpNotifications[key].push_back(notification);
}

/** Runs one frame at a time until the render window closes.
 * Each frame removes dead objects, activates queued objects, dispatches input,
 * updates active objects, reports overlapping collision pairs to both objects,
 * runs late updates, then renders background and foreground content. Objects
 * queued during a frame are initialized at the beginning of the next frame.
 * The loop does not stop merely because no game objects remain.
 */
void GameEngine::Run() {
    DrawContext screenContext(mWindow, mFont);
    DrawContext guiContext(mWindow, mFont);

    GameContext context;

    context.mEngineView = this;
    context.ScreenContext = &screenContext;
    context.GUIContext = &guiContext;
    context.mNotificationManager = &mNotificationManager;
    context.currentFrame = 0;

    while (mWindow->isOpen())  // window is open
    {
        // 0. Remove any objects that are now dead
        std::vector<std::shared_ptr<GameObject>> livingObjects;
        for (const std::shared_ptr<GameObject>& gameObject : mGameObjects) {
            if (gameObject && gameObject->IsAlive()) {
                livingObjects.push_back(gameObject);
            }
        }
        mGameObjects = std::move(livingObjects);

        // 1. Activate and initialize any objects added during the last frame
        std::vector<std::shared_ptr<GameObject>> objectsToActivate = std::move(mPendingGameObjects);
        mPendingGameObjects.clear();

        for (std::shared_ptr<GameObject>& object : objectsToActivate) {
            if (object) {
                context.CurrObject = object;
                object->Initialize(&context);
                mGameObjects.push_back(std::move(object));
            }
        }

        // 2. Process events
        bool shouldClose = false;
        while (const std::optional event = mWindow->pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                mWindow->close();
                shouldClose = true;
                break;
            } else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                int key = static_cast<int>(keyPressed->scancode);

                auto found = mKeyDownNotifications.find(key);

                if (found != mKeyDownNotifications.end()) {
                    for (const std::string& notification : found->second) {
                        mNotificationManager.Notify(notification);
                    }
                }
            } else if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
                int key = static_cast<int>(keyReleased->scancode);

                auto found = mKeyUpNotifications.find(key);

                if (found != mKeyUpNotifications.end()) {
                    for (const std::string& notification : found->second) {
                        mNotificationManager.Notify(notification);
                    }
                }
            }
        }

        if (shouldClose) {
            break;
        }

        // 3. Update game objects
        for (const auto& object : mGameObjects) {
            if (object) {
                context.CurrObject = object;
                object->Update(&context);
            }
        }

        // 4. Process collision events
        std::vector<std::shared_ptr<CollisionObject>> collisionObjects;

        for (const auto& object : mGameObjects) {
            auto collision = std::dynamic_pointer_cast<CollisionObject>(object);

            if (collision) {
                collisionObjects.push_back(std::move(collision));
            }
        }

        for (std::size_t i = 0; i < collisionObjects.size(); i++) {
            auto& first = collisionObjects[i];
            for (std::size_t j = i + 1; j < collisionObjects.size(); j++) {
                auto& second = collisionObjects[j];

                const Rect a = first->GetBounds();
                const Rect b = second->GetBounds();

                bool overlap = a.overlaps(b);

                if (overlap) {
                    first->CollisionEnter(second);
                    second->CollisionEnter(first);
                }
            }
        }

        // 5. Late updates
        for (const auto& object : mGameObjects) {
            if (object) {
                context.CurrObject = object;
                object->LateUpdate(&context);
            }
        }

        // Clear window
        mWindow->clear();

        // 6. Render background
        for (const auto& object : mGameObjects) {
            auto graphicsObject = std::dynamic_pointer_cast<GraphicsObject>(object);
            if (graphicsObject) {
                graphicsObject->RenderBackground(&context);
            }
        }

        // 7. Render foreground
        for (const auto& object : mGameObjects) {
            auto graphicsObject = std::dynamic_pointer_cast<GraphicsObject>(object);
            if (graphicsObject) {
                graphicsObject->RenderForeground(&context);
            }
        }

        // Actually render to window
        mWindow->display();
        context.currentFrame++;
    }
}
}  // namespace CMPUT350
