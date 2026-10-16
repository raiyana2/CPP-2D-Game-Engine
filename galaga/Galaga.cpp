#include "Galaga.h"

#include "Enemy.h"
#include "Player.h"
#include "GameContext.h"
#include "NotificationManager.h"
#include "Keyboard.h"


Galaga::Galaga(int width, int height)
    : mWidth(width),
      mHeight(height),
      mState(State::Waiting),
      mScore(0),
      mLives(3),
      mLevel(1),
      mWave(0),
      mEnemiesAlive(0),
      mStateStartFrame(0),
      mLastWaveFrame(0),
      mCoinInserted(false),
      mStartRequested(false),
      mPlayerNeedsRespawn(false),
      mGameOverRequested(false),
      mEngineView(nullptr),
      mNotificationManager(nullptr)
{
}




void Galaga::Initialize(CMPUT350::GameContext* context)
{
    mEngineView = context->mEngineView;
    mNotificationManager = context->mNotificationManager;

    // Galaga listens for these game notifications
    mNotificationManager->Register(
        context->CurrObject,
        "InsertCoin"
    );

    mNotificationManager->Register(
        context->CurrObject,
        "StartGame"
    );

    mNotificationManager->Register(
        context->CurrObject,
        "EnemyDied"
    );

    mNotificationManager->Register(
        context->CurrObject,
        "PlayerDied"
    );

    mNotificationManager->Register(
        context->CurrObject,
        "Score100"
    );

    mNotificationManager->Register(
        context->CurrObject,
        "Score200"
    );

    mNotificationManager->Register(
        context->CurrObject,
        "Score300"
    );

    mNotificationManager->Register(
        context->CurrObject,
        "AttackBonus"
    );


    // Player movement
    context->mEngineView->InstallKeyDownNotification(
        CMPUT350::Key::A,
        "MoveLeftDown"
    );

    context->mEngineView->InstallKeyUpNotification(
        CMPUT350::Key::A,
        "MoveLeftUp"
    );

    context->mEngineView->InstallKeyDownNotification(
        CMPUT350::Key::D,
        "MoveRightDown"
    );

    context->mEngineView->InstallKeyUpNotification(
        CMPUT350::Key::D,
        "MoveRightUp"
    );


    // Arrow keys also move the player
    context->mEngineView->InstallKeyDownNotification(
        CMPUT350::Key::Left,
        "MoveLeftDown"
    );

    context->mEngineView->InstallKeyUpNotification(
        CMPUT350::Key::Left,
        "MoveLeftUp"
    );

    context->mEngineView->InstallKeyDownNotification(
        CMPUT350::Key::Right,
        "MoveRightDown"
    );

    context->mEngineView->InstallKeyUpNotification(
        CMPUT350::Key::Right,
        "MoveRightUp"
    );


    // Shoot
    context->mEngineView->InstallKeyDownNotification(
        CMPUT350::Key::Space,
        "Shoot"
    );


    // Galaga controls
    context->mEngineView->InstallKeyDownNotification(
        CMPUT350::Key::Num5,
        "InsertCoin"
    );

    context->mEngineView->InstallKeyDownNotification(
        CMPUT350::Key::Num1,
        "StartGame"
    );
}




void Galaga::ReceiveNotification(const std::string& message)
{
    if (message == "InsertCoin")
    {
        mCoinInserted = true;
    }
    else if (message == "StartGame")
    {
        mStartRequested = true;
    }
    else if (message == "EnemyDied")
    {
        if (mEnemiesAlive > 0)
        {
            mEnemiesAlive--;
        }
    }
    else if (message == "PlayerDied")
    {
        mLives--;

        if (mLives <= 0)
        {
            mGameOverRequested = true;
        }
        else
        {
            mPlayerNeedsRespawn = true;
        }
    }
    else if (message == "Score100")
    {
        mScore += 100;
    }
    else if (message == "Score200")
    {
        mScore += 200;
    }
    else if (message == "Score300")
    {
        mScore += 300;
    }
    else if (message == "AttackBonus")
    {
        mScore += 100;
    }
}







