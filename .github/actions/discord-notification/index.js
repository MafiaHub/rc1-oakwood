const core = require('@actions/core')
const fetch = require('node-fetch')

async function main() {
    try {
        let type = core.getInput('type')
        let content = ''

        if (type == 'push') {
            const platform  = core.getInput('platform')
            const succeeded = core.getInput('succeeded')

            content = succeeded == 'true'
                ? `**oakwood** ${platform} build has succeeded :white_check_mark:`
                : `**oakwood** ${platform} build has failed :no_entry:`
        }
        else if (type == 'release') {
            content = `A new **oakwood** release has been deployed :white_check_mark:\nhttps://releases.mafiahub.dev/mafiahub/oakwood/latest`
        }

        await fetch(`https://discordapp.com/api/webhooks/${process.env.DISCORD_WEBHOOK_CHANNEL}/${process.env.DISCORD_WEBHOOK_TOKEN}`, {
            method: 'POST',
            headers: {
                'content-type': 'application/json',
                'user-agent': 'bot'
            },
            body: JSON.stringify({ content })
        })
    } catch (error) {
        core.setFailed(error.message);
    }
}

main()
