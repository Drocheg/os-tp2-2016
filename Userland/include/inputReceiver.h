#ifndef INPUTRECEIVER_H
#define INPUTRECEIVER_H

/**
 *Start function for the inputReceiver. Arg 0 is msgQSend, the msgQ where
 *this process will send the inputs. Arg 1 is msgQReceive, the msgQ where 
 *the inputReceiver will receive the signal to be destroyed.
*/
void inputReceiver_start(int argc, char* argv[]);
#endif