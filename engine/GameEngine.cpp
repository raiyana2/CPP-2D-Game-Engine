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
    const auto desktop = sf::VideoMode::getDesktopMode().size;
    float scale = std::min({1.0f, desktop.x * 0.85f / width, desktop.y * 0.85f / height});
    sf::Vector2u windowSize(std::max(1u, static_cast<unsigned int>(width * scale)),
                            std::max(1u, static_cast<unsigned int>(height * scale)));
    mWindow = std::make_shared<sf::RenderWindow>(sf::VideoMode(windowSize), name);
    mWindow->setView(sf::View(
        sf::FloatRect({0.0f, 0.0f}, {static_cast<float>(width), static_cast<float>(height)})));
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

Shape GameEngine::TransformShape(const Shape& shape, DrawContext& context) {
    if (shape.t == ShapeType::kLine) {
        Line line = shape.shape.line;
        line.p1 = context.Transform(line.p1);
        line.p2 = context.Transform(line.p2);
        return Shape(line);
    }

    if (shape.t == ShapeType::kCircle) {
        Circle circle = shape.shape.circle;
        circle.center = context.Transform(circle.center);
        return Shape(circle);
    }

    return Shape(context.Transform(shape.shape.rect));
}

bool GameEngine::CheckShapeCollision(const Shape& first, const Shape& second,
                                     Point2D& collisionPoint, bool& hasCollisionPoint) {
    hasCollisionPoint = false;

    if (first.t == ShapeType::kLine && second.t == ShapeType::kLine) {
        bool collided = LineLineCollision(first.shape.line, second.shape.line, collisionPoint);
        hasCollisionPoint = collided;
        return collided;
    }

    if (first.t == ShapeType::kLine && second.t == ShapeType::kCircle) {
        bool collided = LineCircleCollision(first.shape.line, second.shape.circle, collisionPoint);
        hasCollisionPoint = collided;
        return collided;
    }

    if (first.t == ShapeType::kCircle && second.t == ShapeType::kLine) {
        bool collided = LineCircleCollision(second.shape.line, first.shape.circle, collisionPoint);
        hasCollisionPoint = collided;
        return collided;
    }

    if (first.t == ShapeType::kLine && second.t == ShapeType::kRect) {
        bool collided = LineRectCollision(first.shape.line, second.shape.rect, collisionPoint);
        hasCollisionPoint = collided;
        return collided;
    }

    if (first.t == ShapeType::kRect && second.t == ShapeType::kLine) {
        bool collided = LineRectCollision(second.shape.line, first.shape.rect, collisionPoint);
        hasCollisionPoint = collided;
        return collided;
    }

    if (first.t == ShapeType::kCircle && second.t == ShapeType::kCircle) {
        return CircleCircleCollision(first.shape.circle, second.shape.circle);
    }

    if (first.t == ShapeType::kCircle && second.t == ShapeType::kRect) {
        return CircleRectCollision(first.shape.circle, second.shape.rect);
    }

    if (first.t == ShapeType::kRect && second.t == ShapeType::kCircle) {
        return CircleRectCollision(second.shape.circle, first.shape.rect);
    }

    if (first.t == ShapeType::kRect && second.t == ShapeType::kRect) {
        return RectRectCollision(first.shape.rect, second.shape.rect);
    }

    return false;
}

void GameEngine::ProcessCollisions(DrawContext& context) {
    std::vector<std::shared_ptr<CollisionObject>> collisionObjects;

    for (const auto& object : mGameObjects) {
        auto collisionObject = std::dynamic_pointer_cast<CollisionObject>(object);
        if (collisionObject && collisionObject->IsAlive()) {
            collisionObjects.push_back(collisionObject);
        }
    }

    for (std::size_t i = 0; i < collisionObjects.size(); ++i) {
        auto first = collisionObjects[i];

        for (std::size_t j = i + 1; j < collisionObjects.size(); ++j) {
            auto second = collisionObjects[j];

            if (!first->IsAlive() || !second->IsAlive()) {
                continue;
            }

            context.SetContextOffset(first->GetLocation());
            context.SetContextRotation(first->GetRotation());
            Rect firstWorldBounds = context.Transform(first->GetBounds());

            context.SetContextOffset(second->GetLocation());
            context.SetContextRotation(second->GetRotation());
            Rect secondWorldBounds = context.Transform(second->GetBounds());

            if (!firstWorldBounds.overlaps(secondWorldBounds)) {
                continue;
            }

            std::vector<Shape> firstWorldShapes;
            context.SetContextOffset(first->GetLocation());
            context.SetContextRotation(first->GetRotation());
            for (const Shape& shape : first->GetShapes()) {
                firstWorldShapes.push_back(TransformShape(shape, context));
            }

            std::vector<Shape> secondWorldShapes;
            context.SetContextOffset(second->GetLocation());
            context.SetContextRotation(second->GetRotation());
            for (const Shape& shape : second->GetShapes()) {
                secondWorldShapes.push_back(TransformShape(shape, context));
            }

            bool objectsCollided = false;
            bool foundCollisionPoint = false;
            Point2D worldCollisionPoint;
            float shortestDistanceSquared = 0.0f;

            for (const Shape& firstShape : firstWorldShapes) {
                for (const Shape& secondShape : secondWorldShapes) {
                    Point2D candidatePoint;
                    bool candidateHasPoint = false;

                    if (!CheckShapeCollision(firstShape, secondShape, candidatePoint,
                                             candidateHasPoint)) {
                        continue;
                    }

                    objectsCollided = true;

                    if (candidateHasPoint) {
                        Point2D lineStart = firstShape.t == ShapeType::kLine
                                                ? firstShape.shape.line.p1
                                                : secondShape.shape.line.p1;
                        Point2D difference = candidatePoint - lineStart;
                        float distanceSquared = difference.Dot(difference);

                        if (!foundCollisionPoint || distanceSquared < shortestDistanceSquared) {
                            foundCollisionPoint = true;
                            shortestDistanceSquared = distanceSquared;
                            worldCollisionPoint = candidatePoint;
                        }
                    }
                }
            }

            if (!objectsCollided) {
                continue;
            }

            if (!foundCollisionPoint) {
                first->CollisionEnter(second);
                second->CollisionEnter(first);
                continue;
            }

            context.SetContextOffset(first->GetLocation());
            context.SetContextRotation(first->GetRotation());
            Point2D firstLocalPoint = context.ReverseTransform(worldCollisionPoint);

            context.SetContextOffset(second->GetLocation());
            context.SetContextRotation(second->GetRotation());
            Point2D secondLocalPoint = context.ReverseTransform(worldCollisionPoint);

            first->CollisionEnter(second, firstLocalPoint);
            second->CollisionEnter(first, secondLocalPoint);
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
