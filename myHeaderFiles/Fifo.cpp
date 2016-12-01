//
// Created by samuel on 11/28/2016.
//

#include "Fifo.h"

Fifo::Fifo(){
    empty = true;
    full = false;
    head = 0;
    tail = 0;
}

bool Fifo::isEmpty(){
    return empty;
}
bool Fifo::isFull(){
    return full;
}
message Fifo::getMsg(){
    return mBuffer[tail];
}

int Fifo::pop(){
    if(empty) return -1;

    full = false; // no longer full
    tail++;
    if (tail == FifoSize){
        tail = 0;
    }
    if (tail == head) {
        empty = true;
    }
    return 0;

}
int Fifo::push(message m){
    if (full) return -1; //error couldn't store message

    empty = false; // no longer empty

    mBuffer[head] = m;

    head++;
    if (head == FifoSize){ //put back at beginning of circular buffer
        head = 0;
    }
    if (head==tail){
        full = true;
    }
    return 0; // no error
}

int Fifo::getHead(){
    return head;
}
int Fifo::getTail(){
    return tail;
}