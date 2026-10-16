#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <vector>

#include "CollisionObject.h"

class Bullet;

class Player : public CMPUT350::CollisionObject
{
public:
    Player(CMPUT350::Point2D loc);

    // GameObject Functions
    void Initialize(CMPUT350::GameContext* context) override;
    void Update(CMPUT350::GameContext* context) override;
    void LateUpdate(CMPUT350::GameContext* context) override;
    void ReceiveNotification(const std::string& message) override;

    // Graphics Object Functions
    void RenderBackground(CMPUT350::GameContext* context) override;
    void RenderForeground(CMPUT350::GameContext* context) override;

    CMPUT350::Point2D GetLocation() const override;
    float GetRotation() const override;

    // Collision Object Functions
    void CollisionEnter(
        const std::shared_ptr<CMPUT350::CollisionObject>& obj
    ) override;

    const CMPUT350::Rect& GetBounds() override;

    const std::vector<CMPUT350::Shape>& GetShapes() override;

    bool IsStatic() const override;

private:
    CMPUT350::Point2D mLocation;
    CMPUT350::Rect mBounds;

    std::vector<CMPUT350::Shape> mShapes;

    std::weak_ptr<Bullet> mBulletOne;
    std::weak_ptr<Bullet> mBulletTwo;

    CMPUT350::EngineView* mEngineView;
    CMPUT350::NotificationManager* mNotificationManager;

    std::weak_ptr<CMPUT350::GameObject> mSelf;

    float mScreenWidth;
};

#endif