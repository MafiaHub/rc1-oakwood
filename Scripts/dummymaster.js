const http = require('http')

/**
 * Returns a random integer between min (inclusive) and max (inclusive)
 * Using Math.round() will give you a non-uniform distribution!
 */
function getRandomInc(min, max) {
    return Math.floor(Math.random() * (max - min + 1)) + min;
}

const versions = [
    'v1.0.0',
    'v1.1.2',
    'v2.5.4',
    'v2.5.4-pre2',
]

const ipAddresses = [
	'192.168.0.1',
	'192.168.0.108',
	'192.168.42.1',
	'192.168.50.9',
	'192.168.80.15',
	'192.168.18.5',
	'192.168.4.2',
]

const randomServers = (min, max) => {
    return [...Array(getRandomInc(min, max)).keys()].map(id => {
        return { id,
            name: `Random Server Name #${id}`,
			ip: ipAddresses[getRandomInc(0, ipAddresses.length - 1)],
            version: versions[getRandomInc(0, versions.length - 1)],
            has_password: Math.random() > 0.75,
            players_max: getRandomInc(50, 1000),
            players_now: getRandomInc(0, 50),
        }
    })
}

const head = {
    'Content-Type': 'application/json',
    'Access-Control-Allow-Origin': '*',
    'Access-Control-Allow-Headers': 'Origin, X-Requested-With, Content-Type, Accept',
}

const data = {
    'status': 'success',
    'servers': randomServers(30, 40),
}

http.createServer((req, res) => {
    res.writeHead(200, head)
    res.end(JSON.stringify(data))
}).listen(3000)

console.log('m2o-api: started server at http://localhost:3000/')