#pragma once

#include <QObject>
#include <QPointF>
#include <QList>
#include <QtQml/qqmlregistration.h>

class PhysicsEngine : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    static constexpr qreal GRAVITY = 300.0;
    static constexpr qreal POWER_SCALE = 4.0;
    static constexpr qreal WIND_MAX = 60.0;

    struct SimulationResult {
        qreal landingX = 0.0;
        qreal flightTime = 0.0;
        bool hitGround = false;
    };

    explicit PhysicsEngine(QObject *parent = nullptr);

    qreal launchSpeed(qreal power) const;

    QPointF positionAt(const QPointF &origin, qreal angleDeg, qreal power,
                       int facing, qreal wind, qreal t) const;
    QPointF velocityAt(const QPointF &origin, qreal angleDeg, qreal power,
                       int facing, qreal wind, qreal t) const;

    Q_INVOKABLE QList<QPointF> calculateTrajectory(const QPointF &origin,
                                                   qreal angleDeg, qreal power,
                                                   int facing, qreal wind) const;

    SimulationResult simulate(const QPointF &origin, qreal angleDeg, qreal power,
                              int facing, qreal wind, qreal groundY) const;

    Q_INVOKABLE bool pointIntersectsBox(const QPointF &point,
                                        const QPointF &boxCenter,
                                        qreal boxWidth, qreal boxHeight) const;

    static qreal clampAngle(qreal angleDeg);
    static qreal clampPower(qreal power);
};
