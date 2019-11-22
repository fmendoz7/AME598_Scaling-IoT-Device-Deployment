//PROGRAMMER: Francis Mendoza
//AME598- Scaling IOT Device Deployment

var express = require("express");
var app = express();
var MS = require('mongoskin');
var bodyParser = require('body-parser');
var errorHandler = require('errorhandler');
var methodOverride = require('method-override');
var hostname = process.env.HOSTNAME || 'localhost';
var port = 3000;
var VALUE1 = "";
var VALUE2 = "";


var db = MS.db("mongodb://127.0.0.1/sensorData"); //change this!!!

app.get("/", function (req, res) {
    res.send("Temperature: " + (VALUE1 * 1.8 + 32) + "F \r Humidity: " + VALUE2 + "%");
});

app.get("/getValue", function (req, res) {
    var ts = parseInt(req.query.ts);
    db.collection("data").findOne({ ts: { $lte: ts }, ts: { $gt: 0 } }, function (err, result) {
        res.send(JSON.stringify(result));
    });
});


app.get("/getAverage", function (req, res) { // edit this for A6
    //Date Parser Functionality
    var ts = parseInt(req.query.ts);
    var begin =new Date(ts);
    var end = new Date(ts);
    begin.setHours(0)
    begin.setMinutes(0)
    begin.setSeconds(0)

    db.collection("data").find({ ts: { $lte: end.getTime() }, ts: { $gt: begin.getTime() } }).toArray(function (err, result) {
        console.log(err);
        console.log(result);

        var tempSum = 0;
        var humSum = 0;

        for (var i = 0; i < result.length; i++) {
            tempSum += result[i].t || 0;
            humSum += result[i].h || 0;
        }

        var tAvg = tempSum / result.length;
        var hAvg = humSum / result.length;

        var ret = { // calculate from result
            t: tAvg,
            h: hAvg
        }
        res.send(JSON.stringify(ret));
    });
});


app.get("/setValue", function (req, res) {
    var v1 = decodeURIComponent(req.query.t);
    var v2 = decodeURIComponent(req.query.h);
    VALUE1 = v1;
    VALUE2 = v2;
    var newObj = {
        t: v1,
        h: v2,
        ts: new Date().getTime()
    }

    db.collection("data").insert(newObj, function (err, result) { });

    res.send(VALUE1 + "\n" + VALUE2);
});

app.use(methodOverride());
app.use(bodyParser());
app.use(express.static(__dirname + '/public'));
app.use(errorHandler());

console.log("Simple static server listening at http://" + hostname + ":" + port);
app.listen(port);
