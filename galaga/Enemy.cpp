#include "Enemy.h"

#include <cmath>

#include "Bullet.h"
#include "Player.h"
#include "GameContext.h"
#include "NotificationManager.h"

Enemy::Enemy(
    CMPUT350::Point2D loc,
    const std::vector<CMPUT350::Point2D>& path
)
    : mLocation(loc),
      mFormationLocation(loc),
      mAttackTarget(loc),
      mRotation(0.0f),
      mBounds(-18.0f, -18.0f, 36.0f, 36.0f),
      mPath(path),
      mPathTime(0.0f),
      mCurve(path),
      mPathDistance(0.0f),
      mEntering(true),
      mAttacking(false),
      mInFormation(false),
      mHealth(1),
      mEngineView(nullptr),
      mNotificationManager(nullptr)
{
    if (!mPath.empty())
    {
        mLocation = mPath.front();
    }

    mShapes.push_back(CMPUT350::Shape(mBounds));
}

void Enemy::Initialize(CMPUT350::GameContext* context)
{
    mEngineView = context->mEngineView;
    mNotificationManager = context->mNotificationManager;
    mSelf = context->CurrObject;
}

void Enemy::Update(CMPUT350::GameContext* context)
{
    if (mEntering || mAttacking)
    {
        int segmentCount =
            static_cast<int>((mPath.size() - 1) / 3);

        if (segmentCount > 0)
        {
            mPathDistance += 3.0f;
            mPathTime = mPathDistance >= mCurve.GetLength()
                ? static_cast<float>(segmentCount) : mCurve.GetTimeAtDistance(mPathDistance);

            if (mPathTime >= segmentCount)
            {
                mPathTime =
                    static_cast<float>(segmentCount);

                mLocation =
                    CMPUT350::Bezier::GetPoint(
                        mPath,
                        mPathTime
                    );

                mEntering = false;
                mAttacking = false;
                mInFormation = true;
                mRotation = 0.0f;
            }
            else
            {
                mLocation =
                    CMPUT350::Bezier::GetPoint(
                        mPath,
                        mPathTime
                    );

                CMPUT350::Point2D direction =
                    CMPUT350::Bezier::GetSlope(
                        mPath,
                        mPathTime
                    );

                mRotation =
                    std::atan2(
                        direction.y,
                        direction.x
                    );
            }
        }
    }
    else if (mInFormation)
    {
        float offset =
            std::sin(
                context->currentFrame * 0.03f
            ) * 8.0f;

        mLocation.x =
            mFormationLocation.x + offset;

        mLocation.y =
            mFormationLocation.y;

        mRotation = 0.0f;
    }
}

void Enemy::LateUpdate(CMPUT350::GameContext* context)
{
    if (mAttacking &&
        context->currentFrame % 90 == 0)
    {
        Fire(context);
    }
}

CMPUT350::Point2D Enemy::GetLocation() const
{
    return mLocation;
}

float Enemy::GetRotation() const
{
    return mRotation;
}

void Enemy::CollisionEnter(
    const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    if (!IsAlive())
    {
        return;
    }

    auto bullet = std::dynamic_pointer_cast<Bullet>(obj);

    if (bullet && bullet->IsPlayerBullet())
    {
        mHealth--;

        if (mHealth <= 0)
        {
            if (mNotificationManager != nullptr)
            {
                mNotificationManager->Notify("EnemyDied");

                int points = GetPointValue();

                if (points == 100)
                {
                    mNotificationManager->Notify("Score100");
                }
                else if (points == 200)
                {
                    mNotificationManager->Notify("Score200");
                }
                else
                {
                    mNotificationManager->Notify("Score300");
                }

                if (mAttacking)
                {
                    mNotificationManager->Notify("AttackBonus");
                }
            }

            Kill();
        }

        return;
    }

    auto player = std::dynamic_pointer_cast<Player>(obj);

    if (player)
    {
        if (mNotificationManager != nullptr)
        {
            mNotificationManager->Notify("EnemyDied");
        }

        Kill();
    }
}

const CMPUT350::Rect& Enemy::GetBounds()
{
    return mBounds;
}

const std::vector<CMPUT350::Shape>& Enemy::GetShapes()
{
    return mShapes;
}

bool Enemy::IsStatic() const
{
    return false;
}

bool Enemy::IsInFormation() const
{
    return mInFormation;
}

void Enemy::BeginAttack(
    const std::vector<CMPUT350::Point2D>& path,
    const CMPUT350::Point2D& target
)
{
    mPath = path;
    mPathTime = 0.0f;
    mCurve = CMPUT350::Bezier(path);
    mPathDistance = 0.0f;

    mAttackTarget = target;

    mAttacking = true;
    mInFormation = false;
}

