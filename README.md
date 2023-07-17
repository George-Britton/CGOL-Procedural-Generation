# CGOL Procedural Generation
### A computer game developed using UE4, demonstrating cellular automation in the context of city generation

---------------------------------------

This project was great fun, and through it I developed my understanding of procedural generation.

In this game I use the popular _Conway's Game of Life_ to generate a city of buildings, wrecked cars, and enemy spawners.

The generator also creates a forest and sea, but looking back at this I realise just how unnecessary that is, given the intended gameplay is fully in the city, and not the surrounding environment.

I also created a rudimentary finite-state machine to control the enemies, which isn't super detailed, but still uses:
- The observer pattern
- Distance culling
- Object pooling

Is this super fancy? No. Is it the cutting edge of AI? Also no. Does it reduce the computational overhead to a fraction of its default? Yes.
