

struct sound_node_t {
	uint64_t timems;
	uint64_t freq;
	sound_node_t * next;
}
static int numSounds = 0;
static sound_node_t * prev, current=NULL;

void playSound();
void addSound(uint64_t timems, uint64_t freq);

uint64_t addSound(uint64_t timems, uint64_t freq){
	if(numSounds>=MAX_LIST_SIZE) return -1; 
	sound_node_t * sound=malloc(sizeof(sound_node_t));
	sound->timems=time()+timems;
	sound->freq=freq;
	if(numSounds==0 || current==NULL){
		sound->next=sound;
		prev=sound
		current=sound;
	}else{
		sound->next=current->next;
		current->next=sound;
		if(prev==current){
			prev=sound;
		}
	}
	numSounds++;
	return 0;
}

void playSound(){
	if(numSounds<=0 || current==NULL){
		_int80(SPEAKER, 0,0,0);
		return 0;
	}
	uint64_t currentTime = time();
	if(currentTime>=current->timems){
		if(prev==current){
			current=NULL;
			prev=NULL;
		}else{
			prev->next=current->next;
			current=current->next; 	
		}
		//TODO free(?)
		playSound();
		return 0;
	}
	_int80(SPEAKER, current->freq, 0, 0);
	current=current->next;
	return 0;
}


void initSoundPlayer(){
	while(1){
		playSound();
		yield();
	}
}