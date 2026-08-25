#include <QtTest>

#include "physicsengine.h"

#include <QtMath>

class TestPhysics : public QObject
{
    Q_OBJECT

private slots:
    void launchSpeedScalesWithPower();
    void parabolicArcNoWind();
    void symmetricArc();
    void levelRangeNoWind();
    void windDeflectsTrajectory();
    void facingMirrorsTrajectory();
    void zeroPowerDropsStraight();
    void nearVerticalShot();
    void simulateMatchesAnalytic();
    void trajectoryListTerminates();
    void invalidInputsClamped();
    void boxCollisionBoundaries();
    void deterministic();

private:
    PhysicsEngine physics;
    static bool near(qreal a, qreal b, qreal epsilon = 0.01);
};

bool TestPhysics::near(qreal a, qreal b, qreal epsilon)
{
    return qAbs(a - b) < epsilon;
}

void TestPhysics::launchSpeedScalesWithPower()
{
    QCOMPARE(physics.launchSpeed(0.0), 0.0);
    QCOMPARE(physics.launchSpeed(50.0), 200.0);
    QCOMPARE(physics.launchSpeed(100.0), 400.0);
    QCOMPARE(physics.launchSpeed(150.0), 400.0);
}

void TestPhysics::parabolicArcNoWind()
{
    const QPointF origin(400.0, 400.0);
    const qreal tApex = 141.4213562 / PhysicsEngine::GRAVITY;

    const QPointF apex = physics.positionAt(origin, 45.0, 50.0, 1, 0.0, tApex);
    QVERIFY(near(apex.y(), 400.0 - 20000.0 / 600.0, 0.05));
    QVERIFY(near(apex.x(), 466.6667, 0.05));

    const QPointF start = physics.positionAt(origin, 45.0, 50.0, 1, 0.0, 0.0);
    QCOMPARE(start, origin);
}

void TestPhysics::symmetricArc()
{
    const QPointF origin(400.0, 400.0);
    const qreal tApex = 141.4213562 / PhysicsEngine::GRAVITY;
    const QPointF before = physics.positionAt(origin, 45.0, 50.0, 1, 0.0,
                                              tApex - 0.1);
    const QPointF after = physics.positionAt(origin, 45.0, 50.0, 1, 0.0,
                                             tApex + 0.1);
    QVERIFY(near(before.y(), after.y(), 0.05));
    QVERIFY(after.x() - before.x() > 0.0);
}

void TestPhysics::levelRangeNoWind()
{
    const QPointF origin(400.0, 400.0);
    const PhysicsEngine::SimulationResult r = physics.simulate(origin, 45.0,
                                                               50.0, 1, 0.0,
                                                               400.0);
    QVERIFY(r.hitGround);
    QVERIFY(near(r.landingX, 533.3333, 0.05));
    QVERIFY(near(r.flightTime, 0.9428090, 0.0005));
}

void TestPhysics::windDeflectsTrajectory()
{
    const QPointF origin(400.0, 400.0);

    const PhysicsEngine::SimulationResult calm = physics.simulate(origin, 45.0,
                                                                  50.0, 1, 0.0,
                                                                  400.0);
    const PhysicsEngine::SimulationResult tailwind = physics.simulate(origin,
                                                                      45.0, 50.0,
                                                                      1, 60.0,
                                                                      400.0);
    const PhysicsEngine::SimulationResult headwind = physics.simulate(origin,
                                                                      45.0, 50.0,
                                                                      1, -60.0,
                                                                      400.0);

    QVERIFY(tailwind.landingX > calm.landingX);
    QVERIFY(headwind.landingX < calm.landingX);
    QVERIFY(near(tailwind.landingX, 560.0, 0.05));
    QVERIFY(near(headwind.landingX, 506.6667, 0.05));
}

void TestPhysics::facingMirrorsTrajectory()
{
    const QPointF origin(400.0, 400.0);
    const PhysicsEngine::SimulationResult right = physics.simulate(origin, 45.0,
                                                                   50.0, 1, 0.0,
                                                                   400.0);
    const PhysicsEngine::SimulationResult left = physics.simulate(origin, 45.0,
                                                                  50.0, -1, 0.0,
                                                                  400.0);
    QVERIFY(near(right.landingX - 400.0, 400.0 - left.landingX, 0.05));

    const QPointF pRight = physics.positionAt(origin, 30.0, 80.0, 1, 0.0, 0.3);
    const QPointF pLeft = physics.positionAt(origin, 30.0, 80.0, -1, 0.0, 0.3);
    QVERIFY(near(pRight.x() - 400.0, 400.0 - pLeft.x(), 0.05));
    QVERIFY(near(pRight.y(), pLeft.y(), 0.05));
}

