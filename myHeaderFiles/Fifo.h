//
// Created by samuel on 11/28/2016.
//

#ifndef EX3_FIFO_H
#define EX3_FIFO_H

//struct message{
    //unsigned char validity;
    //unsigned char address;
    //unsigned char cmd;
//};

#define FifoSize 2
struct fifo{
	struct message m[FifoSize];
	bool empty;
	bool full ;
	uint8_t head ;
	uint8_t tail ;
};

void fifoInit(fifo *f){
	for(uint8_t i = 0; i < FifoSize; i++){
		f-> m[i].address = '*';
		f-> m[i].cmd = '*';
		f-> m[i].validity = '*';
	}
	f->empty = true;
	f->full = false;
	f->head = 0;
	f->tail = 0;
}
bool isEmpty(fifo *f){
	return f->empty;
}
bool isFull(fifo *f){
	return f->full;
}
message getMsg(fifo *f){
	return f->m[f->tail];
}
int pop(fifo *f){
	if(f->empty) return -1;

	f->full = false; // no longer full
	f->tail++;
	if (f->tail == FifoSize){
		f->tail = 0;
	}
	if (f->tail == f->head) {
		f->empty = true;
	}
	return 0;
}
int push(message m, fifo *f) {
	if (f->full) return -1; //error couldn't store message

	f->empty = false; // no longer empty

	f->m[f->head] = m;

	f->head++;
	if (f->head == FifoSize){ //put back at beginning of circular buffer
		f->head = 0;
	}
	if (f->head==f->tail){
		f->full = true;
	}
	return 0; // no error
}
int getHead(fifo *f){
	return f->head;
}
int getTail(fifo *f){
	return f->tail;
}


#endif //EX3_FIFO_H
