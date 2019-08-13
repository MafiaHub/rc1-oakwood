const fs = require('fs')

const {createClient, constants} = require('oakwood')
const {vehicleModel, playerModel} = require('oakwood')
const {playerModelList, vehicleModelList} = require('oakwood')

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

oak.event('start', () => console.log('[info] connection started'))
oak.event('stop', () => console.log('[info] connection stopped'))

/* chat system */

oak.event('player_chat', async (pid, text) => {
    /* skip messages with commands */
    if (text.indexOf('/') === 0){
        return;
    }

    /* get author player name */
    const name = await oak.player_name_get(pid)
    const msg = `[chat] ${name}: ${text}`

    /* log stuff into our local console */
    console.log('[chat]', `${name}:`, msg)

    /* send messages to each clients' chat windows */
    oak.chat_broadcast(msg)
})



/* Player system */

const spawnLocs = [
    { name: "tommy", pos: [8.62861251831, 22.8868865967, -602.147888184 ]},
    { name: "oakhill", pos: [738.030334473, 106.889381409, -228.563537598 ]},
    { name: "hoboken", pos: [537.296386719, -5.01502513885, 77.8488616943 ]},
    { name: "downtown", pos: [-188.796401978, 18.6846675873, -668.6328125 ]},
    { name: "hospital", pos: [-760.439697266, 12.6204996109, 753.350646973 ]},
    { name: "central", pos: [-1091.47839355, -7.27131414413, 5.55286931992 ]},
    { name: "china", pos: [-1709.24157715, 16.0029373169, 582.041442871 ]},
    { name: "salieri", pos: [-1774.59301758, -4.88487052917, -2.40491962433 ]},
    { name: "work", pos: [-2550.85546875, -3.96487784386, -554.806213379 ]},
    { name: "pete", pos: [61.4763, 4.72524, 107.708 ]},
    { name: "racing", pos: [-3534.42993164, 7.05113887787, -651.97338867 ]},
]

const spawnplayer = pid => {
    const loc = rndarr(spawnLocs)
    const model = rndarr(playerModelList)

    oak.chat_send(pid, `[info] spawning you at location: ${loc.name}`)
    oak.player_model_set(pid, model)
    oak.player_position_set(pid, loc.pos)
    oak.player_health_set(pid, 200)
    oak.hud_fadeout(pid, 1, 500, 0xFFFFFF)
    oak.hud_fadeout(pid, 0, 500, 0xFFFFFF)
    oak.player_spawn(pid)
}

oak.event('player_connect', async pid => {
    console.log('[info] player connected', pid)
    oak.chat_broadcast(`[info] player ${await oak.player_name_get(pid)} connected.`)
    spawnplayer(pid)
})

oak.event('player_death', async pid => {
    oak.chat_broadcast(`[info] player ${await oak.player_name_get(pid)} died.`)
    spawnplayer(pid)
})

oak.event('player_disconnect', async pid => {
    oak.chat_broadcast(`[info] player ${await oak.player_name_get(pid)} disconnected.`)
})

oak.event('player_hit', (pid, atkr, dmg) => {
    console.log('[info] player_hit', pid, atkr, dmg)
})

oak.cmd('spawn', async pid => {
    spawnplayer(pid)
})

oak.cmd('despawn', async pid => {
    oak.player_despawn(pid)
})

oak.cmd('kill', async pid => {
    oak.player_kill(pid)
})

oak.cmd('help', async (pid) => {
    console.log('[info] player asks for help', pid)
    oak.chat_send(pid, '[info] sorry, we cant help you')
})

oak.cmd('heal', async (pid) => {
    oak.player_health_set(pid, 200.0)
})

oak.cmd('healme', async (pid) => {
    oak.player_health_set(pid, 200.0)
})

oak.cmd('id', pid => {
    oak.chat_send(pid, `[info] your ID is: ${pid}`)
})

