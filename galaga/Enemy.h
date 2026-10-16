#ifndef ENEMY_H
#define ENEMY_H

#include <memory>
#include <vector>

#include "Bezier.h"
#include "CollisionObject.h"

namespace CMPUT350 {
class EngineView;
class NotificationManager;
class GameObject;
}

class Enemy : public CMPUT350::CollisionObject
{
public:
    Enemy(
        CMPUT350::Point2D loc,
        const std::vector<CMPUT350::Point2D>& path
    );

    // GameObject Functions
    void Initialize(CMPUT350::GameContext* context) override;
    void Update(CMPUT350::GameContext* context) override;
    void LateUpdate(CMPUT350::GameContext* context) override;

    // Graphics Object Functions
    CMPUT350::Point2D GetLocation() const override;
    float GetRotation() const override;

    // Collision Object Functions
    void CollisionEnter(
        const std::shared_ptr<CMPUT350::CollisionObject>& obj
    ) override;

    const CMPUT350::Rect& GetBounds() override;

    const std::vector<CMPUT350::Shape>& GetShapes() override;

    bool IsStatic() const override;

    virtual bool Attack(
        CMPUT350::GameContext* context,
        const CMPUT350::Point2D& target
    ) = 0;

    bool IsInFormation() const;

protected:
    void BeginAttack(
        const std::vector<CMPUT350::Point2D>& path,
        const CMPUT350::Point2D& target
    );

    void Fire(CMPUT350::GameContext* context);

    virtual int GetPointValue() const = 0;

    CMPUT350::Point2D mLocation;
    CMPUT350::Point2D mFormationLocation;
    CMPUT350::Point2D mAttackTarget;

    float mRotation;

    CMPUT350::Rect mBounds;

    std::vector<CMPUT350::Shape> mShapes;
    std::vector<CMPUT350::Point2D> mPath;

    float mPathTime;

    bool mEntering;
    bool mAttacking;
    bool mInFormation;

    int mHealth;

    CMPUT350::EngineView* mEngineView;
    CMPUT350::NotificationManager* mNotificationManager;

    std::weak_ptr<CMPUT350::GameObject> mSelf;
};

class Enemy1 : public Enemy
{
public:
    Enemy1(
        CMPUT350::Point2D loc,
        const std::vector<CMPUT350::Point2D>& path
    );

    bool Attack(
        CMPUT350::GameContext* context,
        const CMPUT350::Point2D& target
    ) override;

    void RenderForeground(
        CMPUT350::GameContext* context
    ) override;

protected:
    int GetPointValue() const override;
};

class Enemy2 : public Enemy
{
public:
    Enemy2(
        CMPUT350::Point2D loc,
        const std::vector<CMPUT350::Point2D>& path
    );

    bool Attack(
        CMPUT350::GameContext* context,
        const CMPUT350::Point2D& target
    ) override;

    void RenderForeground(
        CMPUT350::GameContext* context
    ) override;

protected:
    int GetPointValue() const override;
};

class Enemy3 : public Enemy
{
public:
    Enemy3(
        CMPUT350::Point2D loc,
        const std::vector<CMPUT350::Point2D>& path
    );

    bool Attack(
        CMPUT350::GameContext* context,
        const CMPUT350::Point2D& target
    ) override;

    void RenderForeground(
        CMPUT350::GameContext* context
    ) override;

protected:
    int GetPointValue() const override;
};

#endif