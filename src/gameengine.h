#pragma once

#include <QObject>
#include <QVariantList>
#include <QtQml/qqmlregistration.h>

#include "player.h"

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
    Q_PROPERTY(qreal wind READ wind NOTIFY windChanged)
    Q_PROPERTY(QVariantList terrainHeights READ terrainHeights NOTIFY terrainChanged)

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
    QVariantList terrainHeights() const;

    Q_INVOKABLE void startGame(GameMode mode);
    Q_INVOKABLE void returnToMenu();
    Q_INVOKABLE void fireProjectile();
    Q_INVOKABLE void resolveShot(bool directHit);
    Q_INVOKABLE qreal terrainHeightAt(qreal x) const;

signals:
    void phaseChanged();
    void gameModeChanged();
    void currentPlayerChanged();
    void winnerChanged();
    void windChanged();
    void terrainChanged();
    void projectileFired();
    void gameOver(Player *winner);

private:
    void setPhase(Phase phase);
    void advanceTurn();
    void randomizeWind();
    void generateTerrain();
    void positionPlayers();

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
};
