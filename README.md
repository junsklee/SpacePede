# SpacePede
 A multithreaded console shooting game developed with ncurses. 

Users must install ncurses (libncurses5) before playing the game.

 ### Downloading ncurses

	$ sudo apt-get install libncurses5

 ## Usage
 
	$ make
 
 *After make has run successfully, run the exe:*
 
 	$ ./centipede

 ## Objective

 To win, players must remove all existing centipedes on the console by shooting bullets at them, one at a time. 
	Users must beware, when a centipede is shot, it is split into two smaller centipedes, one (the front half) moving much faster than before! 
	When an already-split centipede is shot and cannot get any smaller, then it dies and its corpse remains on the screen.
	Centipedes spawn at a regular interval, and each of them can shoot their own bullets.
	When the spaceship makes contact with a caterpillar's bullet, the player loses a life while the console cleans up.

	
## Playing The Game
Use the WASD keys to move up, left, down, right, respectively. Press the spacebar to shoot a bullet towards a centipede. 


	
![SpacePede_1](https://github.com/jun1750/SpacePede/assets/50674368/4f2343f6-36de-40dd-9031-5856d4daab32)
![SpacePede_2](https://github.com/jun1750/SpacePede/assets/50674368/315e2d4b-2291-4c8c-8cfb-35f4a72ca96e)
![SpacePede_Game_Over](https://github.com/jun1750/SpacePede/assets/50674368/b4093254-61a2-4a14-9245-57001181dfc4)
![SpacePede_Win](https://github.com/jun1750/SpacePede/assets/50674368/19ea72b7-6c6f-4999-882d-7e7c68e0bf4d)
