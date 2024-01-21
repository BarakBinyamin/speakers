const {spawnSync} = require('node:child_process')
const ip          = require('ip')
const fs          = require('fs')

const CONF   = "./capacitor.config.json"
const TMPCONF= "./capacitor.config.json.tmp"
const devUrl = `http://${ip.address()}:5173`

const devModeConfig =  {
  "url": `${devUrl}`,
  "cleartext": true
} 

const spawnOptions = {
  cwd: process.cwd(),
  env: process.env,
  stdio: [process.stdin, process.stdout, process.stderr],
  encoding: 'utf-8'
}

// Read config file
// Copy config file to capacitor.config.json.tmp
// Overwrite capacitor.config.json
// npx cap copy // copy to all native projects
// npx cap open ios
// Revert changes by writing capacitor.config.json.tmp back
//  to capacitor.config.json
// Delete capacitor.config.json.tmp

const capConfig     = require(CONF)
const devConfig     = structuredClone(capConfig)
devConfig["server"] = devModeConfig

fs.copyFileSync(CONF, TMPCONF)
fs.writeFileSync(CONF, JSON.stringify(devConfig))

spawnSync("npx", ["cap","copy"], spawnOptions)
spawnSync("npx", ["cap","open","ios"], spawnOptions)

fs.rmSync(CONF)
fs.renameSync(TMPCONF, CONF)

