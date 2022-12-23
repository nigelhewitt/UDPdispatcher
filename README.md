# UPDdispatcher
For ham radio FT-8 protocol I use WSJT-X, GridWatcher and some others and
getting the UPD feed to all of them is always a problem to me. If I can set
up a chain any one program can knock the whole system over. Hence I decided
to write my own UPD dispatcher that takes in the WSJT-X messages and posts
them off to all the users and if one of them stops responding it just
retries it until it comes back.

I run it on the same PC as WSJXT-X and Gridwatcher so it is set to receive on
port 2237 which is the WSJT-X default and it relays the messages to Gridwatcher
on 127.0.0.1:2238 so that needs re-configuring and to another device on my home
network at 192.169.1.219:2237. See the screenshot below.

![alt text](selfie.jpg)

It was a bit messy to relearn WebSockets after many years but it works now.
I might add a more sensible transaction viewer later but that will involve
unpacking the UDP packets and that would make things specific to these programs
while at the moment it just takes any UDP stream and honest brokers it onwards...

It does do reverse taking any messages from the hosts and pumping them back to
the origonal client but there is no sense in which it provides seperate identities.
It is fully multi-threaded so all the sub-systems run independently.
It saves its configuration file in your 'roaming' folder. I made it remember
it's place on the screen but it could just as well be setup to be minimised.

Usual disclaimers apply. Use at your own risk. May cause cancer in rats.