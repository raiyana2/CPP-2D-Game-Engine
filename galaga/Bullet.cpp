#include "Bullet.h"

#include "Enemy.h"
#include "Player.h"
#include "GameContext.h"

Bullet::Bullet(
    CMPUT350::Point2D location,
    CMPUT350::Point2D heading,
    bool player,
    std::weak_ptr<CMPUT350::GameObject> parent
)
    : mLocation(location),
      mPreviousLocation(location),
      mHeading(heading),
      mBounds(CMPUT350::Point2D(0, 0), heading),
      mPlayerBullet(player),
      mParent(parent),
      mHasCollisionPoint(false),
      mCollisionPoint(0, 0)
{
    mBounds.Inset(-2);

    mShapes.push_back(
        CMPUT350::Shape(
            CMPUT350::Line(
                CMPUT350::Point2D(0, 0),
                mHeading
            )
        )
    );
}

bool Bullet::IsPlayerBullet()
{
    return mPlayerBullet;
}

void Bullet::Update(CMPUT350::GameContext* context)
{
    auto parent = mParent.lock();

    if (!parent || !parent->IsAlive())
    {
        Kill();
        return;
    }

    mPreviousLocation = mLocation;
    mLocation += mHeading;

    mShapes.clear();

    mShapes.push_back(
        CMPUT350::Shape(
            CMPUT350::Line(
                CMPUT350::Point2D(0, 0),
                mHeading
            )
        )
    );

    mBounds = CMPUT350::Rect(
        CMPUT350::Point2D(0, 0),
        mHeading
    );

    mBounds.Inset(-2);

    int width = context->ScreenContext->GetWindowWidth();
    int height = context->ScreenContext->GetWindowHeight();

    if (mLocation.x < 0 ||
        mLocation.x > width ||
        mLocation.y < 0 ||
        mLocation.y > height)
    {
        Kill();
    }
}

void Bullet::LateUpdate(CMPUT350::GameContext* context)
{
}

bool Bullet::IsAlive() const
{
    return CMPUT350::GameObject::IsAlive();
}

void Bullet::Kill()
{
    CMPUT350::GameObject::Kill();
}

void Bullet::RenderForeground(CMPUT350::GameContext* context)
{
    if (!IsAlive() && mHasCollisionPoint)
    {
        context->ScreenContext->DrawCircle(
            mCollisionPoint,
            4.0f,
            CMPUT350::Colors::yellow
        );

        return;
    }

    if (!IsAlive()) return;

    CMPUT350::RGBColor color = CMPUT350::Colors::yellow;

    if (!mPlayerBullet)
    {
        color = CMPUT350::Colors::red;
    }

    context->ScreenContext->DrawLine(
        CMPUT350::Point2D(0, 0),
        mHeading,
        2.0f,
        color
    );
}

CMPUT350::Point2D Bullet::GetLocation() const
{
    return mPreviousLocation;
}

float Bullet::GetRotation() const
{
    return 0.0f;
}

void Bullet::CollisionEnter(
    const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    if (mPlayerBullet)
    {
        auto enemy = std::dynamic_pointer_cast<Enemy>(obj);

        if (enemy)
        {
            Kill();
        }
    }
    else
    {
        auto player = std::dynamic_pointer_cast<Player>(obj);

        if (player)
        {
            Kill();
        }
    }
}

void Bullet::CollisionEnter(
    const std::shared_ptr<CMPUT350::CollisionObject>& obj,
    const CMPUT350::Point2D& collisionPoint)
{
    bool validCollision = false;

    if (mPlayerBullet)
    {
        if (std::dynamic_pointer_cast<Enemy>(obj))
        {
            validCollision = true;
        }
    }
    else
    {
        if (std::dynamic_pointer_cast<Player>(obj))
        {
            validCollision = true;
        }
    }

    if (!validCollision)
    {
        return;
    }

    if (!mHasCollisionPoint ||
        collisionPoint.y > mCollisionPoint.y)
    {
        mCollisionPoint = collisionPoint;
        mHasCollisionPoint = true;
    }

    Kill();
}

const CMPUT350::Rect& Bullet::GetBounds()
{
    return mBounds;
}

const std::vector<CMPUT350::Shape>& Bullet::GetShapes()
{
    return mShapes;
}

bool Bullet::IsStatic() const
{
    return false;
}