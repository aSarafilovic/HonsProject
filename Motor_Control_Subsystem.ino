// setup the pins and variables for all the components

//encoder variables

//enc1
#define ENC_1_A 2
#define ENC_1_B 3
volatile int counter1 = 0;
//enc2
#define ENC_2_A 7
#define ENC_2_B 8
volatile int counter2 = 0;
//enc3
#define ENC_3_A 12
#define ENC_3_B 13
volatile int counter3 = 0;
//enc4
#define ENC_4_A 17
#define ENC_4_B 18
volatile int counter4 = 0;
//enc5
#define ENC_5_A 22
#define ENC_5_B 23
volatile int counter5 = 0;
//enc6
#define ENC_6_A 27
#define ENC_6_B 28
volatile int counter6 = 0;
//enc7
#define ENC_7_A 32
#define ENC_7_B 33
volatile int counter7 = 0;
//enc8
#define ENC_8_A 37
#define ENC_8_B 38
volatile int counter8 = 0;

//motor variables

//motor 1
#define pwrPin1 4
#define brkPin1 5
//motor 2
#define pwrPin2 9
#define brkPin2 10
//motor 3
#define pwrPin3 14
#define brkPin3 15
//motor 4
#define pwrPin4 19
#define brkPin4 20
//motor 5
#define pwrPin5 24
#define brkPin5 25
//motor 6
#define pwrPin6 29
#define brkPin6 30
//motor 7
#define pwrPin7 34
#define brkPin7 35
//motor 8
#define pwrPin8 29
#define brkPin8 40

//bumper variables
#define bumper1 6
#define bumper2 11
#define bumper3 16
#define bumper4 21
#define bumper5 26
#define bumper6 31
#define bumper7 36
#define bumper8 41

bool calibrating = false;

//define the encoder state changing variables
unsigned long _lastIncReadTime = micros(); 
unsigned long _lastDecReadTime = micros(); 
int _pauseLength = 25000;
int _fastIncrement = 1;



//include protothreads
#include <pt.h>
static struct pt pt1; // lat 1
static struct pt pt2; // rot 1
static struct pt pt3; // lat 2
static struct pt pt4; // rot 2
static struct pt pt5; // lat 3
static struct pt pt6; // rot 3
static struct pt pt7; // lat 4
static struct pt pt8; // rot 4

//define the limits for the lateral movements
int limit1 = 0;
int limit2 = 0;
int limit3 = 0;
int limit4 = 0;


//define it here for global scope
bool endReached;

//define the serial input line
String input = "MPPPA";


//define functions before setup for global scope
//basically if you want to use a user-defined function in a lambda function put it above the setup.

void read_encoder(int pin_A, int pin_B, int counter){

  // Encoder interrupt routine given the two pins and the counter that you're tracking
 
  static uint8_t old_AB = 3;  // Lookup table index
  static int8_t encval = 0;   // Encoder value  
  static const int8_t enc_states[]  = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0}; // Lookup table

  old_AB <<=2;  // Remember previous state

  if (digitalRead(pin_A)) old_AB |= 0x02; // Add current state of pin A
  if (digitalRead(pin_B)) old_AB |= 0x01; // Add current state of pin B
  
  encval += enc_states[( old_AB & 0x0f )];

  // Update counter if encoder has rotated a full indent, that is at least 4 steps
  if( encval > 3 ) {        // Four steps forward
    int changevalue = 1;
    if((micros() - _lastIncReadTime) < _pauseLength) {
      changevalue = _fastIncrement * changevalue; 
    }
    _lastIncReadTime = micros();
    counter = counter + changevalue;              // Update counter
    encval = 0;
  }
  else if( encval < -3 ) {        // Four steps backward
    int changevalue = -1;
    if((micros() - _lastDecReadTime) < _pauseLength) {
      changevalue = _fastIncrement * changevalue; 
    }
    _lastDecReadTime = micros();
    counter = counter + changevalue;              // Update counter
    encval = 0;
  }
}

