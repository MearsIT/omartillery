#pragma once

#include <QObject>
#include <QVariantList>
#include <QtQml/qqmlregistration.h>

#include "player.h"
#include "physicsengine.h"
#include "aiopponent.h"

class QQmlEngine;
class QJSEngine;

class GameEngine : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(Phase phase READ phase NOTIFY phaseChanged)
    Q_PROPERTY(GameMode gameMode READ gameMode NOTIFY gameModeChanged)
    Q_PROPERTY(Player *player1 READ player1 CONSTANT)
    Q_PROPERTY(Player *player2 READ player2 CONSTANT)
    Q_PROPERTY(Player *currentPlayer READ currentPlayer NOTIFY currentPlayerChanged)
    Q_PROPERTY(Player *winner READ winner NOTIFY winnerChanged)
    Q_PROPERTY(qreal wind READ wind WRITE setWind NOTIFY windChanged)
    Q_PROPERTY(QVariantList terrainHeights READ terrainHeights NOTIFY terrainChanged)
    Q_PROPERTY(bool projectileInFlight READ projectileInFlight NOTIFY projectileInFlightChanged)
    Q_PROPERTY(qreal projectileX READ projectileX NOTIFY projectilePositionChanged)
    Q_PROPERTY(qreal projectileY READ projectileY NOTIFY projectilePositionChanged)
    Q_PROPERTY(qreal projectileTime READ projectileTime NOTIFY projectilePositionChanged)

public:
    enum class Phase {
        Menu,
        Player1Aim,
        Player1Fire,
        Player2Aim,
        Player2Fire,
        AITurn,
        GameOver,
    };
    Q_ENUM(Phase)

    enum class GameMode {
        TwoPlayer,
        VsAI,
    };
    Q_ENUM(GameMode)

    static constexpr int BOARD_WIDTH = 800;
    static constexpr int BOARD_HEIGHT = 600;
    static constexpr int TERRAIN_COLUMNS = 100;
    static constexpr int MAX_HEALTH = 100;
    static constexpr int HIT_DAMAGE = 50;
    static constexpr qreal WIND_MAX = 60.0;

    explicit GameEngine(QObject *parent = nullptr);

    static GameEngine *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    Phase phase() const;
    GameMode gameMode() const;
    Player *player1() const;
    Player *player2() const;
    Player *currentPlayer() const;
    Player *winner() const;
    qreal wind() const;
    void setWind(qreal wind);
    QVariantList terrainHeights() const;
    bool projectileInFlight() const;
    qreal projectileX() const;
    qreal projectileY() const;
    qreal projectileTime() const;

    Q_INVOKABLE void startGame(GameMode mode);
    Q_INVOKABLE void returnToMenu();
    Q_INVOKABLE void fireProjectile();
    Q_INVOKABLE void resolveShot(bool directHit);
    Q_INVOKABLE void updateFlight(qreal elapsedSeconds);
    Q_INVOKABLE void explosionFinished();
    Q_INVOKABLE void aiTakeShot();
    Q_INVOKABLE qreal terrainHeightAt(qreal x) const;

signals:
    void phaseChanged();
    void gameModeChanged();
    void currentPlayerChanged();
    void winnerChanged();
    void windChanged();
    void terrainChanged();
    void projectileFired();
    void projectileInFlightChanged();
    void projectilePositionChanged();
    void explosionAt(qreal x, qreal y);
    void flightFinished();
    void gameOver(Player *winner);

private:
    void setPhase(Phase phase);
    void advanceTurn();
    void randomizeWind();
    void generateTerrain();
    void positionPlayers();
    void impactAt(const QPointF &point, bool directHit);
    void finishMiss();

    Phase m_phase = Phase::Menu;
    GameMode m_mode = GameMode::TwoPlayer;
    Player *m_player1 = nullptr;
    Player *m_player2 = nullptr;
    Player *m_current = nullptr;
    Player *m_winner = nullptr;
    int m_currentIndex = 0;
    qreal m_wind = 0.0;
    QVector<qreal> m_terrain;
    QVariantList m_terrainList;

    PhysicsEngine m_physics;
    AIOpponent m_ai;
    bool m_projectileInFlight = false;
    bool m_pendingDirectHit = false;
    QPointF m_projectilePos;
    QPointF m_launchOrigin;
    qreal m_launchAngle = 45.0;
    qreal m_launchPower = 50.0;
    int m_launchFacing = 1;
    qreal m_launchWind = 0.0;
    qreal m_flightTime = 0.0;
    qreal m_prevFlightTime = 0.0;
};
