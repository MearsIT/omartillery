#include "gameengine.h"
#include "player.h"

#include <QJSEngine>
#include <QQmlEngine>
#include <QRandomGenerator>

GameEngine::GameEngine(QObject *parent)
    : QObject(parent),
      m_player1(new Player(QStringLiteral("Player 1"), this)),
      m_player2(new Player(QStringLiteral("Player 2"), this)),
      m_current(m_player1)
{
}

GameEngine *GameEngine::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
    Q_UNUSED(qmlEngine);
    Q_UNUSED(jsEngine);
    return new GameEngine();
}

GameEngine::Phase GameEngine::phase() const
{
    return m_phase;
}

GameEngine::GameMode GameEngine::gameMode() const
{
    return m_mode;
}

Player *GameEngine::player1() const
{
    return m_player1;
}

Player *GameEngine::player2() const
{
    return m_player2;
}

Player *GameEngine::currentPlayer() const
{
    return m_current;
}

Player *GameEngine::winner() const
{
    return m_winner;
}

qreal GameEngine::wind() const
{
    return m_wind;
}

QVariantList GameEngine::terrainHeights() const
{
    return m_terrainList;
}

void GameEngine::startGame(GameMode mode)
{
    if (m_mode != mode) {
        m_mode = mode;
        emit gameModeChanged();
    }

    m_player1->resetForNewGame();
    m_player2->resetForNewGame();
    m_player1->setFacing(1);
    m_player2->setFacing(-1);
    m_player2->setIsAI(mode == GameMode::VsAI);
    m_player2->setName(mode == GameMode::VsAI ? QStringLiteral("CPU")
                                              : QStringLiteral("Player 2"));

    generateTerrain();
    positionPlayers();

    m_winner = nullptr;
    emit winnerChanged();

    m_currentIndex = 0;
    m_current = m_player1;
    emit currentPlayerChanged();

    randomizeWind();
    setPhase(Phase::Player1Aim);
}

void GameEngine::returnToMenu()
{
    setPhase(Phase::Menu);
}

void GameEngine::fireProjectile()
{
    if (m_phase != Phase::Player1Aim && m_phase != Phase::Player2Aim
        && m_phase != Phase::AITurn)
        return;

    setPhase(m_currentIndex == 0 ? Phase::Player1Fire : Phase::Player2Fire);
    emit projectileFired();
}

void GameEngine::resolveShot(bool directHit)
{
    if (m_phase != Phase::Player1Fire && m_phase != Phase::Player2Fire)
        return;

    Player *shooter = m_current;
    Player *target = shooter == m_player1 ? m_player2 : m_player1;

    if (directHit) {
        target->setHealth(target->health() - HIT_DAMAGE);
        if (!target->alive()) {
            shooter->incrementScore();
            m_winner = shooter;
            emit winnerChanged();
            setPhase(Phase::GameOver);
            emit gameOver(shooter);
            return;
        }
    }

    advanceTurn();
}

qreal GameEngine::terrainHeightAt(qreal x) const
{
    if (m_terrain.isEmpty())
        return BOARD_HEIGHT;

    const qreal column = qBound<qreal>(0.0, x * TERRAIN_COLUMNS / BOARD_WIDTH,
                                       TERRAIN_COLUMNS - 1.0);
    const int i = int(column);
    const qreal fraction = column - i;
    const qreal a = m_terrain.at(i);
    const qreal b = m_terrain.at(qMin(i + 1, TERRAIN_COLUMNS - 1));
    return a + (b - a) * fraction;
}

void GameEngine::setPhase(Phase phase)
{
    if (m_phase == phase)
        return;
    m_phase = phase;
    emit phaseChanged();
}

void GameEngine::advanceTurn()
{
    m_currentIndex = 1 - m_currentIndex;
    m_current = m_currentIndex == 0 ? m_player1 : m_player2;
    emit currentPlayerChanged();

    randomizeWind();

    if (m_mode == GameMode::VsAI && m_currentIndex == 1)
        setPhase(Phase::AITurn);
    else
        setPhase(m_currentIndex == 0 ? Phase::Player1Aim : Phase::Player2Aim);
}

void GameEngine::randomizeWind()
{
    m_wind = QRandomGenerator::global()->bounded(2.0 * WIND_MAX) - WIND_MAX;
    emit windChanged();
}

void GameEngine::generateTerrain()
{
    m_terrain.clear();
    m_terrain.reserve(TERRAIN_COLUMNS);

    QRandomGenerator *rng = QRandomGenerator::global();
    qreal h = 380.0 + rng->bounded(100.0);
    for (int i = 0; i < TERRAIN_COLUMNS; ++i) {
        h += rng->bounded(48.0) - 24.0;
        h = qBound(340.0, h, 560.0);
        m_terrain.append(h);
    }

    const qreal columnWidth = qreal(BOARD_WIDTH) / TERRAIN_COLUMNS;
    const qreal pad1X = 0.14 * BOARD_WIDTH;
    const qreal pad2X = 0.86 * BOARD_WIDTH;
    const int pad1 = int(pad1X / columnWidth);
    const int pad2 = int(pad2X / columnWidth);
    for (int i = pad1 - 2; i <= pad1 + 2; ++i)
        m_terrain[i] = m_terrain.at(pad1);
    for (int i = pad2 - 2; i <= pad2 + 2; ++i)
        m_terrain[i] = m_terrain.at(pad2);

    m_terrainList.clear();
    m_terrainList.reserve(TERRAIN_COLUMNS);
    for (qreal height : std::as_const(m_terrain))
        m_terrainList.append(height);
    emit terrainChanged();
}

void GameEngine::positionPlayers()
{
    const qreal x1 = 0.14 * BOARD_WIDTH;
    const qreal x2 = 0.86 * BOARD_WIDTH;
    m_player1->setPosition(QPointF(x1, terrainHeightAt(x1)));
    m_player2->setPosition(QPointF(x2, terrainHeightAt(x2)));
}