//raw movement functions
void clockwise(int PinA, int PinB){
  digitalWrite(PinA, HIGH);
  digitalWrite(PinB, LOW);
}

void anticlockwise(int PinA, int PinB){
  digitalWrite(PinA, LOW);
  digitalWrite(PinB, HIGH);
}

void hold(int PinA, int PinB){
  digitalWrite(PinA, HIGH);
  digitalWrite(PinB, HIGH);
}

//higher level movement functions
//these return a true value when theyre completed so that 
//the protothread can wake back up when it's finished
bool move_to(int PinA, int PinB, int counter, int pos){
  //this one is used for both lateral and player moves
  if (counter<pos){
    while (counter<pos){
      anticlockwise(PinA, PinB);
    }
    hold(PinA, PinB);
  }
  else if (counter>pos){
    while (counter>pos){
      clockwise(PinA, PinB);
    }
    hold(PinA, PinB);
  }
  return true;
  //if pos and counter are equal do nothing
}

//player actions
bool kick(int PinA, int PinB, int counter){
  move_to(PinA, PinB, counter, -6);
  move_to(PinA, PinB, counter, 6);
  move_to(PinA, PinB, counter, 0);
  return true;
}

bool stand(int PinA, int PinB, int counter){
  move_to(PinA, PinB, counter, 0);
  return true;
}

bool horizontal(int PinA, int PinB, int counter){
  move_to(PinA, PinB, counter, -6);
  return true;
}

//calibration
bool calibrateBase(int PinA, int PinB){
  calibrating = true;
  endReached = false;
  //go to one end
  while (endReached==false){
    clockwise(PinA,PinB);
  }
  hold(PinA,PinB);
  endReached = false;
  calibrating = false;
  return true;
  //when you hit the first bumper set the counter as 0
  //when you hit the second bumper, set the limit to whatever the counter is at that point
}
int calibrateEnd(int PinA, int PinB, int motorset){
  calibrating = true;
  endReached = false;
  while (endReached == false){
    anticlockwise(PinA, PinB);
  }
  hold(PinA,PinB);
  endReached = false;
  //motorset will only ever be 1,2,3 or 4
  calibrating = false;
  switch(motorset){
    case 1:
      return counter1;
    case 2:
      return counter2;
    case 3:
      return counter3;
    case 4:
      return counter4;  
  }
}


//this still needs to be finished
void pressed(int pin){
  if (calibrating = true){
    //replace with the values for the pin that's being pressed (in ascending order)
    switch(pin){
      case 1:
        endReached = true;
        break;
      case 2:
        endReached = true;
        break;
      case 3:
        endReached = true;
        break;
      case 4:
        endReached = true;
        break;
      case 5:
        endReached = true;
        break;
      case 6:
        endReached = true;
        break;
      case 7:
        endReached = true;
        break;
      case 8:
        endReached = true;
        break;
    }
  }
  else if (calibrating = false){
      //reconfigure these again to match the 
    switch(pin){
      case 1:
        hold(1,2);
        break;
      case 2:
        hold(1,2);
        break;
      case 3:
        hold(1,2);
        break;
      case 4:
        hold(1,2);
        break;
      case 5:
        hold(1,2);
        break;
      case 6:
        hold(1,2);
        break;
      case 7:
        hold(1,2);
        break;
      case 8:
        hold(1,2);
        break;
    }
  }
}

//set up the protothreads////////////////////////////////////////////////////////////////////////////////