oak.cmd('tp', async (pid, targetid) => {
    const tid = parseInt(targetid)

    if (tid === NaN) {
        return oak.chat_send(pid, `[error] provided argument should be a valid number`)
    }

    if (pid == tid) {
        return oak.chat_send(pid, `[error] you can't teleport to yourself`)
    }

    if (await oak.player_invalid(tid)) {
        return oak.chat_send(pid, `[error] player you provided was not found`)
    }

    /* get target name and position */
    const pos = await oak.player_position_get(tid)
    const name = await oak.player_name_get(tid)

    /* are we in any vehicle */
    const veh = await oak.vehicle_player_inside(pid)

    if (!await oak.vehicle_invalid(veh)) {
        /* offset by height */
        pos[1] += 2;

        oak.chat_send(pid, `[info] teleporting your car to player ${name}.`)

        /* set our vehicle position */
        oak.vehicle_position_set(veh, pos)
    } else {
        oak.chat_send(pid, `[info] teleporting you to player ${name}.`)

        /* set our player position */
        oak.player_position_set(pid, pos)
    }
})

oak.cmd('list', async pid => {
    const players = await oak.player_list()

    oak.chat_send(pid, `[info] connected players:`)
    players.map(async (tid, i) => oak.chat_send(pid, `ID: ${tid} | ${await oak.player_name_get(tid)}`))
    oak.chat_send(pid, '---------------------------')
})

oak.cmd('skin', async (pid, arg1) => {
    if (!arg1) {
        return oak.chat_send(pid, '[info] usage: /skin [modelId]')
    }

    const veh = await oak.vehicle_player_inside(pid)

    if (!await oak.vehicle_invalid(veh)) {
        return oak.chat_send(pid, `[error] you can't change skin inside of vehicle!`)
    }

    const modelid = parseInt(arg1)
    oak.player_model_set(pid, playerModel(modelid))
})

oak.cmd('telelist', async (pid) => {
    oak.chat_send(pid, `Location names for /tele :`)

    spawnLocs.map((a, i) => {
        oak.chat_send(pid, `${i}. ${a.name}`)
    })
})

oak.cmd('tele', async (pid, name) => {
    const location = spawnLocs.find(el => el.name == name)

    if (!location) {
        return oak.chat_send(pid, `[error] cound't find any locations by given name, use /telelist`)
    }

    oak.chat_send(pid, `[info] teleporting your car to a location ${location.name}.`)

    /* are we in any vehicle */
    const veh = await oak.vehicle_player_inside(pid)

    if (!await oak.vehicle_invalid(veh)) {
        oak.vehicle_position_set(veh, location.pos)
    } else {
        oak.player_position_set(pid, location.pos)
    }
})




/* Spectating system */

oak.cmd('hideme', async (pid) => {
    const nameVisible = await player_visibility_get(pid, VISIBILITY_NAME)
    const iconVisible = await player_visibility_get(pid, VISIBILITY_ICON)

    oak.player_visibility_set(pid, VISIBILITY_NAME, !nameVisible)
    oak.player_visibility_set(pid, VISIBILITY_ICON, !iconVisible)
})

oak.cmd('spectate', async (pid, arg1) => {
    const tid = parseInt(arg1)

    if (await oak.player_invalid(tid)) {
        return oak.chat_send(pid, `[error] unknown player target`)
    }

    oak.camera_target_player(pid, tid)
})

oak.cmd('stop', async (pid) => {
    oak.camera_target_unset(pid)
})





/* Vehicles */

let playerVehicles = {}
let playerVehiclesValid = (pid, vid) => playerVehicles.hasOwnProperty(pid)
    ? playerVehicles[pid].indexOf(vid) !== -1
    : false

let playerVehiclesAdd = (pid, vid) => {
    if (!playerVehicles.hasOwnProperty(pid)) {
        playerVehicles[pid] = []
    }

    playerVehicles[pid].push(vid)
}

