
#ifndef GAMEENGINE_H
#define GAMEENGINE_H

namespace CMPUT350 {
class GameEngine;
}

#include <memory>
#include <vector>

#include "EngineView.h"
#include "GameObject.h"
#include "MathUtil.h"
#include <SFML/Graphics.hpp>

namespace CMPUT350 {

class DrawContext;

class GameEngine : public EngineView {
public:
    /** Creates a window and initializes the engine resources.
     * @param width Window width in pixels.
     * @param height Window height in pixels.
     * @param name Window title.
     */
    GameEngine(unsigned int width, unsigned int height, const std::string& name);

    /** Closes the window if it is still open and releases engine resources.
     */
    ~GameEngine();

    GameEngine(const GameEngine&) = delete;             // Prevent copy-construction
    GameEngine(GameEngine&&) = delete;                  // Prevent move-construction
    GameEngine& operator=(const GameEngine&) = delete;  // Prevent assignment
    GameEngine& operator=(GameEngine&&) = delete;       // Prevent move-assignment

    /** Queues a non-null object to be initialized and activated on the next frame.
     * @param gameObject Object to add; null pointers are ignored.
     * @return No value.
     */
    void AddGameObject(std::shared_ptr<GameObject> gameObject) override;

    /** Returns an iterator to the first active game object. */
    EngineView::const_iterator cbegin() const override;

    /** Returns an iterator past the final active game object. */
    EngineView::const_iterator cend() const override;

    /** Returns an iterator to the first active game object. */
    EngineView::const_iterator begin() const override;

    /** Returns an iterator past the final active game object. */
    EngineView::const_iterator end() const override;

    /** Runs the event, update, collision, and rendering loop until the window closes.
     * @return No value.
     */
    void Run();

private:
    std::shared_ptr<sf::RenderWindow> mWindow;
    std::shared_ptr<sf::Font> mFont;
    std::vector<std::shared_ptr<GameObject>> mGameObjects;
    std::vector<std::shared_ptr<GameObject>> mPendingGameObjects;
};

}  // namespace CMPUT350

#endif  // GAMEENGINE_H