static int protothread1(struct pt *pt){
  static unsigned long time = 0;

  //motor type: 
  int motorDes = 1;
  bool player = true;
  //redesignate the defined pins as local variables so I can copy-paste 
  //the main body code between protothreads and just change a few numbers
  
  //define counter
  int loccounter = counter1;
  //encoder pins:
  int e1 = ENC_1_A;
  int e2 = ENC_1_B;
  //motor pins:
  int m1 = pwrPin1;
  int m2 = brkPin1;
  //(optional) bumper pins (only if player == false): 
  int b1 = bumper1;
  int b2 = bumper2;
  //
  int loclimit = limit1;
  //define the characters that can be used
  String locinput = input;
  int m;
  int locpos;
  int act;
  ///////////////////////////////////////////// setup done
  PT_BEGIN(pt);
  while (1){
    //segment the input line
    m = input.substring(0,1).toInt();
    locpos = locinput.substring(1,4).toInt();
    act = input.substring(4,5).toInt();
    if (motorDes == m){
      //we put the activation of the movement functions inside a wait until so that the 
      //thread sleeps and allows other threads to run while the movement code is running.
      //motor control code
      if (0<locpos && locpos < loclimit && player == false){
        PT_WAIT_UNTIL(pt, move_to(m1,m2,loccounter,locpos));
      }
      else if(player == false && locpos == 999){
        calibrateBase(m1,m2);
        loclimit = calibrateEnd(m1,m2,motorDes);
      }
      //motor action code
      if (act == 1){
        PT_WAIT_UNTIL(pt, kick(m1,m2,loccounter));
      }
      else if (act == 2){
        PT_WAIT_UNTIL(pt, stand(m1,m2,loccounter));
      }
      else if (act == 3){
        PT_WAIT_UNTIL(pt, horizontal(m1,m2,loccounter));
      }
    }
    else{
      PT_WAIT_UNTIL(pt, input != locinput);
    }
  }
  PT_END(pt);
}

static int protothread2(struct pt *pt){
  static unsigned long time = 0;

  //motor type: 
  int motorDes = 1;
  bool player = false;
  //redesignate the defined pins as local variables so I can copy-paste 
  //the main body code between protothreads and just change a few numbers
  
  //define counter
  int loccounter = counter2;
  //encoder pins:
  int e1 = ENC_2_A;
  int e2 = ENC_2_B;
  //motor pins:
  int m1 = pwrPin2;
  int m2 = brkPin2;
  //(optional) bumper pins (only if player == false): 
  int b1 = bumper2;
  int b2 = bumper2;
  //
  int loclimit = limit1;
  //define the characters that can be used
  String locinput = input;
  int m;
  int locpos;
  int act;
  ///////////////////////////////////////////// setup done
  PT_BEGIN(pt);
  while (1){
    //segment the input line
    m = input.substring(0,1).toInt();
    locpos = locinput.substring(1,4).toInt();
    act = input.substring(4,5).toInt();
    if (motorDes == m){
      //we put the activation of the movement functions inside a wait until so that the 
      //thread sleeps and allows other threads to run while the movement code is running.
      //motor control code
      if (0<locpos && locpos < loclimit && player == false){
        PT_WAIT_UNTIL(pt, move_to(m1,m2,loccounter,locpos));
      }
      else if(player == false && locpos == 999){
        //I want this bit to be blocking
        calibrateBase(m1,m2);
        loclimit = calibrateEnd(m1,m2,motorDes);
      }
      //motor action code
      if (act == 1){
        PT_WAIT_UNTIL(pt, kick(m1,m2,loccounter));
      }
      else if (act == 2){
        PT_WAIT_UNTIL(pt, stand(m1,m2,loccounter));
      }
      else if (act == 3){
        PT_WAIT_UNTIL(pt, horizontal(m1,m2,loccounter));
      }
    }
    else{
      PT_WAIT_UNTIL(pt, input != locinput);
    }
  }
  PT_END(pt);
}

