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

    Rect rect = context.Transform(shape.shape.rect);

    return Shape(rect);
}

bool GameEngine::CheckShapeCollision(const Shape& first, const Shape& second,
                                     Point2D& collisionPoint, bool& hasCollisionPoint) {
    hasCollisionPoint = false;

    // Line-line
    if (first.t == ShapeType::kLine && second.t == ShapeType::kLine) {
        bool collided = LineLineCollision(first.shape.line, second.shape.line, collisionPoint);

        hasCollisionPoint = collided;
        return collided;
    }

    // Line-circle
    if (first.t == ShapeType::kLine && second.t == ShapeType::kCircle) {
        bool collided = LineCircleCollision(first.shape.line, second.shape.circle, collisionPoint);

        hasCollisionPoint = collided;
        return collided;
    }

    // Circle-line
    if (first.t == ShapeType::kCircle && second.t == ShapeType::kLine) {
        bool collided = LineCircleCollision(second.shape.line, first.shape.circle, collisionPoint);

        hasCollisionPoint = collided;
        return collided;
    }

    // Line-rectangle
    if (first.t == ShapeType::kLine && second.t == ShapeType::kRect) {
        bool collided = LineRectCollision(first.shape.line, second.shape.rect, collisionPoint);

        hasCollisionPoint = collided;
        return collided;
    }

    // Rectangle-line
    if (first.t == ShapeType::kRect && second.t == ShapeType::kLine) {
        bool collided = LineRectCollision(second.shape.line, first.shape.rect, collisionPoint);

        hasCollisionPoint = collided;
        return collided;
    }

    // Circle-circle
    if (first.t == ShapeType::kCircle && second.t == ShapeType::kCircle) {
        return CircleCircleCollision(first.shape.circle, second.shape.circle);
    }

    // Circle-rectangle
    if (first.t == ShapeType::kCircle && second.t == ShapeType::kRect) {
        return CircleRectCollision(first.shape.circle, second.shape.rect);
    }

    // Rectangle-circle
    if (first.t == ShapeType::kRect && second.t == ShapeType::kCircle) {
        return CircleRectCollision(second.shape.circle, first.shape.rect);
    }

    // Rectangle-rectangle
    if (first.t == ShapeType::kRect && second.t == ShapeType::kRect) {
        return RectRectCollision(first.shape.rect, second.shape.rect);
    }

    return false;
}

void GameEngine::ProcessCollisions(DrawContext& context) {
    std::vector<std::shared_ptr<CollisionObject>> collisionObjects;

    // Collect all active objects that support collision.
    for (const auto& object : mGameObjects) {
        auto collisionObject = std::dynamic_pointer_cast<CollisionObject>(object);

        if (collisionObject) {
            collisionObjects.push_back(collisionObject);
        }
    }

    // Compare each unordered object pair exactly once.
    for (std::size_t firstIndex = 0; firstIndex < collisionObjects.size(); firstIndex++) {
        std::shared_ptr<CollisionObject> first = collisionObjects[firstIndex];

        for (std::size_t secondIndex = firstIndex + 1; secondIndex < collisionObjects.size();
             secondIndex++) {
            std::shared_ptr<CollisionObject> second = collisionObjects[secondIndex];

            /*
             * Broad phase:
             * transform each object's local bounds into world space.
             */
            context.SetContextOffset(first->GetLocation());

            context.SetContextRotation(first->GetRotation());

            Rect firstWorldBounds = context.Transform(first->GetBounds());

            context.SetContextOffset(second->GetLocation());

            context.SetContextRotation(second->GetRotation());

            Rect secondWorldBounds = context.Transform(second->GetBounds());

            // Skip component checks if the world bounds do not overlap.
            if (!firstWorldBounds.overlaps(secondWorldBounds)) {
                continue;
            }

            /*
             * Transform the first object's component shapes.
             */
            std::vector<Shape> firstWorldShapes;

            context.SetContextOffset(first->GetLocation());

            context.SetContextRotation(first->GetRotation());

            for (const Shape& localShape : first->GetShapes()) {
                firstWorldShapes.push_back(TransformShape(localShape, context));
            }

            /*
             * Transform the second object's component shapes.
             */
            std::vector<Shape> secondWorldShapes;

            context.SetContextOffset(second->GetLocation());

            context.SetContextRotation(second->GetRotation());

            for (const Shape& localShape : second->GetShapes()) {
                secondWorldShapes.push_back(TransformShape(localShape, context));
            }

            bool objectsCollided = false;
            bool foundCollisionPoint = false;

            Point2D worldCollisionPoint;

            /*
             * Distance is used to retain the earliest directed-line
             * collision if multiple components overlap.
             */
            float shortestDistanceSquared = 0.0f;

            for (const Shape& firstShape : firstWorldShapes) {
                for (const Shape& secondShape : secondWorldShapes) {
                    Point2D candidatePoint;
                    bool candidateHasPoint = false;

                    bool shapesCollided = CheckShapeCollision(firstShape, secondShape,
                                                              candidatePoint, candidateHasPoint);

                    if (!shapesCollided) {
                        continue;
                    }

                    objectsCollided = true;

                    if (candidateHasPoint) {
                        Point2D lineStart;

                        /*
                         * Use the start of the directed line to decide
                         * which collision point occurs first.
                         *
                         * For line-line, the first shape's direction
                         * is used.
                         */
                        if (firstShape.t == ShapeType::kLine) {
                            lineStart = firstShape.shape.line.p1;
                        } else {
                            lineStart = secondShape.shape.line.p1;
                        }

                        float differenceX = candidatePoint.x - lineStart.x;

                        float differenceY = candidatePoint.y - lineStart.y;

                        float distanceSquared =
                            differenceX * differenceX + differenceY * differenceY;

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

            /*
             * Deliver one collision callback to each object in the pair.
             */
            if (!foundCollisionPoint) {
                first->CollisionEnter(second);
                second->CollisionEnter(first);
                continue;
            }

            /*
             * Convert the same world collision point into each
             * receiving object's local coordinate system.
             */
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
        ProcessCollisions(screenContext);

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
