#pragma once

#include <QObject>
#include <QPointF>
#include <QtQml/qqmlregistration.h>

class AIOpponent : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    static constexpr qreal MAX_ANGLE_ERROR = 5.0;
    static constexpr qreal MAX_POWER_ERROR = 0.10;

    explicit AIOpponent(QObject *parent = nullptr);

    Q_INVOKABLE QPointF calculateShot(const QPointF &origin,
                                      const QPointF &target, int facing,
                                      qreal wind, qreal groundY) const;

    QPointF calculateShotWithError(const QPointF &origin, const QPointF &target,
                                   int facing, qreal wind, qreal groundY) const;
};
