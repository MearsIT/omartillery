#include "physicsengine.h"

#include <QtMath>

PhysicsEngine::PhysicsEngine(QObject *parent)
    : QObject(parent)
{
}

qreal PhysicsEngine::launchSpeed(qreal power) const
{
    return clampPower(power) * POWER_SCALE;
}

QPointF PhysicsEngine::positionAt(const QPointF &origin, qreal angleDeg,
                                  qreal power, int facing, qreal wind,
                                  qreal t) const
{
    const qreal angle = qDegreesToRadians(clampAngle(angleDeg));
    const qreal speed = launchSpeed(power);
    const qreal dir = facing < 0 ? -1.0 : 1.0;
    const qreal vx0 = dir * speed * qCos(angle);
    const qreal vy0 = speed * qSin(angle);
    const qreal w = qBound(-WIND_MAX, wind, WIND_MAX);

    const qreal x = origin.x() + vx0 * t + 0.5 * w * t * t;
    const qreal y = origin.y() - vy0 * t + 0.5 * GRAVITY * t * t;
    return QPointF(x, y);
}

QPointF PhysicsEngine::velocityAt(const QPointF &origin, qreal angleDeg,
                                  qreal power, int facing, qreal wind,
                                  qreal t) const
{
    Q_UNUSED(origin);
    const qreal angle = qDegreesToRadians(clampAngle(angleDeg));
    const qreal speed = launchSpeed(power);
    const qreal dir = facing < 0 ? -1.0 : 1.0;
    const qreal w = qBound(-WIND_MAX, wind, WIND_MAX);

    const qreal vx = dir * speed * qCos(angle) + w * t;
    const qreal vy = -(speed * qSin(angle)) + GRAVITY * t;
    return QPointF(vx, vy);
}

QList<QPointF> PhysicsEngine::calculateTrajectory(const QPointF &origin,
                                                  qreal angleDeg, qreal power,
                                                  int facing, qreal wind) const
{
    QList<QPointF> points;
    const qreal dt = 1.0 / 60.0;
    const qreal maxTime = 10.0;
    for (qreal t = 0.0; t <= maxTime; t += dt) {
        const QPointF p = positionAt(origin, angleDeg, power, facing, wind, t);
        points.append(p);
        if (p.y() > 700.0 || p.x() < -100.0 || p.x() > 900.0)
            break;
    }
    return points;
}

PhysicsEngine::SimulationResult PhysicsEngine::simulate(const QPointF &origin,
                                                        qreal angleDeg,
                                                        qreal power, int facing,
                                                        qreal wind,
                                                        qreal groundY) const
{
    SimulationResult result;
    const qreal angle = qDegreesToRadians(clampAngle(angleDeg));
    const qreal speed = launchSpeed(power);
    const qreal dir = facing < 0 ? -1.0 : 1.0;
    const qreal vx0 = dir * speed * qCos(angle);
    const qreal vy0 = speed * qSin(angle);
    const qreal w = qBound(-WIND_MAX, wind, WIND_MAX);

    const qreal discriminant = vy0 * vy0 + 2.0 * GRAVITY * (groundY - origin.y());
    if (discriminant < 0.0)
        return result;

    const qreal t = (vy0 + qSqrt(discriminant)) / GRAVITY;
    result.hitGround = true;
    result.flightTime = t;
    result.landingX = origin.x() + vx0 * t + 0.5 * w * t * t;
    return result;
}

bool PhysicsEngine::pointIntersectsBox(const QPointF &point,
                                       const QPointF &boxCenter,
                                       qreal boxWidth, qreal boxHeight) const
{
    return qAbs(point.x() - boxCenter.x()) * 2.0 <= boxWidth
        && qAbs(point.y() - boxCenter.y()) * 2.0 <= boxHeight;
}

qreal PhysicsEngine::clampAngle(qreal angleDeg)
{
    return qBound(0.0, angleDeg, 90.0);
}

qreal PhysicsEngine::clampPower(qreal power)
{
    return qBound(0.0, power, 100.0);
}