static int protothread3(struct pt *pt){
  static unsigned long time = 0;

  //motor type: 
  int motorDes = 2;
  bool player = true;
  //redesignate the defined pins as local variables so I can copy-paste 
  //the main body code between protothreads and just change a few numbers
  
  //define counter
  int loccounter = counter3;
  //encoder pins:
  int e1 = ENC_3_A;
  int e2 = ENC_3_B;
  //motor pins:
  int m1 = pwrPin3;
  int m2 = brkPin3;
  //(optional) bumper pins (only if player == false): 
  int b1 = bumper3;
  int b2 = bumper3;
  //
  int loclimit = limit2;
  //define the characters that can be used
  String locinput = input;
  int m;
  int locpos;
  int act;
  ///////////////////////////////////////////// setup done
  PT_BEGIN(pt);
  while (1){
    //segment the input line
    m = input.substring(0,1).toInt();
    locpos = locinput.substring(1,4).toInt();
    act = input.substring(4,5).toInt();
    if (motorDes == m){
      //we put the activation of the movement functions inside a wait until so that the 
      //thread sleeps and allows other threads to run while the movement code is running.
      //motor control code
      if (0<locpos && locpos < loclimit && player == false){
        PT_WAIT_UNTIL(pt, move_to(m1,m2,loccounter,locpos));
      }
      else if(player == false && locpos == 999){
        calibrateBase(m1,m2);
        loclimit = calibrateEnd(m1,m2,motorDes);
      }
      //motor action code
      if (act == 1){
        PT_WAIT_UNTIL(pt, kick(m1,m2,loccounter));
      }
      else if (act == 2){
        PT_WAIT_UNTIL(pt, stand(m1,m2,loccounter));
      }
      else if (act == 3){
        PT_WAIT_UNTIL(pt, horizontal(m1,m2,loccounter));
      }
    }
    else{
      PT_WAIT_UNTIL(pt, input != locinput);
    }
  }
  PT_END(pt);
}

static int protothread4(struct pt *pt){
  static unsigned long time = 0;

  //motor type: 
  int motorDes = 2;
  bool player = false;
  //redesignate the defined pins as local variables so I can copy-paste 
  //the main body code between protothreads and just change a few numbers
  
  //define counter
  int loccounter = counter4;
  //encoder pins:
  int e1 = ENC_4_A;
  int e2 = ENC_4_B;
  //motor pins:
  int m1 = pwrPin4;
  int m2 = brkPin4;
  //(optional) bumper pins (only if player == false): 
  int b1 = bumper4;
  int b2 = bumper4;
  //
  int loclimit = limit2;
  //define the characters that can be used
  String locinput = input;
  int m;
  int locpos;
  int act;
  ///////////////////////////////////////////// setup done
  PT_BEGIN(pt);
  while (1){
    //segment the input line
    m = input.substring(0,1).toInt();
    locpos = locinput.substring(1,4).toInt();
    act = input.substring(4,5).toInt();
    if (motorDes == m){
      //we put the activation of the movement functions inside a wait until so that the 
      //thread sleeps and allows other threads to run while the movement code is running.
      //motor control code
      if (0<locpos && locpos < loclimit && player == false){
        PT_WAIT_UNTIL(pt, move_to(m1,m2,loccounter,locpos));
      }
      else if(player == false && locpos == 999){
        calibrateBase(m1,m2);
        loclimit = calibrateEnd(m1,m2,motorDes);
      }
      //motor action code
      if (act == 1){
        PT_WAIT_UNTIL(pt, kick(m1,m2,loccounter));
      }
      else if (act == 2){
        PT_WAIT_UNTIL(pt, stand(m1,m2,loccounter));
      }
      else if (act == 3){
        PT_WAIT_UNTIL(pt, horizontal(m1,m2,loccounter));
      }
    }
    else{
      PT_WAIT_UNTIL(pt, input != locinput);
    }
  }
  PT_END(pt);
}

