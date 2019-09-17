const core = require('@actions/core')
const fetch = require('node-fetch')

async function main() {
    try {
        const platform  = core.getInput('platform')
        const succeeded = core.getInput('succeeded')

        const content = succeeded
            ? `**oakwood** ${platform} build has succeeded :white_check_mark:`
            : `**oakwood** ${platform} build has failed :no_entry:`

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
