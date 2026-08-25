#include "player.h"
#include "physicsengine.h"

Player::Player(const QString &name, QObject *parent)
    : QObject(parent), m_name(name)
{
}

QString Player::name() const
{
    return m_name;
}

void Player::setName(const QString &name)
{
    if (m_name == name)
        return;
    m_name = name;
    emit nameChanged();
}

int Player::health() const
{
    return m_health;
}

void Player::setHealth(int health)
{
    const int clamped = qBound(0, health, MAX_HEALTH);
    if (m_health == clamped)
        return;
    m_health = clamped;
    emit healthChanged();
}

bool Player::alive() const
{
    return m_health > 0;
}

int Player::score() const
{
    return m_score;
}

void Player::incrementScore()
{
    ++m_score;
    emit scoreChanged();
}

qreal Player::angle() const
{
    return m_angle;
}

void Player::setAngle(qreal angle)
{
    const qreal clamped = PhysicsEngine::clampAngle(angle);
    if (qFuzzyIsNull(m_angle - clamped))
        return;
    m_angle = clamped;
    emit angleChanged();
}

qreal Player::power() const
{
    return m_power;
}

void Player::setPower(qreal power)
{
    const qreal clamped = PhysicsEngine::clampPower(power);
    if (qFuzzyIsNull(m_power - clamped))
        return;
    m_power = clamped;
    emit powerChanged();
}

qreal Player::x() const
{
    return m_position.x();
}

qreal Player::y() const
{
    return m_position.y();
}

void Player::setX(qreal x)
{
    if (qFuzzyIsNull(m_position.x() - x))
        return;
    m_position.setX(x);
    emit positionChanged();
}

void Player::setY(qreal y)
{
    if (qFuzzyIsNull(m_position.y() - y))
        return;
    m_position.setY(y);
    emit positionChanged();
}

void Player::setPosition(const QPointF &position)
{
    if (m_position == position)
        return;
    m_position = position;
    emit positionChanged();
}

int Player::facing() const
{
    return m_facing;
}

void Player::setFacing(int facing)
{
    const int clamped = facing < 0 ? -1 : 1;
    if (m_facing == clamped)
        return;
    m_facing = clamped;
    emit facingChanged();
}

bool Player::isAI() const
{
    return m_isAI;
}

void Player::setIsAI(bool isAI)
{
    if (m_isAI == isAI)
        return;
    m_isAI = isAI;
    emit isAIChanged();
}

void Player::resetForNewGame()
{
    setHealth(MAX_HEALTH);
    setAngle(45.0);
    setPower(50.0);
}
