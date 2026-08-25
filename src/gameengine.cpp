#include "gameengine.h"
#include "player.h"

#include <QJSEngine>
#include <QQmlEngine>
#include <QRandomGenerator>
#include <QtMath>

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

void GameEngine::setWind(qreal wind)
{
    const qreal clamped = qBound(-WIND_MAX, wind, WIND_MAX);
    if (qFuzzyIsNull(m_wind - clamped))
        return;
    m_wind = clamped;
    emit windChanged();
}

bool GameEngine::projectileInFlight() const
{
    return m_projectileInFlight;
}

qreal GameEngine::projectileX() const
{
    return m_projectilePos.x();
}

qreal GameEngine::projectileY() const
{
    return m_projectilePos.y();
}

qreal GameEngine::projectileTime() const
{
    return m_flightTime;
}

QVariantList GameEngine::terrainHeights() const
{
    QVariantList heights;
    heights.reserve(m_terrain.size());
    for (qreal height : std::as_const(m_terrain))
        heights.append(height);
    return heights;
}

void GameEngine::startGame(GameMode mode)
{
    if (m_mode != mode) {
        m_mode = mode;
        emit gameModeChanged();
    }

    m_projectileInFlight = false;
    emit projectileInFlightChanged();
    m_pendingDirectHit = false;

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

    const Player *shooter = m_current;
    const qreal angle = shooter->angle();
    const qreal radians = qDegreesToRadians(angle);
    const qreal dir = shooter->facing() < 0 ? -1.0 : 1.0;
    const QPointF center(shooter->x(), shooter->y() - TANK_HALF_HEIGHT);
    const QPointF muzzle(center.x() + dir * 12.0 * qCos(radians),
                         center.y() - 12.0 * qSin(radians));

    m_launchOrigin = muzzle;
    m_launchAngle = angle;
    m_launchPower = shooter->power();
    m_launchFacing = shooter->facing();
    m_launchWind = m_wind;
    m_flightTime = 0.0;
    m_prevFlightTime = 0.0;
    m_projectilePos = muzzle;
    m_pendingDirectHit = false;

    setPhase(m_current == m_player1 ? Phase::Player1Fire : Phase::Player2Fire);

    m_projectileInFlight = true;
    emit projectileInFlightChanged();
    emit projectilePositionChanged();
    emit projectileFired();
}

void GameEngine::updateFlight(qreal elapsedSeconds)
{
    if (!m_projectileInFlight)
        return;

    m_prevFlightTime = m_flightTime;
    m_flightTime = elapsedSeconds;
    const QPointF p = m_physics.positionAt(m_launchOrigin, m_launchAngle,
                                           m_launchPower, m_launchFacing,
                                           m_launchWind, elapsedSeconds);

    if (p.x() < -80.0 || p.x() > BOARD_WIDTH + 80.0
        || p.y() > BOARD_HEIGHT + 60.0 || elapsedSeconds > 10.0) {
        finishMiss();
        return;
    }

    Player *opponent = m_current == m_player1 ? m_player2 : m_player1;
    const QPointF boxCenter(opponent->x(), opponent->y() - TANK_HALF_HEIGHT);

    for (int step = 1; step <= 4; ++step) {
        const qreal t = m_prevFlightTime
                + (elapsedSeconds - m_prevFlightTime) * step / 4.0;
        const QPointF q = m_physics.positionAt(m_launchOrigin, m_launchAngle,
                                               m_launchPower, m_launchFacing,
                                               m_launchWind, t);
        if (m_physics.pointIntersectsBox(q, boxCenter, 20.0, 20.0)) {
            impactAt(q, true);
            return;
        }
        if (q.y() >= terrainHeightAt(q.x())) {
            impactAt(q, false);
            return;
        }
    }

    m_projectilePos = p;
    emit projectilePositionChanged();
}

void GameEngine::explosionFinished()
{
    if (m_phase != Phase::Player1Fire && m_phase != Phase::Player2Fire)
        return;
    const bool directHit = m_pendingDirectHit;
    m_pendingDirectHit = false;
    resolveShot(directHit);
    emit flightFinished();
}

void GameEngine::impactAt(const QPointF &point, bool directHit)
{
    m_projectileInFlight = false;
    emit projectileInFlightChanged();
    m_projectilePos = point;
    emit projectilePositionChanged();
    m_pendingDirectHit = directHit;
    emit explosionAt(point.x(), point.y());
}

void GameEngine::finishMiss()
{
    m_projectileInFlight = false;
    emit projectileInFlightChanged();
    m_pendingDirectHit = false;
    resolveShot(false);
    emit flightFinished();
}

void GameEngine::aiTakeShot()
{
    if (m_phase != Phase::AITurn || m_current == nullptr || !m_current->isAI())
        return;

    Player *shooter = m_current;
    Player *target = m_current == m_player1 ? m_player2 : m_player1;

    const QPointF origin(shooter->x(), shooter->y() - TANK_HALF_HEIGHT);
    const QPointF aimPoint(target->x(), target->y() - TANK_HALF_HEIGHT);
    const QPointF shot = m_ai.calculateShotWithError(origin, aimPoint,
                                                     shooter->facing(), m_wind,
                                                     terrainHeightAt(target->x()));

    shooter->setAngle(shot.x());
    shooter->setPower(shot.y());
    fireProjectile();
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
    m_current = m_current == m_player1 ? m_player2 : m_player1;
    emit currentPlayerChanged();

    randomizeWind();

    if (m_mode == GameMode::VsAI && m_current == m_player2)
        setPhase(Phase::AITurn);
    else
        setPhase(m_current == m_player1 ? Phase::Player1Aim : Phase::Player2Aim);
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
    const int pad1 = int(PLAYER1_START_X / columnWidth);
    const int pad2 = int(PLAYER2_START_X / columnWidth);
    for (int i = pad1 - 2; i <= pad1 + 2; ++i)
        m_terrain[i] = m_terrain.at(pad1);
    for (int i = pad2 - 2; i <= pad2 + 2; ++i)
        m_terrain[i] = m_terrain.at(pad2);

    emit terrainChanged();
}

void GameEngine::positionPlayers()
{
    m_player1->setPosition(QPointF(PLAYER1_START_X,
                                   terrainHeightAt(PLAYER1_START_X)));
    m_player2->setPosition(QPointF(PLAYER2_START_X,
                                   terrainHeightAt(PLAYER2_START_X)));
}
