# Web-Available Stranger Things Light Display

![in action](https://thumbs.gfycat.com/FavorableMiserlyGreatdane-size_restricted.gif)

After season one of Stranger Things came out back in 2016 a friend of mine asked if I could recreate the christmas light scene for her upcoming halloween party. Initially she had wanted it to loop a message, but I figured why not take it a step further and build something that would let our friends send their own messages to be spelled out by the lights in real time.

This project was hacked together just in time for the party, and given my lack of experience with electronics, Arduino, and PHP, I'm sure there is a better way to do this. In any case, there are two main components to the software half of this project: the PHP web interface and the Arduino code that runs on an ESP8266.

## PHP

This is essentially a fork of the Heroku [php-getting-started](https://github.com/heroku/php-getting-started) repo with a few extra bits.

**Be sure to update the URL at the end of updateMessage.php to your own** before following the instructions in that link to get this code running on Heroku.

## Arduino

This program requires two third-party Arduino libraries:

- [FastLED](https://github.com/FastLED/FastLED)
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson)

Additionally, **be sure to fill in your own details for your router ssid and password on lines 9 and 10, as well as the URL of your app on line 13**, before putting the code on the chip.

## Hardware

The details for assembling the hardware components of this project are outside the scope of this readme, but here's the gist of my setup:

The ESP8266 is powered by a rechargeable li-ion battery. One of it's pins is connected to the LED string's data line via breadboard. The LED string's ground and power lines are connected to a 12v wall-wart via the breadboard.

## Mechanism

Initially I intended to have the app send data to the chip but making a router available to the web was more complicated than was worth it.

It turned out to be much simpler to have the chip continually poll the URL of a JSON file and parse it in order to get the message. The app simply displays a form and changes the contents of the JSON file on POST.

## Shortcomings

There is one big gotcha here: say message A is playing while two new messages, B and C, are submitted within a few seconds of one another. If message A doesn't complete almost exaclty as message B is received, message B will be skipped.

This happens for two reasons:

- The chip doesn't store a record of messages played
- The app overwrites the contents of the JSON file on every POST

I wasn't able to build the solution to this in time for the party, but it should be pretty simple to store each submitted message in the JSON file (instead of overwriting it) and then send a POST request from the chip to the app every time a message completes, signaling the app to remove that message from the JSON.
