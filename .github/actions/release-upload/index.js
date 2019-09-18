const fs     = require('fs')
const path   = require('path')
const core   = require('@actions/core')
const github = require('@actions/github')

async function main() {
    try {
        const {GIHUB_TAG, GITHUB_REPO, GITHUB_TOKEN} = process.env
        const octokit   = new github.GitHub(GITHUB_TOKEN)

        const input     = core.getInput('input')
        const output    = core.getInput('output')

        const [owner, repo] = GITHUB_REPO.split('/')
        const release = await octokit.repos.getReleaseByTag({ owner, repo, tag: GIHUB_TAG })

        let type = 'application/octet-stream'

        if (input.indexOf('zip') !== -1) {
            type = 'application/zip'
        }

        const fileData = fs.readFileSync(input)

        await octokit.repos.uploadReleaseAsset({
            url: release.upload_url,
            name: output.replace('{v}', GIHUB_TAG),
            headers: {
                'content-type': type,
                'content-length': fileData.length,
            },
            fileData,
        })
    } catch (error) {
        core.setFailed(error.message);
    }
}

main()