void Enemy::Fire(CMPUT350::GameContext* context)
{
    CMPUT350::Point2D heading =
        mAttackTarget - mLocation;

    heading.Normalize();
    heading = heading * 8.0f;

    auto bullet = std::make_shared<Bullet>(
        mLocation,
        heading,
        false,
        mSelf
    );

    context->mEngineView->AddGameObject(bullet);
}




Enemy1::Enemy1(
    CMPUT350::Point2D loc,
    const std::vector<CMPUT350::Point2D>& path
)
    : Enemy(loc, path)
{
    mHealth = 1;
}

bool Enemy1::Attack(
    CMPUT350::GameContext* context,
    const CMPUT350::Point2D& target
)
{
    if (!mInFormation)
    {
        return false;
    }

    std::vector<CMPUT350::Point2D> path;

    path.push_back(mLocation);
    path.push_back(mLocation + CMPUT350::Point2D(-80, 80));
    path.push_back(target + CMPUT350::Point2D(-100, -100));
    path.push_back(target);

    path.push_back(target + CMPUT350::Point2D(100, 100));
    path.push_back(mFormationLocation + CMPUT350::Point2D(80, 80));
    path.push_back(mFormationLocation);

    BeginAttack(path, target);

    return true;
}

void Enemy1::RenderForeground(
    CMPUT350::GameContext* context)
{
    context->ScreenContext->DrawRect(
        CMPUT350::Rect(-18, -12, 36, 24),
        CMPUT350::Colors::green
    );

    context->ScreenContext->DrawCircle(
        CMPUT350::Point2D(0, 0),
        6,
        CMPUT350::Colors::yellow
    );
}

int Enemy1::GetPointValue() const
{
    return 100;
}






Enemy2::Enemy2(
    CMPUT350::Point2D loc,
    const std::vector<CMPUT350::Point2D>& path
)
    : Enemy(loc, path)
{
    mHealth = 2;
}

bool Enemy2::Attack(
    CMPUT350::GameContext* context,
    const CMPUT350::Point2D& target
)
{
    if (!mInFormation)
    {
        return false;
    }

    std::vector<CMPUT350::Point2D> path;

    path.push_back(mLocation);
    path.push_back(mLocation + CMPUT350::Point2D(120, 40));
    path.push_back(target + CMPUT350::Point2D(150, -120));
    path.push_back(target);

    path.push_back(target + CMPUT350::Point2D(-150, 100));
    path.push_back(mFormationLocation + CMPUT350::Point2D(-100, 50));
    path.push_back(mFormationLocation);

    BeginAttack(path, target);

    return true;
}

void Enemy2::RenderForeground(
    CMPUT350::GameContext* context)
{
    CMPUT350::RGBColor color =
        CMPUT350::Colors::red;

    if (mHealth == 1)
    {
        color =
            CMPUT350::Colors::yellow;
    }

    context->ScreenContext->DrawRect(
        CMPUT350::Rect(-20, -15, 40, 30),
        color
    );

    context->ScreenContext->DrawCircle(
        CMPUT350::Point2D(0, 0),
        7,
        CMPUT350::Colors::white
    );
}

int Enemy2::GetPointValue() const
{
    return 200;
}






Enemy3::Enemy3(
    CMPUT350::Point2D loc,
    const std::vector<CMPUT350::Point2D>& path
)
    : Enemy(loc, path)
{
    mHealth = 1;
}

bool Enemy3::Attack(
    CMPUT350::GameContext* context,
    const CMPUT350::Point2D& target
)
{
    if (!mInFormation)
    {
        return false;
    }

    std::vector<CMPUT350::Point2D> path;

    path.push_back(mLocation);
    path.push_back(
        CMPUT350::Point2D(
            mLocation.x,
            mLocation.y + 150
        )
    );

    path.push_back(
        CMPUT350::Point2D(
            target.x,
            target.y - 150
        )
    );

    path.push_back(target);

    path.push_back(
        CMPUT350::Point2D(
            target.x - 200,
            target.y
        )
    );

    path.push_back(
        CMPUT350::Point2D(
            mFormationLocation.x - 100,
            mFormationLocation.y
        )
    );

    path.push_back(mFormationLocation);

    BeginAttack(path, target);

    return true;
}

void Enemy3::RenderForeground(
    CMPUT350::GameContext* context)
{
    context->ScreenContext->DrawRect(
        CMPUT350::Rect(-16, -16, 32, 32),
        CMPUT350::Colors::magenta
    );

    context->ScreenContext->DrawCircle(
        CMPUT350::Point2D(0, 0),
        5,
        CMPUT350::Colors::cyan
    );
}

int Enemy3::GetPointValue() const
{
    return 300;
}