// index.js - test web service
//

const express = require("express")
const cors = require("cors");
const path = require("path")
const fs = require("fs")


//
//
//

const app = express()

app.use(express.json())
app.use(express.urlencoded({ extended: false }));
app.use(cors())



//
//
//

const webRoot = path.join(__dirname, "..", "data")
app.use("/", express.static(webRoot))


app.post("/Update/:target", (req, res) => {
    const target = req.params.target
    const confFile = path.join(webRoot, target)

    try {
        if (fs.existsSync(confFile)) {
            const data = fs.readFileSync(confFile, { encoding: "utf-8" })
            const jsonOld = JSON.parse(data)
            const jsonNew = req.body
            const merged = { ...jsonOld, ...jsonNew }
            //console.log("original config: ", jsonOld)
            //console.log("request config: ", jsonNew)
            fs.writeFileSync(confFile, JSON.stringify(merged, null, 4), { encoding: "utf-8" })

            console.log("Update File :", target)
            res.send("OK")
        } else {
            console.warn("Update File : File Not Found :", target)
            res.status(404).send("File Not Exist")
        }
    } catch (err) {
        console.error(err)
        res.status(500).send("File Write Failed!")
    }
})

function toDateString(date) {
    const Y = `0000${date.getFullYear()}`.slice(-4)
    const M = `00${date.getMonth()}`.slice(-2)
    const D = `00${date.getDay()}`.slice(-2)
    const h = `00${date.getHours()}`.slice(-2)
    const m = `00${date.getMinutes()}`.slice(-2)
    const s = `00${date.getSeconds()}`.slice(-2)

    return `${Y}-${M}-${D} ${h}:${m}:${s}`    
}

app.get("/TrackLogs/:target", (req, res) => {
    const target = req.params.target
    if (target === "list") {
        try {
            const logRoot = path.join(__dirname, "TrackLogs")
            const list = fs.readdirSync(logRoot)
            const json = list.filter(file => file.slice(-4) === ".igc").map(file => {
                const fullPath = path.join(logRoot, file)
                const stat = fs.statSync(fullPath)
                return {
                    name: file,
                    size: stat.size,
                    date: toDateString(new Date(stat.mtime))
                }
            })

            console.log("List up Files :", list)
            res.json(json)
        } catch (err) {
            console.error(err)
            res.status(500).send("FAILED")
        }

    } else {
        const logFile = path.join(__dirname, "TrackLogs", target)
        try {
            if (fs.existsSync(logFile)) {
                const data = fs.readFileSync(logFile)
                console.log("Download File :", target)
                res.send(data)
            } else {
                console.warn("Download File : File Not Exit :", target)
                res.status(404).send("File Not Exist")
            }
        } catch (err) {
            console.error(err)
            res.status(500).send("FAILED")
        }
    }
})

app.delete("/TrackLogs/:target", (req, res) => {
    const target = req.params.target
    const logFile = path.join(__dirname, "TrackLogs", target)
    try {
        fs.unlinkSync(logFile)

        console.log("Delete File :", target)
        res.send("OK")
    } catch (err) {
        console.error(err)
        res.status(500).send("FAILED")
    }
})


//
//
//

const port = 8080

app.listen(port, () => {
    console.log(`web-server started. --> listen on ${port}`)
})
