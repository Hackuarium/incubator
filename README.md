# Cheap and simple incubator

Can we do a simple incubator that is actually more advanced than the commercial one ?

The idea of this project is to create the hardware that allows to control a peltier
thermoelectric module in order to warm or cool down a closed space.

In order to make an incubator we want to use any polystyrene box, make a hole and
insert a module.

A smart control should allow precise control of the temperature not only to warm up
but also to cool down and to define a temperature program. Like that you could incubate
couple of culture plate at 37°C the Friday evening and program the incubator so 
that it cools down to 4°C after 24h.

Well this is still a work in progress. The PCB in done and functional. The fun starts now !

## Cloning the project

In order to clone it use:

git clone --recurse-submodules https://github.com/hackuarium/incubator

To update the submodules:

git submodule update --recursive --remote
