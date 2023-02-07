CC = gcc

BASEFLAGS = -Wall -pthread -std=c99
NODEBUG_FLAGS = -dNDEBUG 
DEBUG_FLAGS = -g

LDLIBS = -lcurses -pthread

OBJS = main.o console.o centipede.o player.o threadwrappers.o enemy.o bullet.o enemyBullet.o upKeep.o

EXE = centipede

debug: CFLAGS = $(BASEFLAGS) $(DEBUG_FLAGS)
debug: $(EXE)

release: CFLAGS = $(BASEFLAGS) $(NODEBUG_FLAGS) 
release: $(EXE)

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $(EXE) $(LDLIBS)

main.o: main.c centipede.h globals.h
	$(CC) $(CFLAGS) -c main.c

player.o: player.c player.h centipede.h console.h threadwrappers.h globals.h
	$(CC) $(CFLAGS) -c player.c

enemy.o: enemy.c enemy.h centipede.h console.h threadwrappers.h globals.h enemyBulletList.h
	$(CC) $(CFLAGS) -c enemy.c

upKeep.o: upKeep.c upKeep.h centipede.h console.h threadwrappers.h globals.h player.h enemy.h bullet.h
	$(CC) $(CFLAGS) -c upKeep.c
	

console.o: console.c console.h globals.h
	$(CC) $(CFLAGS) -c console.c


threadwrappers.o: threadwrappers.c threadwrappers.h globals.h
	$(CC) $(CFLAGS) -c threadwrappers.c

centipede.o: centipede.c player.h enemy.h centipede.h console.h globals.h bullet.h globals.h threadwrappers.h bulletList.h upKeep.h
	$(CC) $(CFLAGS) -c centipede.c

bullet.o: bullet.c bullet.h player.h centipede.h console.h threadwrappers.h globals.h enemy.h llist.h
	$(CC) $(CFLAGS) -c bullet.c


enemyBullet.o: enemyBullet.c enemyBullet.h player.h centipede.h console.h threadwrappers.h globals.h 
	$(CC) $(CFLAGS) -c enemyBullet.c	

clean:
	rm -f $(OBJS)
	rm -f *~
	rm -f $(EXE)
	rm -f $(EXE)_d

run:
	./$(EXE)

run_debug:
	./$(EXE)_d