oak.event('start', async () => {
    const existing = await oak.vehicle_list()

    /* despawn all empty vehicles */
    if (existing.length > 0) {
        console.log('[info] found', existing.length, 'existing cars on start-up')

        for (var i = 0; i < existing.length; i++) {
            const veh = existing[i]
            const pass = await oak.vehicle_player_list(veh)

            if (pass.length == 0) {
                await oak.vehicle_despawn(veh)
            } else {
                console.log('[info] skipping respawn for occupied vehicle', veh)
            }
        }
    }

    let models = [
        {pos: [-1991.89, -5.09753, 10.4476], heading: 0.0, model: 148}, // Manta Prototype
        {pos: [-1974.2, -4.8862, 22.5578], heading: 0.0, model: 148}, // Manta Prototype
        {pos: [-1981.11, -4.98206, 22.7471], heading: 0.0, model: 148}, // Manta Prototype
        {pos: [-1991.69, -5.12453, 22.3242], heading: 0.0, model: 148}, // Manta Prototype
    ]

    const data = fs
        .readFileSync('./savedcars.txt', 'utf8')
        .split('\n')
        .filter(line => line.trim().length)
        .map(line => line.split(' '))
        .map(line => {
            const [model, heading, x, y, z] = line;
            return {
                heading: parseFloat(heading),
                model: parseInt(model),
                pos: [
                    parseFloat(x),
                    parseFloat(y),
                    parseFloat(z),
                ]
            }
        })

    models.concat(data).map(async car => {
        const {pos, heading, model} = car;
        oak.vehicle_spawn(vehicleModel(model), pos, heading)
    })
})

const spawncar = async (pid, model, adjustPos) => {
    const m = parseInt(model)

    if (m === NaN) {
        return oak.chat_send(pid, `[error] provided argument should be a valid number`)
    }

    oak.chat_send(pid, `[info] spawning vehicle model ${vehicleModelList[m][0]}`)

    let pos = await oak.player_position_get(pid)
    let heading = await oak.player_heading_get(pid)

    if (adjustPos === true) {
        let dir = await oak.player_direction_get(pid)
        pos = pos.map((p, i) => p + dir[i] * 1.5)
        heading -= 90.0
    }

    const veh = await oak.vehicle_spawn(vehicleModel(m), pos, heading)

    playerVehiclesAdd(pid, veh)

    return veh
}

oak.cmd('car', async (pid, model) => {
    spawncar(pid, model, true)
})

oak.cmd('putcar', async (pid, model) => {
    const veh = await spawncar(pid, model, false)
    oak.vehicle_player_put(veh, pid, 0)
})

oak.cmd('repair', async pid => {
    const veh = await oak.vehicle_player_inside(pid)
    if (await oak.vehicle_invalid(veh)) return;
    oak.vehicle_repair(veh)
})

oak.cmd('delcar', async (pid) => {
    const veh = await oak.vehicle_player_inside(pid)

    if (await oak.vehicle_invalid(veh)) {
        return oak.chat_send(pid, '[error] you are not in a vehicle')
    }

    if (!playerVehiclesValid(pid, veh)) {
        return oak.chat_send(pid, `[error] you can't remove car not spawned by you`)
    }

    oak.vehicle_despawn(vid)
    oak.chat_send(pid, `[info] car has been successfully removed`)
})

oak.cmd('fuel', async (pid, arg1 = 10.0) => {
    const fuel = parseFloat(arg1)

    const veh = await oak.vehicle_player_inside(pid)
    if (await oak.vehicle_invalid(veh)) return;

    oak.vehicle_fuel_set(veh, fuel)
})

oak.cmd('race', async (pid, flags) => {
    const f = parseInt(flags)

    if (f === NaN) {
        return oak.chat_send(pid, '[error] pakuj do pici')
    }

    oak.hud_countdown(pid, f)
})
