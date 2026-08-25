#pragma once

#include <QObject>
#include <QPointF>
#include <QString>
#include <QtQml/qqmlregistration.h>

class Player : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Players are created and owned by GameEngine")

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(int health READ health WRITE setHealth NOTIFY healthChanged)
    Q_PROPERTY(bool alive READ alive NOTIFY healthChanged)
    Q_PROPERTY(int score READ score NOTIFY scoreChanged)
    Q_PROPERTY(qreal angle READ angle WRITE setAngle NOTIFY angleChanged)
    Q_PROPERTY(qreal power READ power WRITE setPower NOTIFY powerChanged)
    Q_PROPERTY(qreal x READ x WRITE setX NOTIFY positionChanged)
    Q_PROPERTY(qreal y READ y WRITE setY NOTIFY positionChanged)
    Q_PROPERTY(int facing READ facing WRITE setFacing NOTIFY facingChanged)
    Q_PROPERTY(bool isAI READ isAI WRITE setIsAI NOTIFY isAIChanged)

public:
    static constexpr int MAX_HEALTH = 100;

    explicit Player(const QString &name, QObject *parent = nullptr);

    QString name() const;
    void setName(const QString &name);

    int health() const;
    void setHealth(int health);
    bool alive() const;

    int score() const;
    void incrementScore();

    qreal angle() const;
    void setAngle(qreal angle);

    qreal power() const;
    void setPower(qreal power);

    qreal x() const;
    qreal y() const;
    void setX(qreal x);
    void setY(qreal y);
    void setPosition(const QPointF &position);

    int facing() const;
    void setFacing(int facing);

    bool isAI() const;
    void setIsAI(bool isAI);

    void resetForNewGame();

signals:
    void nameChanged();
    void healthChanged();
    void scoreChanged();
    void angleChanged();
    void powerChanged();
    void positionChanged();
    void facingChanged();
    void isAIChanged();

private:
    QString m_name;
    int m_health = MAX_HEALTH;
    int m_score = 0;
    qreal m_angle = 45.0;
    qreal m_power = 50.0;
    QPointF m_position;
    int m_facing = 1;
    bool m_isAI = false;
};
