#include "GameEngine.h"

#include "CollisionObject.h"
#include "CollisionGeometry.h"
#include <algorithm>
#include "DrawContext.h"
#include "GameContext.h"
#include "GraphicsObject.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

/// @brief
namespace CMPUT350 {
#include "FontData.h"

GameEngine::GameEngine(unsigned int width, unsigned int height, const std::string& name) {
    const auto desktop = sf::VideoMode::getDesktopMode().size;
    float scale = std::min({1.0f, desktop.x * 0.85f / width, desktop.y * 0.85f / height});
    sf::Vector2u windowSize(std::max(1u, static_cast<unsigned int>(width * scale)),
                            std::max(1u, static_cast<unsigned int>(height * scale)));
    mWindow = std::make_shared<sf::RenderWindow>(sf::VideoMode(windowSize), name);
    mWindow->setView(sf::View(sf::FloatRect({0.0f, 0.0f},
                                          {static_cast<float>(width), static_cast<float>(height)})));
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

void GameEngine::ProcessCollisions(DrawContext& context) {
    struct Contact {
        std::shared_ptr<CollisionObject> first, second;
        Point2D point;
        bool hasPoint;
        float distance;
    };
    std::vector<Contact> contacts;
    std::vector<std::shared_ptr<CollisionObject>> objects;
    for (const auto& object : mGameObjects) {
        auto collision = std::dynamic_pointer_cast<CollisionObject>(object);
        if (collision && collision->IsAlive()) objects.push_back(collision);
    }
    for (std::size_t i = 0; i < objects.size(); ++i) {
        auto first = objects[i];
        ObjectPose fp{first->GetLocation(), first->GetRotation()};
        context.SetContextOffset(fp.location);
        context.SetContextRotation(fp.rotation);
        Rect firstBounds = context.Transform(first->GetBounds());
        for (std::size_t j = i + 1; j < objects.size(); ++j) {
            auto second = objects[j];
            ObjectPose sp{second->GetLocation(), second->GetRotation()};
            context.SetContextOffset(sp.location);
            context.SetContextRotation(sp.rotation);
            if (!firstBounds.overlaps(context.Transform(second->GetBounds()))) continue;
            bool collided = false, hasPoint = false;
            Point2D point;
            float nearest = 0;
            for (const Shape& a : first->GetShapes()) {
                for (const Shape& b : second->GetShapes()) {
                    Point2D candidate;
                    bool candidateHasPoint = false;
                    if (!ComponentCollision(a, fp, b, sp, candidate, candidateHasPoint)) continue;
                    collided = true;
                    if (!candidateHasPoint) continue;
                    Point2D origin = a.t == ShapeType::kLine ? fp.World(a.shape.line.p1)
                                                                          : sp.World(b.shape.line.p1);
                    Point2D delta = candidate - origin;
                    float distance = delta.Dot(delta);
                    if (!hasPoint || distance < nearest) {
                        hasPoint = true;
                        nearest = distance;
                        point = candidate;
                    }
                }
            }
            if (collided) contacts.push_back({first, second, point, hasPoint, nearest});
        }
    }
    // Resolve nearer line hits first so a destroyed bullet cannot damage a farther target.
    std::stable_sort(contacts.begin(), contacts.end(), [](const Contact& a, const Contact& b) {
        return a.distance < b.distance;
    });
    for (const Contact& contact : contacts) {
        if (!contact.first->IsAlive() || !contact.second->IsAlive()) continue;
        if (contact.hasPoint) {
            ObjectPose fp{contact.first->GetLocation(), contact.first->GetRotation()};
            ObjectPose sp{contact.second->GetLocation(), contact.second->GetRotation()};
            // Both sides receive this contact even if the first callback kills an object.
            contact.first->CollisionEnter(contact.second, fp.Local(contact.point));
            contact.second->CollisionEnter(contact.first, sp.Local(contact.point));
        } else {
            contact.first->CollisionEnter(contact.second);
            contact.second->CollisionEnter(contact.first);
        }
    }
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
            if (object && object->IsAlive()) {
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
            if (object && object->IsAlive()) {
                context.CurrObject = object;
                object->Update(&context);
            }
        }

        // 4. Process collision events
        ProcessCollisions(screenContext);

        // 5. Late updates
        for (const auto& object : mGameObjects) {
            if (object && object->IsAlive()) {
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
                context.CurrObject = object;

                screenContext.SetContextOffset(graphicsObject->GetLocation());

                screenContext.SetContextRotation(graphicsObject->GetRotation());

                graphicsObject->RenderBackground(&context);
            }
        }

        // 7. Render foreground
        for (const auto& object : mGameObjects) {
            auto graphicsObject = std::dynamic_pointer_cast<GraphicsObject>(object);

            if (graphicsObject) {
                context.CurrObject = object;

                screenContext.SetContextOffset(graphicsObject->GetLocation());

                screenContext.SetContextRotation(graphicsObject->GetRotation());

                graphicsObject->RenderForeground(&context);
            }
        }

        guiContext.SetContextOffset(Point2D(0.0f, 0.0f));
        guiContext.SetContextRotation(0.0f);

        for (const auto& object : mGameObjects) {
            if (object) {
                context.CurrObject = object;
                object->RenderUI(&context);
            }
        }

        // Actually render to window
        mWindow->display();
        context.currentFrame++;
    }
}
}  // namespace CMPUT350
