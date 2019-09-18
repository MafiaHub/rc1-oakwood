const fs        = require('fs')
const path      = require('path')
const {Plugin}  = require('release-it')

const filename  = path.join(__dirname, '..', 'Code', 'Shared', 'version.h')

class Bumper extends Plugin {
    getLatestVersion() {
        const data = fs
            .readFileSync(filename, 'utf8')
            .trim()
            .split('\n')
            .filter(l => l.indexOf('#define OAK_VERSION ') !== -1)

        if (data.length < 1)
            return null

        return data[0]
            .replace('#define OAK_VERSION', '')
            .replace(/\"/gi, '')
            .trim()
    }

    bump(version) {
        this.version = version;

        const data = fs
            .readFileSync(filename, 'utf8')
            .split('\n')
            .map(l => {
                if (l.indexOf('#define OAK_VERSION ') !== -1) {
                    return `#define OAK_VERSION "${version}"`
                }

                return l
            })

        fs.writeFileSync(filename, data.join('\n'))
    }
}

module.exports = Bumper
