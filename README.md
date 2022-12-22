# UPDdispatcher
I use WSJT-X, GridWatcher and other and the UPD is always a problem to me.
So I decided to write my own UPD dispatcher that takes in the WSJT-X messages
and posts them off to all the customers. This is is.

I run it on the same PC as WSJXT-X and Gridwatcher so it is set to receive on
port 2237 which is the XSJT-X default and it relays the messages to Gridwatcher
on 127.0.0.1:2238 so that needs configuring and to another device on my home
network at 192.169.1.219:2237

It was a bit messy to set up but it works. I'd like to add a more sensible
transaction viewer but that will involve unpacking the UDP packets and that
would make things specific to these programs...

Usual disclaimers apply. Use at your own risk. May cause cancer in rats.