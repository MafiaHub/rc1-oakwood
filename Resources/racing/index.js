Array.prototype.random = function () {
    return this[Math.floor((Math.random() * this.length))];
}

const fs = require('fs')
const { createClient, constants } = require('oakwood')
const { vehicleModels, playerModels } = require('oakwood')

const oak = createClient(process.platform !== "win32" ? {
    inbound: 'tcp://192.168.1.3:10101',
    outbound: 'tcp://192.168.1.3:10102',
} : {})

/*
* Gamemode constants
*/
const PLAYER_STATE_RACING = 'PLAYER_STATE_RACING',
      PLAYER_STATE_WAITING = 'PLAYER_STATE_WAITING'

const RACE_STARTING = 'RACE_STARTING',
      RACE_WARMUP = 'RACE_WARMUP',
      RACE_ACTIVE = 'RACE_ACTIVE',
      RACE_FINISHED = 'RACE_FINISHED'

const {
    VISIBILITY_NAME,
    VISIBILITY_ICON,
    VISIBILITY_RADAR,
    VISIBILITY_MODEL,
    VISIBILITY_COLLISION,
} = constants

const vehicleModelsList = [
    87, 88
]

const vehicleSpawnPositions = [
    [305.79599, 0, -137.582474],
    [297.17865, 0, -148.994385],
    [288.64566, 0, -137.530869],
    [280.282654, 0, -148.832489],
    [271.688232, 0, -137.026657],
    [263.18042, 0, -148.353851],
    [254.687592, 0, -136.756195],
    [246.128525, 0, -147.928497],
    [237.085098, 0, -136.462891],
    [228.723404, 0, -147.91481],
    [220.130203, 0, -136.46846],
    [211.618027, 0, -147.823135],
    [203.104935, 0, -135.873718],
    [194.651489, 0, -147.212463],
    [186.116394, 0, -135.614044],
    [177.67836, 0, -146.946365]
]

/* 
* Global variables
*/
let mainTicker = null
let raceState = RACE_FINISHED
let raceParticipants = {}
let raceCheckpoints = []
let raceCheckStateInteval = new Date()

/*
* check race state only in 5 second interval
*/
const onRaceTick = () => {
    if(new Date() - raceCheckStateInteval > 5000) {
        raceCheckState()
        raceCheckStateInteval = new Date()
    }
}

/* 
* restart player states
*/
const raceFinished = async () => {
    Object.values(raceParticipants).map(async player => {
        await oak.playerDespawn(player.id)

        if(player.vehicle > -1)
            await oak.vehicleDespawn(player.vehicle)

        player.state = PLAYER_STATE_WAITING

        setupStartingCamera(player.id)

        oak.hudMessage(player.id, 'Race finished, wating for enough players...', 0xFF0000)
    })

}

const raceCheckState = async () => {
    const participantsArray = Object.values(raceParticipants)

    // console.log(raceState, participantsArray.filter(player => player.laps === raceCheckpoints.length).length, participantsArray.filter(player => player.state !== PLAYER_STATE_RACING).length)

    //when some player finished racing circuit or 
    //player count in race has lowered and race cannot continue

    const activeRacers = participantsArray.filter(player => player.state === PLAYER_STATE_RACING)
    if (raceState === RACE_ACTIVE && activeRacers.length < 2) {
        console.log('FINSIHING RACE')

        raceFinished()
        raceState = RACE_FINISHED
    }

    if (raceState === RACE_FINISHED) {
        raceState = RACE_STARTING
    }

    //NOTE(DavoSK): check if race is in starting mode and there are more then 1 player waiting 
    if (raceState === RACE_STARTING && 
        participantsArray.filter(player => player.state === PLAYER_STATE_WAITING).length > 1) {

        //NOTE(DavoSK): pepare vehicles on start & change race state to starting 
        raceState = RACE_WARMUP

        let playerCounter = 0

        Object.values(raceParticipants).map(async player => {
            const spawnPos = vehicleSpawnPositions[playerCounter++]
            const playerPos = [].concat(spawnPos)

            player.state            = PLAYER_STATE_RACING
            player.laps             = 0
            player.lastCheckpoint   = 0
            player.vehicle          = await oak.vehicleSpawn(vehicleModels[vehicleModelsList.random()][1], spawnPos, 90)

            playerPos[2] += 1.5;

            await oak.playerPositionSet(player.id, playerPos)
            await oak.playerHealthSet(player.id, 200.0)
            await oak.playerSpawn(player.id)

                oak.vehiclePlayerPut(player.vehicle, player.id, 0)
            // oak.cameraUnlock(player.id)

            setTimeout(() => {
            }, 1000)
        })

        let currentCountdown = 3
        const countDownInterval = setInterval(() => {
            Object.values(raceParticipants).map(player => {
                if (player && player.state === PLAYER_STATE_RACING) {
                    oak.hudCountdown(player.id, currentCountdown)
                }
            })

            if (--currentCountdown < 0) {
                clearInterval(countDownInterval)
                raceState = RACE_ACTIVE
            }
        }, 1000)
    }
}

