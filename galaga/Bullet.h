#ifndef BULLET_H
#define BULLET_H

#include <memory>
#include <vector>

#include "CollisionObject.h"

class Bullet : public CMPUT350::CollisionObject
{
public:
    Bullet(
        CMPUT350::Point2D location,
        CMPUT350::Point2D heading,
        bool player,
        std::weak_ptr<CMPUT350::GameObject> parent
    );

    bool IsPlayerBullet();

    // GameObject Functions
    void Update(CMPUT350::GameContext* context) override;
    void LateUpdate(CMPUT350::GameContext* context) override;

    bool IsAlive() const override;
    void Kill() override;

    // Graphics Object Functions
    void RenderForeground(CMPUT350::GameContext* context) override;

    CMPUT350::Point2D GetLocation() const override;
    float GetRotation() const override;

    // Collision Object Functions
    void CollisionEnter(
        const std::shared_ptr<CMPUT350::CollisionObject>& obj
    ) override;

    void CollisionEnter(
        const std::shared_ptr<CMPUT350::CollisionObject>& obj,
        const CMPUT350::Point2D& collisionPoint
    ) override;

    const CMPUT350::Rect& GetBounds() override;

    const std::vector<CMPUT350::Shape>& GetShapes() override;

    bool IsStatic() const override;

private:
    CMPUT350::Point2D mLocation;
    CMPUT350::Point2D mPreviousLocation;
    CMPUT350::Point2D mHeading;

    CMPUT350::Rect mBounds;
    std::vector<CMPUT350::Shape> mShapes;

    bool mPlayerBullet;

    std::weak_ptr<CMPUT350::GameObject> mParent;

    bool mHasCollisionPoint;
    CMPUT350::Point2D mCollisionPoint;
};

#endif