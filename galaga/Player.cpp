#include "Player.h"

#include "Bullet.h"
#include "Enemy.h"
#include "GameContext.h"
#include "NotificationManager.h"

Player::Player(CMPUT350::Point2D loc)
    : mLocation(loc),
      mBounds(-20.0f, -20.0f, 40.0f, 40.0f),
      mEngineView(nullptr),
      mNotificationManager(nullptr),
      mScreenWidth(0.0f),
      mMoveLeft(false),
      mMoveRight(false)
{
    mShapes.push_back(CMPUT350::Shape(mBounds));
}

void Player::Initialize(CMPUT350::GameContext* context)
{
    mEngineView = context->mEngineView;
    mNotificationManager = context->mNotificationManager;
    mSelf = context->CurrObject;

    mScreenWidth = static_cast<float>(
        context->ScreenContext->GetWindowWidth()
    );

    mNotificationManager->Register(context->CurrObject, "MoveLeftDown");
    mNotificationManager->Register(context->CurrObject, "MoveLeftUp");

    mNotificationManager->Register(context->CurrObject, "MoveRightDown");
    mNotificationManager->Register(context->CurrObject, "MoveRightUp");

    mNotificationManager->Register(context->CurrObject, "Shoot");
}

void Player::Update(CMPUT350::GameContext* context)
{
    const float moveAmount = 10.0f;
    const float halfWidth = 20.0f;

    if (mMoveLeft)
    {
        mLocation.x -= moveAmount;
        if (mLocation.x < halfWidth)
        {
            mLocation.x = halfWidth;
        }
    }

    if (mMoveRight)
    {
        mLocation.x += moveAmount;
        if (mLocation.x > mScreenWidth - halfWidth)
        {
            mLocation.x = mScreenWidth - halfWidth;
        }
    }
}

void Player::LateUpdate(CMPUT350::GameContext* context)
{
}

void Player::ReceiveNotification(const std::string& message)
{
    if (message == "MoveLeftDown")
    {
        mMoveLeft = true;
    }
    else if (message == "MoveLeftUp")
    {
        mMoveLeft = false;
    }
    else if (message == "MoveRightDown")
    {
        mMoveRight = true;
    }
    else if (message == "MoveRightUp")
    {
        mMoveRight = false;
    }
    else if (message == "Shoot")
    {
        if (!mBulletOne.expired() && !mBulletTwo.expired())
        {
            return;
        }

        auto bullet = std::make_shared<Bullet>(
            CMPUT350::Point2D(mLocation.x, mLocation.y - 20.0f),
            CMPUT350::Point2D(0.0f, -15.0f),
            true,
            mSelf
        );

        mEngineView->AddGameObject(bullet);

        if (mBulletOne.expired())
        {
            mBulletOne = bullet;
        }
        else
        {
            mBulletTwo = bullet;
        }
    }
}

void Player::RenderBackground(CMPUT350::GameContext* context)
{
}

void Player::RenderForeground(CMPUT350::GameContext* context)
{
    CMPUT350::Rect body(-10.0f, -20.0f, 20.0f, 30.0f);
    CMPUT350::Rect leftWing(-20.0f, 0.0f, 10.0f, 15.0f);
    CMPUT350::Rect rightWing(10.0f, 0.0f, 10.0f, 15.0f);
    CMPUT350::Rect cockpit(-5.0f, -15.0f, 10.0f, 10.0f);

    context->ScreenContext->DrawRect(body, CMPUT350::Colors::blue);
    context->ScreenContext->DrawRect(leftWing, CMPUT350::Colors::blue);
    context->ScreenContext->DrawRect(rightWing, CMPUT350::Colors::blue);
    context->ScreenContext->DrawRect(cockpit, CMPUT350::Colors::cyan);
}

CMPUT350::Point2D Player::GetLocation() const
{
    return mLocation;
}

float Player::GetRotation() const
{
    return 0.0f;
}

void Player::CollisionEnter(
    const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    if (!IsAlive())
    {
        return;
    }

    auto enemy = std::dynamic_pointer_cast<Enemy>(obj);

    if (enemy)
    {
        Kill();

        if (mNotificationManager != nullptr)
        {
            mNotificationManager->Notify("PlayerDied");
        }

        return;
    }

    auto bullet = std::dynamic_pointer_cast<Bullet>(obj);

    if (bullet && !bullet->IsPlayerBullet())
    {
        Kill();

        if (mNotificationManager != nullptr)
        {
            mNotificationManager->Notify("PlayerDied");
        }
    }
}

const CMPUT350::Rect& Player::GetBounds()
{
    return mBounds;
}

const std::vector<CMPUT350::Shape>& Player::GetShapes()
{
    return mShapes;
}

bool Player::IsStatic() const
{
    return false;
}