static int protothread5(struct pt *pt){
  static unsigned long time = 0;

  //motor type: 
  int motorDes = 3;
  bool player = true;
  //redesignate the defined pins as local variables so I can copy-paste 
  //the main body code between protothreads and just change a few numbers
  
  //define counter
  int loccounter = counter2;
  //encoder pins:
  int e1 = ENC_5_A;
  int e2 = ENC_5_B;
  //motor pins:
  int m1 = pwrPin5;
  int m2 = brkPin5;
  //(optional) bumper pins (only if player == false): 
  int b1 = bumper5;
  int b2 = bumper5;
  //
  int loclimit = limit3;
  //define the characters that can be used
  String locinput = input;
  int m;
  int locpos;
  int act;
  ///////////////////////////////////////////// setup done
  PT_BEGIN(pt);
  while (1){
    //segment the input line
    m = input.substring(0,1).toInt();
    locpos = locinput.substring(1,4).toInt();
    act = input.substring(4,5).toInt();
    if (motorDes == m){
      //we put the activation of the movement functions inside a wait until so that the 
      //thread sleeps and allows other threads to run while the movement code is running.
      //motor control code
      if (0<locpos && locpos < loclimit && player == false){
        PT_WAIT_UNTIL(pt, move_to(m1,m2,loccounter,locpos));
      }
      else if(player == false && locpos == 999){
        calibrateBase(m1,m2);
        loclimit = calibrateEnd(m1,m2,motorDes);
      }
      //motor action code
      if (act == 1){
        PT_WAIT_UNTIL(pt, kick(m1,m2,loccounter));
      }
      else if (act == 2){
        PT_WAIT_UNTIL(pt, stand(m1,m2,loccounter));
      }
      else if (act == 3){
        PT_WAIT_UNTIL(pt, horizontal(m1,m2,loccounter));
      }
    }
    else{
      PT_WAIT_UNTIL(pt, input != locinput);
    }
  }
  PT_END(pt);
}

static int protothread6(struct pt *pt){
  static unsigned long time = 0;

  //motor type: 
  int motorDes = 3;
  bool player = false;
  //redesignate the defined pins as local variables so I can copy-paste 
  //the main body code between protothreads and just change a few numbers
  
  //define counter
  int loccounter = counter6;
  //encoder pins:
  int e1 = ENC_6_A;
  int e2 = ENC_6_B;
  //motor pins:
  int m1 = pwrPin6;
  int m2 = brkPin6;
  //(optional) bumper pins (only if player == false): 
  int b1 = bumper6;
  int b2 = bumper6;
  //
  int loclimit = limit3;
  //define the characters that can be used
  String locinput = input;
  int m;
  int locpos;
  int act;
  ///////////////////////////////////////////// setup done
  PT_BEGIN(pt);
  while (1){
    //segment the input line
    m = input.substring(0,1).toInt();
    locpos = locinput.substring(1,4).toInt();
    act = input.substring(4,5).toInt();
    if (motorDes == m){
      //we put the activation of the movement functions inside a wait until so that the 
      //thread sleeps and allows other threads to run while the movement code is running.
      //motor control code
      if (0<locpos && locpos < loclimit && player == false){
        PT_WAIT_UNTIL(pt, move_to(m1,m2,loccounter,locpos));
      }
      else if(player == false && locpos == 999){
        calibrateBase(m1,m2);
        loclimit = calibrateEnd(m1,m2,motorDes);
      }
      //motor action code
      if (act == 1){
        PT_WAIT_UNTIL(pt, kick(m1,m2,loccounter));
      }
      else if (act == 2){
        PT_WAIT_UNTIL(pt, stand(m1,m2,loccounter));
      }
      else if (act == 3){
        PT_WAIT_UNTIL(pt, horizontal(m1,m2,loccounter));
      }
    }
    else{
      PT_WAIT_UNTIL(pt, input != locinput);
    }
  }
  PT_END(pt);
}

