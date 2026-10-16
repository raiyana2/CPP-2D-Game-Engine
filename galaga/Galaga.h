#ifndef GALAGA_H
#define GALAGA_H

#include <memory>
#include <string>
#include <vector>

#include "GameObject.h"
#include "MathUtil.h"

class Enemy;
class Player;

namespace CMPUT350
{
    class EngineView;
    class NotificationManager;
}

class Galaga : public CMPUT350::GameObject
{
public:
    Galaga(int width, int height);

    void Initialize(CMPUT350::GameContext* context) override;
    void Update(CMPUT350::GameContext* context) override;
    void RenderUI(CMPUT350::GameContext* context) override;
    void ReceiveNotification(const std::string& message) override;

private:
    enum class State
    {
        Waiting,
        Startup,
        Waves,
        Gameplay,
        GameOver
    };

    /** Kills this round's actors, including bullets, before dropping weak references. */
    void ClearRound();

    void SpawnPlayer(CMPUT350::GameContext* context);
    void SpawnWave(CMPUT350::GameContext* context);

    bool AllEnemiesInFormation() const;

    int mWidth;
    int mHeight;

    State mState;

    int mScore;
    int mLives;
    int mLevel;

    int mWave;
    int mEnemiesAlive;

    int mStateStartFrame;
    int mLastWaveFrame;

    bool mCoinInserted;
    bool mStartRequested;
    bool mPlayerNeedsRespawn;
    bool mGameOverRequested;

    std::weak_ptr<Player> mPlayer;

    std::vector<std::weak_ptr<Enemy>> mEnemies;

    CMPUT350::EngineView* mEngineView;
    CMPUT350::NotificationManager* mNotificationManager;
};

#endif