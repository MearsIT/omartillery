#pragma once

#include "gameengine.h"
#include "physicsengine.h"
#include "player.h"

#include <QQmlEngine>
#include <QtMath>

namespace TestUtils {

inline GameEngine *engineSingleton(QQmlEngine &engine)
{
    const int typeId = qmlTypeId("ArtilleryDuel", 1, 0, "GameEngine");
    return typeId >= 0 ? engine.singletonInstance<GameEngine *>(typeId) : nullptr;
}

inline QPointF muzzlePos(const Player *shooter, qreal angleDeg)
{
    const qreal radians = qDegreesToRadians(angleDeg);
    const qreal facing = shooter->facing() < 0 ? -1.0 : 1.0;
    return QPointF(shooter->x() + facing * 12.0 * qCos(radians),
                   shooter->y() - GameEngine::TANK_HALF_HEIGHT
                       - 12.0 * qSin(radians));
}

inline void stepFlight(GameEngine &engine, qreal dt, int maxSteps)
{
    for (int i = 0; i < maxSteps && engine.projectileInFlight(); ++i)
        engine.updateFlight(engine.projectileTime() + dt);
}

inline void settleFlight(GameEngine &engine)
{
    stepFlight(engine, 1.0 / 120.0, 1440);
    if (engine.phase() == GameEngine::Phase::Player1Fire
        || engine.phase() == GameEngine::Phase::Player2Fire)
        engine.explosionFinished();
}

inline bool findClearShot(const GameEngine &engine, qreal &angle, qreal &power)
{
    const PhysicsEngine physics;
    const Player *shooter = engine.currentPlayer();
    const Player *target = shooter == engine.player1() ? engine.player2()
                                                       : engine.player1();
    const int facing = shooter->facing();
    const qreal wind = engine.wind();
    const QPointF boxCenter(target->x(),
                            target->y() - GameEngine::TANK_HALF_HEIGHT);

    for (int a = 80; a >= 15; a -= 5) {
        const QPointF origin = muzzlePos(shooter, a);
        for (int p = 15; p <= 100; ++p) {
            const auto sim = physics.simulate(origin, a, p, facing, wind,
                                              GameEngine::BOARD_HEIGHT + 60.0);
            const qreal tEnd = qMin(sim.hitGround ? sim.flightTime : 4.0, 4.0);
            bool terrainFirst = false;
            bool boxHit = false;
            for (qreal t = 0.0; t <= tEnd && !terrainFirst && !boxHit;
                 t += 1.0 / 480.0) {
                const QPointF q = physics.positionAt(origin, a, p, facing, wind, t);
                if (physics.pointIntersectsBox(q, boxCenter, 12.0, 12.0))
                    boxHit = true;
                else if (q.y() >= engine.terrainHeightAt(q.x()) - 2.0)
                    terrainFirst = true;
            }
            if (boxHit && !terrainFirst) {
                angle = a;
                power = p;
                return true;
            }
        }
    }
    return false;
}

}