static int protothread7(struct pt *pt){
  static unsigned long time = 0;

  //motor type: 
  int motorDes = 4;
  bool player = true;
  //redesignate the defined pins as local variables so I can copy-paste 
  //the main body code between protothreads and just change a few numbers
  
  //define counter
  int loccounter = counter7;
  //encoder pins:
  int e1 = ENC_7_A;
  int e2 = ENC_7_B;
  //motor pins:
  int m1 = pwrPin7;
  int m2 = brkPin7;
  //(optional) bumper pins (only if player == false): 
  int b1 = bumper7;
  int b2 = bumper7;
  //
  int loclimit = limit4;
  //define the characters that can be used
  String locinput = input;
  int m;
  int locpos;
  int act;
  ///////////////////////////////////////////// setup done
  PT_BEGIN(pt);
  while (1){
    //segment the input line
    m = input.substring(0,1).toInt();
    locpos = locinput.substring(1,4).toInt();
    act = input.substring(4,5).toInt();
    if (motorDes == m){
      //we put the activation of the movement functions inside a wait until so that the 
      //thread sleeps and allows other threads to run while the movement code is running.
      //motor control code
      if (0<locpos && locpos < loclimit && player == false){
        PT_WAIT_UNTIL(pt, move_to(m1,m2,loccounter,locpos));
      }
      else if(player == false && locpos == 999){
        calibrateBase(m1,m2);
        loclimit = calibrateEnd(m1,m2,motorDes);
      }
      //motor action code
      if (act == 1){
        PT_WAIT_UNTIL(pt, kick(m1,m2,loccounter));
      }
      else if (act == 2){
        PT_WAIT_UNTIL(pt, stand(m1,m2,loccounter));
      }
      else if (act == 3){
        PT_WAIT_UNTIL(pt, horizontal(m1,m2,loccounter));
      }
    }
    else{
      PT_WAIT_UNTIL(pt, input != locinput);
    }
  }
  PT_END(pt);
}

static int protothread8(struct pt *pt){
  static unsigned long time = 0;

  //motor type: 
  int motorDes = 4;
  bool player = false;
  //redesignate the defined pins as local variables so I can copy-paste 
  //the main body code between protothreads and just change a few numbers
  
  //define counter
  int loccounter = counter8;
  //encoder pins:
  int e1 = ENC_8_A;
  int e2 = ENC_8_B;
  //motor pins:
  int m1 = pwrPin8;
  int m2 = brkPin8;
  //(optional) bumper pins (only if player == false): 
  int b1 = bumper8;
  int b2 = bumper8;
  //
  int loclimit = limit4;
  //define the characters that can be used
  String locinput = input;
  int m;
  int locpos;
  int act;
  ///////////////////////////////////////////// setup done
  PT_BEGIN(pt);
  while (1){
    //segment the input line
    m = input.substring(0,1).toInt();
    locpos = locinput.substring(1,4).toInt();
    act = input.substring(4,5).toInt();
    if (motorDes == m){
      //we put the activation of the movement functions inside a wait until so that the 
      //thread sleeps and allows other threads to run while the movement code is running.
      //motor control code
      if (0<locpos && locpos < loclimit && player == false){
        PT_WAIT_UNTIL(pt, move_to(m1,m2,loccounter,locpos));
      }
      else if(player == false && locpos == 999){
        calibrateBase(m1,m2);
        loclimit = calibrateEnd(m1,m2,motorDes);
      }
      //motor action code
      if (act == 1){
        PT_WAIT_UNTIL(pt, kick(m1,m2,loccounter));
      }
      else if (act == 2){
        PT_WAIT_UNTIL(pt, stand(m1,m2,loccounter));
      }
      else if (act == 3){
        PT_WAIT_UNTIL(pt, horizontal(m1,m2,loccounter));
      }
    }
    else{
      PT_WAIT_UNTIL(pt, input != locinput);
    }
  }
  PT_END(pt);
}

