# UPDdispatcher
For ham radio FT-8 protocol I use WSJT-X, GridWatcher and some others and the
UPD feed is always a problem to me. Hence I decided to write my own UPD
dispatcher that takes in the WSJT-X messages and posts them off to all the
customers. This is is.

I run it on the same PC as WSJXT-X and Gridwatcher so it is set to receive on
port 2237 which is the WSJT-X default and it relays the messages to Gridwatcher
on 127.0.0.1:2238 so that needs re-configuring and to another device on my home
network at 192.169.1.219:2237. See the screenshot below.

![alt text](selfie.jpg)

It was a bit messy to set up but it works now. I might add a more sensible
transaction viewer later but that will involve unpacking the UDP packets and
that would make things specific to these programs while at the moment it just
takes N bytes and honest brokers them onwards...

This is just the first dump from my workbench so the code deserves a bit of
a tidy up which will happen later.

It saves it's configuration file in your 'roaming' folder. I made it remember
it's place on the screen but it could just as well be setup to be minimised.

Usual disclaimers apply. Use at your own risk. May cause cancer in rats.