# Problem 1 Implementation

### player_enterAcademy

The player thread calls wait on semaphore sem_2player. This semaphore is a counting
semaphore with limit of 2. This makes it so only 2 players meet organizer at a time.
Rest of the player threads wait.

### player_meetOrganizer

The player thread calls post on semaphore sem_organizer_wait in order to signal
organizer that it is waiting to meet it. sem_organizer_wait is a binary semaphore.

### player_enterCourt

The player thread calls wait on semaphore sem_organizer_confirm. This semaphore is
a binary semaphore. This makes it so that the player thread waits until the
organizer signals that it has met both players and the referee and is ready
to lead them to the court.

### player_warmUp

The player thread calls sleep(1) in order to warm up. Then it call signal on
semaphore sem_player_ready in order to signal the referee that it is ready.
sem_player_ready is a binary semaphore. After that it call wait on semaphore
sem_match_start in order to wait for signal from referee that the match has started.
sem_match_start is a binary semaphore. Finally it calls post on semaphore sem_2player
in order to let another player meet the organizer.

### referee_enterAcademy

The referee thread calls wait on semaphore sem_1referee. This semaphore is a counting
semaphore with limit of 1. This makes it so only 1 referee meets organizer at a time.
Rest of the referee threads wait.

### referee_meetOrganizer

The referee thread calls post on semaphore sem_referee_wait in order to signal
organizer that it is waiting to meet it. sem_referee_wait is a binary semaphore.

### referee_enterCourt

The referee thread calls wait on semaphore sem_organizer_confirm. This semaphore is
a binary semaphore. This makes it so that the referee thread waits until the
organizer signals that it has met both players and the referee and is ready
to lead them to the court.

### referee_adjustEquipment

The referee thread calls sleep(0.5) in order to adjust the equipment.

### referee_startGame

The referee thread calls wait on semaphore sem_player_ready twice in order to wait
for both players to finish warm up and get ready for the match. sem_player_ready
is a binary semaphore. Then it calls post on semaphore sem_match_start thrice in order
to signal both players and the organizer that the match has started. Finally it calls
post on semaphore sem_1referee in order to let another referee meet the organizer.

### organizer

The organizer thread calls wait on semaphore sem_organizer_wait twice in order to
wait for 2 players to meet it. It calls wait on semaphore on sem_referee_wait in
order to wait for referee to meet it. Then it calls post on sem_organizer_confirm
thrice to signal to both players and referee that it has met them and that they
can enter the court. Finally it calls wait on sempahore sem_match_start in order to
wait for referee to signal match start so that organizer can go and meet another
set of players and referee.

# Input Format
**N** - Positive Integer

# How to Run
gcc -pthread prob1.c <br>
./a.out
