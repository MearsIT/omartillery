#include "aiopponent.h"
#include "physicsengine.h"

#include <QRandomGenerator>

AIOpponent::AIOpponent(QObject *parent)
    : QObject(parent)
{
}

QPointF AIOpponent::calculateShot(const QPointF &origin, const QPointF &target,
                                  int facing, qreal wind, qreal groundY) const
{
    const PhysicsEngine physics;

    qreal bestError = std::numeric_limits<qreal>::max();
    qreal bestAngle = 45.0;
    qreal bestPower = 50.0;

    for (int angle = 20; angle <= 80; angle += 2) {
        for (int power = 10; power <= 100; power += 2) {
            const auto result = physics.simulate(origin, angle, power, facing,
                                                 wind, groundY);
            if (!result.hitGround)
                continue;
            const qreal error = qAbs(result.landingX - target.x());
            if (error < bestError) {
                bestError = error;
                bestAngle = angle;
                bestPower = power;
            }
        }
    }

    return QPointF(bestAngle, bestPower);
}

QPointF AIOpponent::calculateShotWithError(const QPointF &origin,
                                           const QPointF &target, int facing,
                                           qreal wind, qreal groundY) const
{
    const QPointF shot = calculateShot(origin, target, facing, wind, groundY);
    QRandomGenerator *rng = QRandomGenerator::global();

    const qreal angleError = rng->bounded(2.0 * MAX_ANGLE_ERROR)
            - MAX_ANGLE_ERROR;
    const qreal powerError = rng->bounded(2.0 * MAX_POWER_ERROR)
            - MAX_POWER_ERROR;

    return QPointF(PhysicsEngine::clampAngle(shot.x() + angleError),
                   PhysicsEngine::clampPower(shot.y() * (1.0 + powerError)));
}
