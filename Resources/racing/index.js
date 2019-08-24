const fs = require('fs')
const EventEmitter = require('events')
const {createClient, constants} = require('oakwood')
const {vehicleModels, playerModels} = require('oakwood')

const {
    VISIBILITY_NAME,
    VISIBILITY_ICON,
    VISIBILITY_RADAR,
    VISIBILITY_MODEL,
    VISIBILITY_COLLISION,
} = constants

const oak = createClient(process.platform !== "win32" ? {
    inbound: 'tcp://192.168.1.3:10101',
    outbound: 'tcp://192.168.1.3:10102',
} : {})

/* get random element from an array */
const rndarr = arr => arr[Math.floor(Math.random()*arr.length)]
const delay = amt => new Promise(r => setTimeout(r, amt))

function shuffle(array) {
    let counter = array.length;

    // While there are elements in the array
    while (counter > 0) {
        // Pick a random index
        let index = Math.floor(Math.random() * counter);

        // Decrease counter by 1
        counter--;

        // And swap the last element with it
        let temp = array[counter];
        array[counter] = array[index];
        array[index] = temp;
    }

    return array;
}

oak.event('start', () => {
    console.log('[info] connection started')
    oak.log('[info] oakwood-node connected')
})

oak.event('stop', () => {
    console.log('[info] connection stopped')
})

oak.event('playerConnect', pid => {
    const camPos = [316.769, 12.445, -166.209]
    const camDir = [-0.915, -0.346, 0.402]

    oak.cameraSet(pid, camPos, camDir)
    setTimeout(() => oak.cameraSet(pid, camPos, camDir), 250)
})

/* Gamemode */

oak.event('start', async () => {
    /* despawn all empty vehicles */
    const existing = await oak.vehicleList()
    if (existing.length > 0) {
        for (var i = 0; i < existing.length; i++) oak.vehicleDespawn(existing[i])
    }
})

const distance = (a, b) => (Math.pow(a[0] - b[0], 2) + Math.pow(a[2] - b[2], 2))

class Race extends EventEmitter {
    constructor() {
        super()

        this.state = 'waiting'
        this.route = fs
            .readFileSync('route.txt', 'utf8')
            .split('\n')
            .map(l => l.split(','))

        this.positions = [
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
            [177.67836, 0, -146.946365],
        ]

        this.participants = []
        this.vehicles = []
        this.proute = {}

        this.ticker = null
    }

    async start() {
        console.log('starting race')

        this.ticker = setInterval(this.tick.bind(this), 100)

        for (var i = 0; i < this.participants.length; i++) {
            const pid = this.participants[i]
            const veh = await oak.vehicleSpawn("bugatti00.i3d", this.positions[i], 90.0)

            oak.vehiclePlayerPut(veh, pid, 0)
            this.vehicles.push(veh)
        }


        this.state = 'race'
    }

    stop() {
        clearInterval(this.ticker)
    }

    async tick() {
        if (this.state === 'race') {
            this.participants.map(async pid => {
                const pos = await oak.playerPositionGet(pid)
                const rot1 = this.proute[pid][0]
                const rot2 = this.proute[pid][1]
                const diff = distance(pos, rot1)

                if (diff < Math.pow(12, 2)) {
                    this.proute[pid].shift()
                    this.emit('checkpoint', pid)
                }

                /* skipped checkpoint check */
                if (!rot2) return
                const diff2 = distance(pos, rot2)

                if (diff2 < Math.pow(12, 2)) {
                    this.emit('wrongWay', pid)
                }
            })
        }
    }

    participantAdd(pid) {
        if (this.state !== 'waiting')
            return false;

        this.participants.push(pid)
        this.proute[pid] = [].concat(this.route)

        return true;
    }

    participantRemove(pid, spectate = true) {
        this.participants = this.participants
            .filter(aid => aid !== pid)

        // TODO
    }
}

const race = new Race();

race.on('start', async (pid) => {

})

race.on('wrongWay', pid => {
    oak.chatSend(pid, 'wrong way!!!!!!!')
})

race.on('checkpoint', async (pid) => {
    oak.chatSend(pid, 'checkpoint!')
})

race.on('finish', async (pid) => {

})

oak.cmd('s', pid => race.start())

oak.cmd('j', pid => {
    race.participantAdd(pid)
})

oak.cmd('l', pid => {
    race.participantRemove(pid)
})

oak.event('playerDeath', pid => {
    race.participantRemove(pid)
})

oak.event('playerDisconnect', pid => {
    race.participantRemove(pid, false)
})






oak.cmd('spawn', async pid => {
    oak.playerHealthSet(pid, 200)
    oak.playerPositionSet(pid, race.positions[0])
    oak.playerSpawn(pid)
})

// const startRace = async () => {
//     match.started = true;
//     shuffle(match.drivers)

//     for (var i = 0; i < match.drivers.length; i++) {
//         const pid = match.drivers[i]
//         const veh = await oak.vehicleSpawn("bugatti00.i3d", positions[i], 90.0)

//         match.playerRoutes[pid] = [].concat(match.originalRoute)

//         oak.vehiclePlayerPut(veh, pid, 0)
//         match.vehicles.push(veh)
//     }

//     let checker = setInterval(async () => {
//         const players = await oak.playerList()

//         for (var i = 0; i < players.length; i++) {
//             // const pid = players[i]
//             // const pos = await oak.playerPositionGet(pid)
//             // const route = match.playerRoutes[pid][0]

//             // let dist2 = Math.pow(pos[0] - route[0], 2) + Math.pow(pos[2] - route[2], 2)
//             // if (dist2 < Math.pow(15, 2)) {
//             //     match.playerRoutes[pid].shift()
//             // }
//         }
//     }, 100)


// }

// /* main timer */
// setInterval(async () => {
//     if (!oak.playerList) return;
//     const players = await oak.playerList()

//     if (!match.started) {
//         for (var i = 0; i < players.length; i++) {
//             const pid = players[i]
//             const diff = Math.round((match.next - Date.now()) / 1000, 1)

//             if (diff != 0) {
//                 oak.hudAnnounce(pid, `Next race starts in: ${diff}sec`, 2)
//             }

//             if (diff < 4) {
//                 oak.hudCountdown(pid, diff)
//             }
//         }

//         if (Date.now() > (match.next + 150)) {
//             startRace()
//         }
//     } else {
//         for (var i = 0; i < players.length; i++) {
//             const pid = players[i]
//             oak.hudAnnounce(pid, `Position: ${getRacePosition(pid)}`, 10)
//         }
//     }
// }, 1000)