const raceStart = async () => {

    //#1 clean old vehicles and ticker
    if (mainTicker)
        clearInterval(mainTicker)

    raceState = RACE_STARTING
    cleanClientState()

    //#2 get checkpoints from file
    raceCheckpoints = fs.readFileSync('route.txt', 'utf8')
                        .split('\n')
                        .map(l => l.split(','))

    mainTicker = setInterval(onRaceTick, 100)
}

const cleanClientState = async () => {
    for(vid of await oak.vehicleList())
        oak.vehicleDespawn(vid)

    for (pid of await oak.playerList())
        oak.playerDespawn(pid)
}

const setupStartingCamera = (pid) => {
    const camPos = [316.769, 12.445, -166.209]
    const camDir = [-0.915, -0.346, 0.402]

    // setTimeout(() => oak.cameraSet(pid, camPos, camDir), 250)
}

/*
* on node start
*/
oak.event('start', async () => {
    console.log('[info] connection started')
    oak.log('[info] oakwood-node connected')

    raceStart()

    /* get previously connected players */
    const ids = await oak.playerList()
    ids.map(pid => oak.__events['playerConnect'].map(fn => fn(pid)))
})

/* 
* on node stop
*/
oak.event('stop', () => {
    console.log('[info] connection stopped')

    if (mainTicker)
        clearInterval(mainTicker)
})

/* 
* on player join we announce message about new player 
* we also tracks player state as new object
*/
oak.event('playerConnect', pid => {

    setupStartingCamera(pid)
    raceParticipants[pid] = {
        id: pid,
        laps: 0,
        lastCheckpoint: 0,
        vehicle: -1,
        state: PLAYER_STATE_WAITING
    }

    oak.hudMessage(pid, 'Welcome to Oakwood racing', 0xFFFFFF)
    oak.hudMessage(pid, 'Currently youve been added to waiting queue...', 0xFFFFFF)
    oak.hudMessage(pid, 'Race will start when there will be more then 2 ready players', 0xFFFFFF)
})

/* 
* after player die we change hes state to waiting 
* so he can spectate other players during race
*/
oak.event('playerDeath', async pid => {
    let playerObject = raceParticipants[pid]
    if (playerObject) {
        if(playerObject.vehicle > -1)
            await oak.vehicleDespawn(playerObject.vehicle)

        await oak.playerDespawn(pid)
        playerObject.state = PLAYER_STATE_WAITING
        setupStartingCamera(pid)
    }

    oak.hudMessage(pid, 'You are dead, now you can spectate others players during race', 0xFF0000)

    for (let otherPid of await oak.playerList())
        oak.hudMessage(otherPid, 'Player ' + await oak.playerNameGet(pid) + ' died', 0xFFFFFF)
})

/* 
* on player diconnect we remove player object from participants
* and announce disconnection
*/
oak.event('playerDisconnect', async pid => {

    let playerObject = raceParticipants[pid]
    if (playerObject && playerObject.vehicle > -1)
        await oak.vehicleDespawn(playerObject.vehicle)

    delete raceParticipants[pid]

    for(let otherPid of await oak.playerList()) 
        oak.hudMessage(otherPid, 'Player ' + await oak.playerNameGet(pid) + ' has disconnected', 0xFF0000)
})

oak.cmd('die', pid => {
    oak.playerKill(pid)
})

/*
* gamemode commands
*/
oak.cmd('start', pid => {
    raceStart()
})
