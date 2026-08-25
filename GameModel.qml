pragma Singleton
pragma ModuleName: "omartillery"

import QtQuick

QtObject {
    id: root

    // Constants
    readonly property int boardWidth: 800
    readonly property int boardHeight: 600
    readonly property int terrainColumns: 100
    readonly property int hitDamage: 50
    readonly property real windMax: 60.0
    readonly property real tankHalfHeight: 8.0
    readonly property real player1StartX: 0.14 * boardWidth
    readonly property real player2StartX: 0.86 * boardWidth
    readonly property int maxHealth: 100

    // Physics constants
    readonly property real gravity: 300.0
    readonly property real powerScale: 4.0

    // AI constants
    readonly property real maxAngleError: 5.0
    readonly property real maxPowerError: 0.10

    // Game State
    property int phase: Phase.Menu
    property int gameMode: GameMode.TwoPlayer
    property var player1: createPlayer("Player 1", 1)
    property var player2: createPlayer("Player 2", -1)
    property var currentPlayer: player1
    property var winner: null
    property real wind: 0.0
    property var terrain: []
    property bool projectileInFlight: false
    property real projectileX: 0
    property real projectileY: 0
    property real projectileTime: 0

    // Internal state
    property bool pendingDirectHit: false
    property point projectilePos: Qt.point(0, 0)
    property point launchOrigin: Qt.point(0, 0)
    property real launchAngle: 45.0
    property real launchPower: 50.0
    property int launchFacing: 1
    property real launchWind: 0.0
    property real flightTime: 0.0
    property real prevFlightTime: 0.0

    // Enums
    enum Phase {
        Menu,
        Player1Aim,
        Player1Fire,
        Player2Aim,
        Player2Fire,
        AITurn,
        GameOver
    }

    enum GameMode {
        TwoPlayer,
        VsAI
    }

    // Signals
    signal phaseChanged()
    signal gameModeChanged()
    signal currentPlayerChanged()
    signal winnerChanged()
    signal windChanged()
    signal terrainChanged()
    signal projectileFired()
    signal projectileInFlightChanged()
    signal projectilePositionChanged()
    signal explosionAt(real x, real y)
    signal flightFinished()
    signal gameOver(var winner)

    // Player factory
    function createPlayer(name, facing) {
        return {
            name: name,
            health: maxHealth,
            alive: true,
            score: 0,
            angle: 45.0,
            power: 50.0,
            x: 0,
            y: 0,
            facing: facing,
            isAI: false
        }
    }

    // Physics functions
    function clampAngle(angle) {
        return Math.max(0, Math.min(90, angle))
    }

    function clampPower(power) {
        return Math.max(0, Math.min(100, power))
    }

    function launchSpeed(power) {
        return clampPower(power) * powerScale
    }

    function positionAt(origin, angleDeg, power, facing, wind, t) {
        const angle = angleDeg * Math.PI / 180.0
        const speed = launchSpeed(power)
        const dir = facing < 0 ? -1.0 : 1.0
        const vx0 = dir * speed * Math.cos(angle)
        const vy0 = speed * Math.sin(angle)
        const w = Math.max(-windMax, Math.min(windMax, wind))

        const x = origin.x + vx0 * t + 0.5 * w * t * t
        const y = origin.y - vy0 * t + 0.5 * gravity * t * t
        return Qt.point(x, y)
    }

    function pointIntersectsBox(point, boxCenter, boxWidth, boxHeight) {
        return Math.abs(point.x - boxCenter.x) * 2.0 <= boxWidth
            && Math.abs(point.y - boxCenter.y) * 2.0 <= boxHeight
    }

    function simulateShot(origin, angleDeg, power, facing, wind, groundY) {
        const angle = angleDeg * Math.PI / 180.0
        const speed = launchSpeed(power)
        const dir = facing < 0 ? -1.0 : 1.0
        const vx0 = dir * speed * Math.cos(angle)
        const vy0 = speed * Math.sin(angle)
        const w = Math.max(-windMax, Math.min(windMax, wind))

        const discriminant = vy0 * vy0 + 2.0 * gravity * (groundY - origin.y)
        if (discriminant < 0.0)
            return { hitGround: false, landingX: 0, flightTime: 0 }

        const t = (vy0 + Math.sqrt(discriminant)) / gravity
        return {
            hitGround: true,
            landingX: origin.x + vx0 * t + 0.5 * w * t * t,
            flightTime: t
        }
    }

    // AI functions
    function calculateAIShot(origin, target, facing, wind, groundY) {
        let bestError = Number.MAX_VALUE
        let bestAngle = 45.0
        let bestPower = 50.0

        for (let angle = 20; angle <= 80; angle += 2) {
            for (let power = 10; power <= 100; power += 2) {
                const result = simulateShot(origin, angle, power, facing, wind, groundY)
                if (!result.hitGround)
                    continue
                const error = Math.abs(result.landingX - target.x)
                if (error < bestError) {
                    bestError = error
                    bestAngle = angle
                    bestPower = power
                }
            }
        }

        // Add random error
        const angleError = (Math.random() * 2.0 - 1.0) * maxAngleError
        const powerError = (Math.random() * 2.0 - 1.0) * maxPowerError

        return Qt.point(
            clampAngle(bestAngle + angleError),
            clampPower(bestPower * (1.0 + powerError))
        )
    }

    // Terrain functions
    function terrainHeightAt(x) {
        if (terrain.length === 0)
            return boardHeight

        const column = Math.max(0, Math.min(terrainColumns - 1,
                                            x * terrainColumns / boardWidth))
        const i = Math.floor(column)
        const fraction = column - i
        const a = terrain[i]
        const b = terrain[Math.min(i + 1, terrainColumns - 1)]
        return a + (b - a) * fraction
    }

    function generateTerrain() {
        let newTerrain = []
        let h = 380.0 + Math.random() * 100.0

        for (let i = 0; i < terrainColumns; i++) {
            h += Math.random() * 48.0 - 24.0
            h = Math.max(340.0, Math.min(560.0, h))
            newTerrain.push(h)
        }

        // Flatten areas for players
        const columnWidth = boardWidth / terrainColumns
        const pad1 = Math.floor(player1StartX / columnWidth)
        const pad2 = Math.floor(player2StartX / columnWidth)

        for (let i = pad1 - 2; i <= pad1 + 2; i++)
            newTerrain[i] = newTerrain[pad1]
        for (let i = pad2 - 2; i <= pad2 + 2; i++)
            newTerrain[i] = newTerrain[pad2]

        terrain = newTerrain
        terrainChanged()
    }

    function positionPlayers() {
        player1.x = player1StartX
        player1.y = terrainHeightAt(player1StartX)
        player2.x = player2StartX
        player2.y = terrainHeightAt(player2StartX)
    }

    // Game flow functions
    function startGame(mode) {
        if (gameMode !== mode) {
            gameMode = mode
            gameModeChanged()
        }

        projectileInFlight = false
        projectileInFlightChanged()
        pendingDirectHit = false

        // Reset players
        player1.health = maxHealth
        player1.alive = true
        player1.angle = 45.0
        player1.power = 50.0
        player1.facing = 1
        player1.isAI = false

        player2.health = maxHealth
        player2.alive = true
        player2.angle = 45.0
        player2.power = 50.0
        player2.facing = -1
        player2.isAI = (mode === GameMode.VsAI)
        player2.name = (mode === GameMode.VsAI) ? "CPU" : "Player 2"

        generateTerrain()
        positionPlayers()

        winner = null
        winnerChanged()

        currentPlayer = player1
        currentPlayerChanged()

        randomizeWind(true)
        phase = Phase.Player1Aim
        phaseChanged()
    }

    function returnToMenu() {
        phase = Phase.Menu
        phaseChanged()
    }

    function fireProjectile() {
        if (phase !== Phase.Player1Aim && phase !== Phase.Player2Aim && phase !== Phase.AITurn)
            return

        const shooter = currentPlayer
        const angle = shooter.angle * Math.PI / 180.0
        const dir = shooter.facing < 0 ? -1.0 : 1.0
        const center = Qt.point(shooter.x, shooter.y - tankHalfHeight)
        const muzzle = Qt.point(
            center.x + dir * 12.0 * Math.cos(angle),
            center.y - 12.0 * Math.sin(angle)
        )

        launchOrigin = muzzle
        launchAngle = shooter.angle
        launchPower = shooter.power
        launchFacing = shooter.facing
        launchWind = wind
        flightTime = 0.0
        prevFlightTime = 0.0
        projectilePos = muzzle
        pendingDirectHit = false

        phase = (currentPlayer === player1) ? Phase.Player1Fire : Phase.Player2Fire
        phaseChanged()

        projectileInFlight = true
        projectileInFlightChanged()
        projectileX = muzzle.x
        projectileY = muzzle.y
        projectilePositionChanged()
        projectileFired()
    }

    function updateFlight(elapsedSeconds) {
        if (!projectileInFlight)
            return

        prevFlightTime = flightTime
        flightTime = elapsedSeconds
        const p = positionAt(launchOrigin, launchAngle, launchPower,
                           launchFacing, launchWind, elapsedSeconds)

        // Check bounds
        if (p.x < -80.0 || p.x > boardWidth + 80.0 ||
            p.y > boardHeight + 60.0 || elapsedSeconds > 10.0) {
            finishMiss()
            return
        }

        // Check collision
        const opponent = (currentPlayer === player1) ? player2 : player1
        const boxCenter = Qt.point(opponent.x, opponent.y - tankHalfHeight)

        // Substep collision detection
        for (let step = 1; step <= 4; step++) {
            const t = prevFlightTime + (elapsedSeconds - prevFlightTime) * step / 4.0
            const q = positionAt(launchOrigin, launchAngle, launchPower,
                               launchFacing, launchWind, t)

            if (pointIntersectsBox(q, boxCenter, 20.0, 20.0)) {
                impactAt(q, true)
                return
            }

            if (q.y >= terrainHeightAt(q.x)) {
                impactAt(q, false)
                return
            }
        }

        projectilePos = p
        projectileX = p.x
        projectileY = p.y
        projectilePositionChanged()
    }

    function impactAt(point, directHit) {
        projectileInFlight = false
        projectileInFlightChanged()
        projectilePos = point
        projectileX = point.x
        projectileY = point.y
        projectilePositionChanged()
        pendingDirectHit = directHit
        explosionAt(point.x, point.y)
    }

    function finishMiss() {
        projectileInFlight = false
        projectileInFlightChanged()
        pendingDirectHit = false
        resolveShot(false)
        flightFinished()
    }

    function explosionFinished() {
        if (phase !== Phase.Player1Fire && phase !== Phase.Player2Fire)
            return
        const directHit = pendingDirectHit
        pendingDirectHit = false
        resolveShot(directHit)
        flightFinished()
    }

    function resolveShot(directHit) {
        if (phase !== Phase.Player1Fire && phase !== Phase.Player2Fire)
            return

        const shooter = currentPlayer
        const target = (shooter === player1) ? player2 : player1

        if (directHit) {
            target.health -= hitDamage
            target.health = Math.max(0, target.health)
            target.alive = target.health > 0

            if (!target.alive) {
                shooter.score++
                winner = shooter
                winnerChanged()
                phase = Phase.GameOver
                phaseChanged()
                gameOver(shooter)
                return
            }
        }

        advanceTurn()
    }

    function advanceTurn() {
        currentPlayer = (currentPlayer === player1) ? player2 : player1
        currentPlayerChanged()

        randomizeWind()

        if (gameMode === GameMode.VsAI && currentPlayer === player2) {
            phase = Phase.AITurn
        } else {
            phase = (currentPlayer === player1) ? Phase.Player1Aim : Phase.Player2Aim
        }
        phaseChanged()
    }

    function randomizeWind(useFullRange) {
        if (useFullRange) {
            wind = Math.random() * 2.0 * windMax - windMax
        } else {
            const delta = Math.random() * 30.0 - 15.0
            wind = Math.max(-windMax, Math.min(windMax, wind + delta))
        }
        windChanged()
    }

    function aiTakeShot() {
        if (phase !== Phase.AITurn || !currentPlayer || !currentPlayer.isAI)
            return

        const shooter = currentPlayer
        const target = (currentPlayer === player1) ? player2 : player1

        const origin = Qt.point(shooter.x, shooter.y - tankHalfHeight)
        const aimPoint = Qt.point(target.x, target.y - tankHalfHeight)
        const shot = calculateAIShot(origin, aimPoint, shooter.facing, wind,
                                    terrainHeightAt(target.x))

        shooter.angle = shot.x
        shooter.power = shot.y
        fireProjectile()
    }
}
