shmem -- queue of autos
message queue -- for autos
init -- queue is empty, mech process status -- sleeping

auto process:
    init -- try to push into queue (if unsuccessful - drive away)
    subscribe for messages. each time 0 pops -- shift one forward
    when you are the last and move forward -- do minus to the queue len
