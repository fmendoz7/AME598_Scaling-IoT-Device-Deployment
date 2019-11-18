//PROGRAMMER: Francis Mendoza
//AME598- Scaling IOT Device Deployment

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//(!!!) NEW FUNCTIONALITY NOT PART OF ORIGINAL BOILERPLATE
//Nodemailer functionality
var nodemailer = require('nodemailer');
//let transporter = nodemailer.createTransport('smtp://ame394fall2018%40gmail.com:francissamuelmendoza7@gmail.com');
let transporter = nodemailer.createTransport('smtp://ame394fall2018%40gmail.com:nodemcu1234@smtp.gmail.com');
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

//-----------------------------------------------------------------------------------------------------
var db = MS.db("mongodb://34.216.181.26:27017/sensorData"); //change this!!!
    //REQUIRE ASSITANCE ON CHANGING MONGODB TO ROOT!

app.get("/", function (req, res) {
  res.send("Temperature: " + (VALUE1*1.8 + 32) + "F \r Humidity: " + VALUE2 + "%");
});
//-----------------------------------------------------------------------------------------------------
app.get("/getValue", function (req, res) {
  var ts = parseInt(req.query.ts);
	db.collection("data").findOne({ts:{$lte:ts}, ts:{$gt:0}}, function(err, result){
    res.send(JSON.stringify(result));
  });
});
//-----------------------------------------------------------------------------------------------------
app.get("/getAverage", function (req, res) { // edit this for A6

  //Date Parser Functionality
  var ts = parseInt(req.query.ts);
  var begin = ts;
  var end = ts;
  /*begin.setHours(0)
   * begin.setMinutes(0)
   */

	db.collection("data").find({ts:{$lte:end.getTime()}, ts:{$gt:begin.getTime()}}).toArray(function(err, result){
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
//-----------------------------------------------------------------------------------------------------
//FUNCTION: setValue
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

	db.collection("data").insert(newObj, function(err, result){});

  res.send(VALUE1 + "\n" + VALUE2);
});
//-----------------------------------------------------------------------------------------------------
//(!!!) NEW FUNCTION, NOT PART OF ORIGINAL BOILERPLATE
//FUNCTION: sendEmail
function sendEmail(temp, time) {
    let message =
    {
        // Comma separated list of recipients
        to: 'Francis Mendoza <francissamuelmendoza7@gmail.com>',
        subject: 'IoT Sensor Alert- Warning',
        // plaintext body

        //MODIFY TO GET REAL TIME STATEMENT
        text: 'Sensor tripped at ' + time + '. Temperature exceeded 100*F. Current Temperature: ' + temp,
        // HTML body

        //Can view as plain text HTML
        html: '<p> Sensor tripped at ' + time + '. Temperature exceeded 100*F. Current Temperature: ' + temp + '</p>',
        watchHtml: '<p> Sensor tripped at ' + time + '. Temperature exceeded 100*F. Current Temperature: ' + temp + '</p>'
    }


    console.log('Sending Mail');
    transporter.sendMail(message, function (err, result) {
        console.log(err, result);
    });
}
//-----------------------------------------------------------------------------------------------------
app.use(methodOverride());
app.use(bodyParser());
app.use(express.static(__dirname + '/public'));
app.use(errorHandler());

console.log("Simple static server listening at http://" + hostname + ":" + port);
app.listen(port);