//protothreads finished /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  // Set encoder pins and attach interrupts
  //ENC1
  pinMode(ENC_1_A, INPUT_PULLUP);
  pinMode(ENC_1_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_1_A), [] () {read_encoder(ENC_1_A, ENC_1_B, counter1);}, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_1_B), [] () {read_encoder(ENC_1_A, ENC_1_B, counter1);}, CHANGE);
  //ENC2
  pinMode(ENC_2_A, INPUT_PULLUP);
  pinMode(ENC_2_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_2_A), [] () {read_encoder(ENC_2_A, ENC_2_B, counter2);}, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_2_B), [] () {read_encoder(ENC_2_A, ENC_2_B, counter2);}, CHANGE);
  //ENC3
  pinMode(ENC_3_A, INPUT_PULLUP);
  pinMode(ENC_3_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_3_A), [] () {read_encoder(ENC_3_A, ENC_3_B, counter3);}, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_3_B), [] () {read_encoder(ENC_3_A, ENC_3_B, counter3);}, CHANGE);
  //ENC4
  pinMode(ENC_4_A, INPUT_PULLUP);
  pinMode(ENC_4_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_4_A), [] () {read_encoder(ENC_4_A, ENC_4_B, counter4);}, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_4_B), [] () {read_encoder(ENC_4_A, ENC_4_B, counter4);}, CHANGE);
  //ENC5
  pinMode(ENC_5_A, INPUT_PULLUP);
  pinMode(ENC_5_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_5_A), [] () {read_encoder(ENC_5_A, ENC_5_B, counter5);}, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_5_B), [] () {read_encoder(ENC_5_A, ENC_5_B, counter5);}, CHANGE);
  //ENC6
  pinMode(ENC_6_A, INPUT_PULLUP);
  pinMode(ENC_6_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_6_A), [] () {read_encoder(ENC_6_A, ENC_6_B, counter6);}, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_6_B), [] () {read_encoder(ENC_6_A, ENC_6_B, counter6);}, CHANGE);
  //ENC7
  pinMode(ENC_7_A, INPUT_PULLUP);
  pinMode(ENC_7_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_7_A), [] () {read_encoder(ENC_7_A, ENC_7_B, counter7);}, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_7_B), [] () {read_encoder(ENC_7_A, ENC_7_B, counter7);}, CHANGE);
  //ENC8
  pinMode(ENC_8_A, INPUT_PULLUP);
  pinMode(ENC_8_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_8_A), [] () {read_encoder(ENC_8_A, ENC_8_B, counter8);}, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_8_B), [] () {read_encoder(ENC_8_A, ENC_8_B, counter8);}, CHANGE);

  //define the bumper pins and interrupts 
  //bumper1
  pinMode(bumper1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(bumper1), [] () {pressed(bumper1);}, RISING); //rising means only detect when it goes from low to high
  //bumper2
  pinMode(bumper2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(bumper2), [] () {pressed(bumper2);}, RISING);
  //bumper3
  pinMode(bumper3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(bumper3), [] () {pressed(bumper3);}, RISING);
  //bumper4
  pinMode(bumper4, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(bumper4), [] () {pressed(bumper4);}, RISING);
  //bumper5
  pinMode(bumper5, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(bumper5), [] () {pressed(bumper5);}, RISING);
  //bumper6
  pinMode(bumper6, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(bumper6), [] () {pressed(bumper6);}, RISING);
  //bumper7
  pinMode(bumper7, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(bumper7), [] () {pressed(bumper7);}, RISING);
  //bumper8
  pinMode(bumper8, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(bumper8), [] () {pressed(bumper8);}, RISING);

  //define motor pins
  //motor 1
  pinMode(pwrPin1, OUTPUT);
  pinMode(brkPin1, OUTPUT);
  //motor 2
  pinMode(pwrPin2, OUTPUT);
  pinMode(brkPin2, OUTPUT);
  //motor 3
  pinMode(pwrPin3, OUTPUT);
  pinMode(brkPin3, OUTPUT);
  //motor 4
  pinMode(pwrPin4, OUTPUT);
  pinMode(brkPin4, OUTPUT);
  //motor 5
  pinMode(pwrPin5, OUTPUT);
  pinMode(brkPin5, OUTPUT);
  //motor 6
  pinMode(pwrPin6, OUTPUT);
  pinMode(brkPin6, OUTPUT);
  //motor 7
  pinMode(pwrPin7, OUTPUT);
  pinMode(brkPin7, OUTPUT);
  //motor 8
  pinMode(pwrPin8, OUTPUT);
  pinMode(brkPin8, OUTPUT);
}

void loop() {
  if (Serial.available()){
      input = Serial.readStringUntil('\n');
    }
  //execute the protothreads
  protothread1(&pt1);
  protothread2(&pt2);
  protothread3(&pt3);
  protothread4(&pt4);
  protothread5(&pt5);
  protothread6(&pt6);
  protothread7(&pt7);
  protothread8(&pt8);

}