void TestPhysics::zeroPowerDropsStraight()
{
    const QPointF origin(400.0, 400.0);
    const PhysicsEngine::SimulationResult r = physics.simulate(origin, 45.0,
                                                               0.0, 1, 0.0,
                                                               500.0);
    QVERIFY(r.hitGround);
    QVERIFY(near(r.landingX, 400.0, 0.05));
    QVERIFY(near(r.flightTime, 0.8164966, 0.0005));

    const PhysicsEngine::SimulationResult drifted = physics.simulate(origin, 45.0,
                                                                     0.0, 1,
                                                                     60.0,
                                                                     500.0);
    QVERIFY(near(drifted.landingX, 420.0, 0.05));
}

void TestPhysics::nearVerticalShot()
{
    const QPointF origin(400.0, 400.0);
    const PhysicsEngine::SimulationResult r = physics.simulate(origin, 90.0,
                                                               100.0, 1, 0.0,
                                                               400.0);
    QVERIFY(r.hitGround);
    QVERIFY(near(r.landingX, 400.0, 0.05));
    QVERIFY(near(r.flightTime, 2.6666667, 0.001));

    const PhysicsEngine::SimulationResult drifted = physics.simulate(origin, 90.0,
                                                                     100.0, 1,
                                                                     60.0,
                                                                     400.0);
    QVERIFY(drifted.landingX > 500.0);
}

void TestPhysics::simulateMatchesAnalytic()
{
    const QPointF origin(250.0, 380.0);
    const PhysicsEngine::SimulationResult r = physics.simulate(origin, 60.0,
                                                               70.0, 1, -30.0,
                                                               520.0);
    QVERIFY(r.hitGround);
    const QPointF landing = physics.positionAt(origin, 60.0, 70.0, 1, -30.0,
                                               r.flightTime);
    QVERIFY(near(landing.x(), r.landingX, 0.05));
    QVERIFY(near(landing.y(), 520.0, 0.05));
}

void TestPhysics::trajectoryListTerminates()
{
    const QPointF origin(400.0, 400.0);
    const QList<QPointF> path = physics.calculateTrajectory(origin, 45.0, 50.0,
                                                            1, 0.0);
    QVERIFY(path.size() >= 2);
    QCOMPARE(path.first(), origin);
    const QPointF &last = path.last();
    QVERIFY(last.y() > 700.0 || last.x() < -100.0 || last.x() > 900.0);
    for (const QPointF &p : path) {
        QVERIFY(qIsFinite(p.x()));
        QVERIFY(qIsFinite(p.y()));
    }
}

void TestPhysics::invalidInputsClamped()
{
    const QPointF origin(400.0, 400.0);
    const QPointF clampedAngle = physics.positionAt(origin, 200.0, 50.0, 1,
                                                    0.0, 0.5);
    const QPointF maxAngle = physics.positionAt(origin, 90.0, 50.0, 1, 0.0,
                                                0.5);
    QCOMPARE(clampedAngle, maxAngle);

    const QPointF clampedPower = physics.positionAt(origin, 45.0, -50.0, 1,
                                                    0.0, 0.5);
    const QPointF zeroPower = physics.positionAt(origin, 45.0, 0.0, 1, 0.0,
                                                 0.5);
    QCOMPARE(clampedPower, zeroPower);

    const PhysicsEngine::SimulationResult hugeWind = physics.simulate(origin,
                                                                      45.0, 50.0,
                                                                      1, 999.0,
                                                                      400.0);
    const PhysicsEngine::SimulationResult maxWind = physics.simulate(origin,
                                                                     45.0, 50.0,
                                                                     1, 60.0,
                                                                     400.0);
    QCOMPARE(hugeWind.landingX, maxWind.landingX);
}

void TestPhysics::boxCollisionBoundaries()
{
    const QPointF center(400.0, 400.0);
    QVERIFY(physics.pointIntersectsBox(QPointF(400.0, 400.0), center, 16.0,
                                       16.0));
    QVERIFY(physics.pointIntersectsBox(QPointF(407.9, 400.0), center, 16.0,
                                       16.0));
    QVERIFY(!physics.pointIntersectsBox(QPointF(408.1, 400.0), center, 16.0,
                                        16.0));
    QVERIFY(physics.pointIntersectsBox(QPointF(400.0, 408.0), center, 16.0,
                                       16.0));
    QVERIFY(!physics.pointIntersectsBox(QPointF(400.0, 408.1), center, 16.0,
                                        16.0));
    QVERIFY(!physics.pointIntersectsBox(QPointF(380.0, 400.0), center, 16.0,
                                        16.0));
}

void TestPhysics::deterministic()
{
    const QPointF origin(300.0, 450.0);
    physics.positionAt(origin, 55.0, 60.0, -1, 40.0, 0.25);
    const QPointF a = physics.positionAt(origin, 55.0, 60.0, -1, 40.0, 0.5);
    physics.positionAt(origin, 10.0, 90.0, 1, -20.0, 0.75);
    const QPointF b = physics.positionAt(origin, 55.0, 60.0, -1, 40.0, 0.5);
    QCOMPARE(a, b);
}

QTEST_MAIN(TestPhysics)
#include "test_physics.moc"