void Galaga::Update(CMPUT350::GameContext* context)
{
    if (mGameOverRequested)
    {
        mState = State::GameOver;
        mStateStartFrame = context->currentFrame;
        mGameOverRequested = false;
    }


    switch (mState)
    {
        case State::Waiting:
        {
            if (mCoinInserted && mStartRequested)
            {
                mScore = 0;
                mLives = 3;
                mLevel = 1;

                mWave = 0;
                mEnemiesAlive = 0;

                mEnemies.clear();

                SpawnPlayer(context);

                mState = State::Startup;
                mStateStartFrame = context->currentFrame;
            }

            break;
        }


        case State::Startup:
        {
            // Wait a short time while showing level information
            if (context->currentFrame - mStateStartFrame > 90)
            {
                mState = State::Waves;
                mLastWaveFrame = context->currentFrame;
            }

            break;
        }


        case State::Waves:
        {
            // Spawn one wave every 90 frames
            if (mWave < 5 &&
                context->currentFrame - mLastWaveFrame > 90)
            {
                SpawnWave(context);

                mWave++;
                mLastWaveFrame = context->currentFrame;
            }

            // After all 5 waves have entered,
            // move into normal gameplay
            if (mWave >= 5 && AllEnemiesInFormation())
            {
                mState = State::Gameplay;
            }

            if (mPlayerNeedsRespawn &&
                AllEnemiesInFormation())
            {
                SpawnPlayer(context);
                mPlayerNeedsRespawn = false;
            }

            break;
        }


        case State::Gameplay:
        {
            // Level completed
            if (mEnemiesAlive <= 0)
            {
                mLevel++;

                mWave = 0;
                mEnemies.clear();

                mState = State::Startup;
                mStateStartFrame = context->currentFrame;

                break;
            }


            // Respawn player when enemies are back in formation
            if (mPlayerNeedsRespawn &&
                AllEnemiesInFormation())
            {
                SpawnPlayer(context);
                mPlayerNeedsRespawn = false;
            }


            // Every few seconds, choose one enemy to attack
            if (context->currentFrame % 180 == 0)
            {
                auto player = mPlayer.lock();

                if (player)
                {
                    for (auto& weakEnemy : mEnemies)
                    {
                        auto enemy = weakEnemy.lock();

                        if (enemy &&
                            enemy->IsAlive() &&
                            enemy->Attack(
                                context,
                                player->GetLocation()
                            ))
                        {
                            break;
                        }
                    }
                }
            }

            break;
        }


        case State::GameOver:
        {
            // Show game over for a few seconds
            if (context->currentFrame - mStateStartFrame > 180)
            {
                mState = State::Waiting;

                mCoinInserted = false;
                mStartRequested = false;
                mPlayerNeedsRespawn = false;

                mEnemies.clear();
            }

            break;
        }
    }
}





void Galaga::SpawnPlayer(CMPUT350::GameContext* context)
{
    auto player = std::make_shared<Player>(
        CMPUT350::Point2D(
            mWidth / 2.0f,
            mHeight - 100.0f
        )
    );
    

    mPlayer = player;

    context->mEngineView->AddGameObject(player);
}




void Galaga::SpawnWave(CMPUT350::GameContext* context)
{
    for (int i = 0; i < 8; i++)
    {
        float x = 100.0f + i * 75.0f;
        float y = 120.0f + mWave * 55.0f;

        CMPUT350::Point2D formationLocation(x, y);


        std::vector<CMPUT350::Point2D> path;

        path.push_back(
            CMPUT350::Point2D(-50, 100)
        );

        path.push_back(
            CMPUT350::Point2D(100, 0)
        );

        path.push_back(
            CMPUT350::Point2D(
                x - 100,
                y
            )
        );

        path.push_back(formationLocation);


        std::shared_ptr<Enemy> enemy;

        int enemyType = (mWave + i) % 3;


        if (enemyType == 0)
        {
            enemy = std::make_shared<Enemy1>(
                formationLocation,
                path
            );
        }
        else if (enemyType == 1)
        {
            enemy = std::make_shared<Enemy2>(
                formationLocation,
                path
            );
        }
        else
        {
            enemy = std::make_shared<Enemy3>(
                formationLocation,
                path
            );
        }



        context->mEngineView->AddGameObject(enemy);

        mEnemies.push_back(enemy);

        mEnemiesAlive++;
    }
}





bool Galaga::AllEnemiesInFormation() const
{
    bool foundEnemy = false;

    for (const auto& weakEnemy : mEnemies)
    {
        auto enemy = weakEnemy.lock();

        if (enemy && enemy->IsAlive())
        {
            foundEnemy = true;

            if (!enemy->IsInFormation())
            {
                return false;
            }
        }
    }

    return foundEnemy;
}







void Galaga::RenderUI(CMPUT350::GameContext* context)
{
    context->GUIContext->DrawText(
        "SCORE: " + std::to_string(mScore),
        24,
        CMPUT350::Point2D(20, 20),
        CMPUT350::Colors::white
    );


    context->GUIContext->DrawText(
        "LIVES: " + std::to_string(mLives),
        24,
        CMPUT350::Point2D(20, 50),
        CMPUT350::Colors::white
    );


    context->GUIContext->DrawText(
        "LEVEL: " + std::to_string(mLevel),
        24,
        CMPUT350::Point2D(20, 80),
        CMPUT350::Colors::white
    );


    if (mState == State::Waiting)
    {
        context->GUIContext->DrawCenteredText(
            "PRESS 5 FOR COIN - 1 TO START",
            26,
            CMPUT350::Point2D(
                mWidth / 2.0f,
                mHeight / 2.0f
            ),
            CMPUT350::Colors::white
        );
    }
    else if (mState == State::Startup)
    {
        context->GUIContext->DrawCenteredText(
            "LEVEL " + std::to_string(mLevel),
            30,
            CMPUT350::Point2D(
                mWidth / 2.0f,
                mHeight / 2.0f
            ),
            CMPUT350::Colors::white
        );
    }
    else if (mState == State::GameOver)
    {
        context->GUIContext->DrawCenteredText(
            "GAME OVER",
            40,
            CMPUT350::Point2D(
                mWidth / 2.0f,
                mHeight / 2.0f
            ),
            CMPUT350::Colors::red
        );
    }
}