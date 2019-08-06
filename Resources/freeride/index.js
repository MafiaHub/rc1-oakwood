const {createClient, vehicleModel, vehicleModelList} = require('oakwood')

const oak = createClient(process.platform !== "win32" ? {
    inbound: 'tcp://192.168.1.3:10101',
    outbound: 'tcp://192.168.1.3:10102',
} : {})

/* intiailization */

oak.event('start', async () => {
    console.log("[info] connected to the server")
    oak.log("[info] hello world from nodejs")
})

/* general player events */

const spawnplayer = pid => {
    oak.player_position_set(pid, [-1774.59301758, -4.88487052917, -2.40491962433])
    oak.player_health_set(pid, 200)
    oak.hud_fadeout(pid, 1, 500, 0xFFFFFF)
    oak.hud_fadeout(pid, 0, 500, 0xFFFFFF)
    oak.player_spawn(pid)
}

oak.event('player_connect', async pid => {
    console.log('[info] player connected', pid)
    spawnplayer(pid)
})

oak.event('player_death', async pid => {
    console.log('[info] player died', pid)
    spawnplayer(pid)
})

oak.event('player_disconnect', pid => {
    console.log('[info] player disconnected', pid)
})

oak.event('player_hit', (pid, atkr, dmg) => {
    console.log('player_hit', pid, atkr, dmg)
})

oak.event('player_key', async (pid, key) => {
    if (key != 32) return;

    const veh = await oak.vehicle_player_inside(pid)
    if (await oak.vehicle_invalid(veh)) return;

    const pos = await oak.vehicle_position_get(veh)
    pos[1] += 10;

    oak.vehicle_position_set(veh, pos)
})

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
    console.log(msg)

    /* send messages to each clients' chat windows */
    oak.chat_broadcast(msg)
})

/* helper commands */

oak.cmd('spawn', async pid => {
    spawnplayer(pid)
})

oak.cmd('repair', async pid => {
    const veh = await oak.vehicle_player_inside(pid)
    if (await oak.vehicle_invalid(veh)) return;
    oak.vehicle_repair(veh)
})

oak.cmd('despawn', async pid => {
    oak.player_despawn(pid)
})

oak.cmd('kill', async pid => {
    oak.player_kill(pid)
})

oak.cmd('race', async (pid, flags) => {
    const f = parseInt(flags)

    if (f === NaN) {
        return oak.chat_send(pid, '[error] pakuj do pici')
    }

    oak.hud_countdown(pid, f)
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
    }

    const veh = await oak.vehicle_spawn(vehicleModel(m))

    oak.vehicle_position_set(veh, pos)
    oak.vehicle_heading_set(veh, heading - 90.0)

    return veh
}

oak.cmd('car', async (pid, model) => {
    const id = await spawncar(pid, model, true)
    console.log('spawned', id)
})

oak.cmd('push', async (pid, veh) => {
    veh = parseInt(veh)
    const pos = await oak.vehicle_position_get(veh)
    pos[1] += 10
    oak.vehicle_position_set(veh, pos)
})

oak.cmd('putcar', async (pid, model) => {
    const veh = await spawncar(pid, model, false)

    oak.vehicle_player_put(veh, pid, 0)
})

oak.cmd('help', async (pid) => {
    console.log('player asks for help', pid)
    oak.chat_send('[info] sorry, we cant help you')
})

oak.cmd('heal', async (pid) => {
    oak.player_health_set(pid, 200.0)
})

oak.cmd('goto', async (pid, targetid) => {
    const tid = parseInt(targetid)

    if (tid === NaN) {
        return oak.chat_send(pid, `[error] provided argument should be a valid number`)
    }

    if (await oak.player_invalid(tid)) {
        return oak.chat_send(pid, `[error] player you provided was not found`)
    }

    /* get target position */
    const pos = await oak.player_position_get(tid)

    /* set our player position */
    oak.player_position_set(pid, pos)
})

oak.cmd('list', async pid => {
    console.log("players", await oak.player_list())
    console.log("cars", await oak.vehicle_list())
})

oak.cmd('delcar', async (pid, arg1) => {
    const vid = parseInt(arg1)
    oak.vehicle_despawn(vid)
})

const prevhealth = {}

setInterval(async () => {
    const players = await oak.player_list()

    for (var i = 0; i < players.length; i++) {
        const pid = players[i]

        if (!prevhealth.hasOwnProperty(pid)) {
            prevhealth[pid] = await oak.player_health_get(pid)
            return
        }

        const newhealth = await oak.player_health_get(pid)
        const diff = prevhealth[pid] - newhealth

        if (diff != 0.0) {
            console.log('health is different!', diff, pid)
        }

        prevhealth[pid] = newhealth
    }
}, 1000)
