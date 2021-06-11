//hatY = 0 -> UPside
//hatY = 255 -> DOWNside
//hatX = 0 -> LEFTside
//hatX = 255 -> RIGHTside
//dir = HIGH -> Clockwise
//dir = LOW -> AntiClockwise
//arr = next-hop direction
//directions : 1 = left, 2 = right, 3 = up, 4 = down, 0 = none
//pneumatic position : tz1 , tz3 , tz2

//----------------Locomotion-------------------
  #define left_pwm 2
  #define right_pwm 3
  #define up_pwm 4
  #define down_pwm 5
  #define left_dir 22
  #define right_dir 23
  #define up_dir 24
  #define down_dir 25
//--------------------------------------------

//----------------Pneumatics------------------
  #define pneu_tz1_up 13
  #define pneu_tz1_down 14
  #define pneu_tz2_up 15
  #define pneu_tz2_down 16
  #define pneu_tz3_up 29
  #define pneu_tz3_down 30
//--------------------------------------------

//----------------LSA-08----------------------
  #define rx1 0
  #define tx1 1
  #define serialEn1 6
  #define rx2 9
  #define tx2 10
  #define serialEn2 7
//--------------------------------------------

//----------------LIMIT SWITCH----------------
  #define limit_tz1 11
  #define limit_tz2 12
  #define limit_tz3 28
//--------------------------------------------

//----------------PROXIMITY-------------------
  #define proximity_tz1 33
  #define proximity_tz2 34
  #define proximity_tz3 35
//--------------------------------------------

// Sharp_ir = A17(pin 36)

//----------------Flash Light-------------------
  #define flash 8
//--------------------------------------------

// access element as [source][Destination][0 for next-hop/ 1 for direction]

int arr[7][7][2] = {  //  dst0  dst1  dst2  dst3  dst4  dst5  dst6
                    {    {0,0},{1,2},{1,2},{1,2},{1,2},{1,2},{1,2}    }, //Source 0
                    {    {0,1},{1,0},{2,4},{2,4},{2,4},{2,4},{2,4}    }, //Source 1
                    {    {1,3},{1,3},{2,0},{3,2},{4,4},{4,4},{4,4}    }, //Source 2
                    {    {2,1},{2,1},{2,1},{3,0},{2,1},{2,1},{2,1}    }, //Source 3
                    {    {2,3},{2,3},{2,3},{2,3},{4,0},{5,2},{5,2}    }, //Source 4
                    {    {4,1},{4,1},{4,1},{4,1},{4,1},{5,0},{6,2}    }, //Source 5
                    {    {5,1},{5,1},{5,1},{5,1},{5,1},{5,1},{6,0}    }  //Source 6
                  };  //  NH,D  NH,D  NH,D  NH,D  NH,D  NH,D  NH,D
class Map{
  int Source = 0, Destination = 1, NextHop = 0, Direction = 0, JunctionFlag = 0;
  int lower_tz1 = 7,lower_tz2 = 15,lower_tz3 = 30,upper_tz1 = 24,upper_tz2 = 45,upper_tz3 = 80;
  int tz1 = 0, tz2 = 0, tz3 = 0, tz1_flag = 0, tz2_flag = 0, tz3_flag = 0, i = 0
  };
int Speed = 40, LeftSpeed = 40, RightSpeed = 40;
int SetPoint = 35, MaxSpeed = 70, lastError1 = 0, lastError2 = 0;
int Rotate180 = 0, Rotate360 = 0;
class PID{
float kp1 = 1.2, kd1 = 1.2*50, kp2 = 1.2, kd2 = 1.2*50;
};

void setup() 
{
  delay(5000);
  
  //---------Initialize motors----------
    pinMode(left_pwm,OUTPUT);
    pinMode(right_pwm,OUTPUT);
    pinMode(up_pwm,OUTPUT);
    pinMode(down_pwm,OUTPUT);
    pinMode(left_dir,OUTPUT);
    pinMode(right_dir,OUTPUT);
    pinMode(up_dir,OUTPUT);
    pinMode(down_dir,OUTPUT); 
  //-------------------------------------
  
  //--------Initialize pneumatics--------
    pinMode(pneu_tz1_up,OUTPUT);
    pinMode(pneu_tz1_down,OUTPUT);
    pinMode(pneu_tz2_up,OUTPUT);
    pinMode(pneu_tz2_down,OUTPUT);
    pinMode(pneu_tz3_up,OUTPUT);
    pinMode(pneu_tz3_down,OUTPUT);
  //-------------------------------------

  //--------Initialize LSA-08------------
    pinMode(serialEn1,OUTPUT);
    pinMode(serialEn2,OUTPUT);
    digitalWrite(serialEn1,HIGH);
    digitalWrite(serialEn2,HIGH);
  //-------------------------------------

  //--------Initialize LIMIT SWITCH------
    pinMode(limit_tz1,INPUT);
    pinMode(limit_tz2,INPUT);
    pinMode(limit_tz3,INPUT);
  //-------------------------------------

  //--------Initialize Proximity---------
    pinMode(proximity_tz1,INPUT);
    pinMode(proximity_tz2,INPUT);
    pinMode(proximity_tz3,INPUT);
  //-------------------------------------

  //--------Flash Light------------------
    pinMode(flash,OUTPUT);
    digitalWrite(flash,HIGH);
  //-------------------------------------

  //---------SharpIr---------------------
    pinMode(A17,INPUT);
  //-------------------------------------
  
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial2.begin(115200);
  
  Pause();  
  Move();
  _init();
  Destination = 2;
  Move();

} // end of setup

void loop() 
{
  if((Source == 2 || Source == 4) && Rotate180 == 1)
  {
    Rotate_180();
    Rotate180 = 0;
  }
  Destination = Wait();
  if(Source != Destination)
  {
    Move();
    Rotate_180();
    Rotate360 = 0;
    Throw();
    _init();
    Destination = 4;
    Move();
  }
} // end of loop

void _init()
{
  Speed = 70;
  MaxSpeed = 120;
  lastError1 = 0;
  lastError2 = 0;
}

void Move() // this function will move bot from source to Destination
{
  while(Source!=Destination)
  {
    if(isJunction() && JunctionFlag>15) 
    {
      Pause();
      delay(200);
      Rebound();
      Pause();
      delay(200);
      _init();
      Source = NextHop;
      JunctionFlag = 0;
    }
    
    if(!isJunction())
    {
      JunctionFlag++;
    }
      
    NextHop = arr[Source][Destination][0];
    Direction = arr[Source][Destination][1];

    if(Rotate180 == 0 && Rotate360 == 1 && Direction != 0)
    {
      Direction = (Direction%2 == 0) ? (Direction - 1) : (Direction + 1);
    }
    
    switch(Direction)
    {
      case 0 : Pause();
               break;
      case 1 : PID1();
               Left();
               break;
      case 2 : PID1();
               Right();
               break;
      case 3 : PID2();
               Up();
               break;
      case 4 : PID2();
               Down();
               break;
    }
  }
  if(Source == 2 || Source == 4)
  {
    Rotate180 = 1;
    Rotate360 = 1;
  }
} // end of move

void Rebound()
{
  LeftSpeed = 40;
  RightSpeed = 40;
  
  while(!isJunction())
  {
    switch(Direction)
    {
      case 0 : Pause();
               break;
      case 1 : Right();
               break;
      case 2 : Left();
               break;
      case 3 : Down();
               break;
      case 4 : Up();
               break;
    }
  }
  Pause();
} // end of rebound

int Wait() // this function will wait until shuttlecock is loaded & return new Destination based on which arm detects the shuttlecock
{
  int val = 27.728 * pow(map(analogRead(A17), 0, 1023, 0, 3500)/1000.0, -1.2045);
  int tz1 = (2 - digitalRead(proximity_tz1)) + ((val > lower_tz1 && val < upper_tz1) ? 1 : 0);
  int tz2 = (2 - digitalRead(proximity_tz2)) + ((val > lower_tz2 && val < upper_tz2) ? 1 : 0);
  int tz3 = (2 - digitalRead(proximity_tz3)) + ((val > lower_tz3 && val < upper_tz3) ? 1 : 0);
  
  if((tz1 > tz2) && (tz1 > tz3) && (tz1 >= 2))
  {
    tz1_flag++;
    tz2_flag = 0;
    tz3_flag = 0;
    delay(10);
  }
  if((tz2 > tz1) && (tz2 > tz3) && (tz2 >= 2))
  {
    tz1_flag = 0;
    tz2_flag++;
    tz3_flag = 0;
    delay(10);
  }
  if((tz3 > tz2) && (tz3 > tz1) && (tz3 >= 2))
  {
    tz1_flag = 0;
    tz2_flag = 0;
    tz3_flag++;
    delay(10);
  }

  if(tz1_flag > 250)
  {
    tz1_flag = 0;
    return 3;
  }
  else if(tz2_flag > 250)
  {
    tz2_flag = 0;
    return 5;
  }
  else if(tz3_flag > 250)
  {
    tz3_flag = 0;
    //return 6;
  }
  else
    return Source;
} // end of wait

bool isJunction() // returns 1 if junction detected
{
  digitalWrite(serialEn1,LOW);
  while(Serial1.available() <= 0);
  int positionVal1 = Serial1.read();
  digitalWrite(serialEn1,HIGH);
  
  digitalWrite(serialEn2,LOW);
  while(Serial2.available() <= 0);
  int positionVal2 = Serial2.read();
  digitalWrite(serialEn2,HIGH);
  
  if((positionVal1 >= 20 && positionVal1 <=50) && (positionVal2 >= 20 && positionVal2 <=50))
    return 1;
  else
    return 0;
} // end of isJunction

void Pause()
{
  analogWrite(left_pwm,0);
  analogWrite(right_pwm,0);
  analogWrite(up_pwm,0);   
  analogWrite(down_pwm,0);
} // end of pause

void Left()
{
  digitalWrite(up_dir,LOW);
  digitalWrite(down_dir,LOW);
  analogWrite(up_pwm,LeftSpeed);
  analogWrite(down_pwm,RightSpeed);
} // end of left

void Right()
{
  digitalWrite(up_dir,HIGH);
  digitalWrite(down_dir,HIGH);
  analogWrite(up_pwm,LeftSpeed);
  analogWrite(down_pwm,RightSpeed);
} // end of Right

void Up()
{
  digitalWrite(left_dir,LOW);
  digitalWrite(right_dir,HIGH);
  analogWrite(left_pwm,LeftSpeed);
  analogWrite(right_pwm,RightSpeed);
} // end of Up

void Down()
{
  digitalWrite(left_dir,HIGH);
  digitalWrite(right_dir,LOW);
  analogWrite(left_pwm,LeftSpeed);
  analogWrite(right_pwm,RightSpeed);
} // end of Down

void PID1()
{
  int positionVal = 255;
  
  digitalWrite(serialEn1,LOW);
  while(Serial1.available() <= 0);
  positionVal = Serial1.read();
  digitalWrite(serialEn1,HIGH);

  if(positionVal == 255) 
  {
    LeftSpeed = 0;
    RightSpeed = 0;
  }
  else 
  {
    int error = positionVal - SetPoint;
    int motorSpeed = kp1 * error + kd1 * (error - lastError1);
    lastError1 = error;

    LeftSpeed = Speed - motorSpeed;
    RightSpeed = Speed + motorSpeed;

    if(RightSpeed > MaxSpeed) RightSpeed = MaxSpeed;
    if(LeftSpeed > MaxSpeed) LeftSpeed = MaxSpeed;

    if(RightSpeed < 0) RightSpeed = 0;
    if(LeftSpeed < 0) LeftSpeed = 0;
  }
} // end of PID1

void PID2()
{
  int positionVal = 255;
  
  digitalWrite(serialEn2,LOW);
  while(Serial2.available() <= 0);
  positionVal = Serial2.read();
  digitalWrite(serialEn2,HIGH);

  if(positionVal == 255) 
  {
    LeftSpeed = 0;
    RightSpeed = 0;
  }
  else 
  {
    int error = positionVal - SetPoint;
    int motorSpeed = kp2 * error + kd2 * (error - lastError2);
    lastError2 = error;

    LeftSpeed = Speed - motorSpeed;
    RightSpeed = Speed + motorSpeed;

    if(RightSpeed > MaxSpeed) RightSpeed = MaxSpeed;
    if(LeftSpeed > MaxSpeed) LeftSpeed = MaxSpeed;

    if(RightSpeed < 0) RightSpeed = 0;
    if(LeftSpeed < 0) LeftSpeed = 0;
  }
} // end of PID1

void Rotate_180()
{
  int JuncCount = 0;
  int RotateFlag = 0;
  digitalWrite(up_dir,HIGH);
  digitalWrite(down_dir,LOW);
  digitalWrite(left_dir,LOW);
  digitalWrite(right_dir,LOW);
  
  while(JuncCount < 2)
  { 
    digitalWrite(serialEn2,LOW);
    while(Serial2.available() <= 0);
    int positionVal3 = Serial2.read();
    digitalWrite(serialEn2,HIGH);   
    
    if(positionVal3 >= 20 && positionVal3 <=50 && RotateFlag > 20)
    {
      JuncCount++;
      RotateFlag = 0;
    }
    else if(positionVal3 < 20 || positionVal3 >50)
      RotateFlag++;
    
    analogWrite(up_pwm,25);
    analogWrite(down_pwm,25);
    analogWrite(right_pwm,25);
    analogWrite(left_pwm,25);
  }
  Pause();
}

void Throw() // this function will throw the shuttlecock based on source
{
  LeftSpeed = 30;
  RightSpeed = 30;

  if(Source == 3)
  {
    Right();
    delay(700);
    Pause();
    Direction = 2;
    flash_flash();
    digitalWrite(pneu_tz1_down,HIGH);
    delay(1000);
    digitalWrite(pneu_tz1_down,LOW);
    delay(1000);
    digitalWrite(pneu_tz1_up,HIGH);
    delay(200);
    digitalWrite(pneu_tz1_up,LOW);
    Rebound();
  }
  else if(Source == 5)
  {
    Left();
    delay(850);
    Pause();
    Direction = 1;
    flash_flash();
    digitalWrite(pneu_tz2_down,HIGH);
    delay(1000);
    digitalWrite(pneu_tz2_down,LOW);
    delay(1000);
    digitalWrite(pneu_tz2_up,HIGH);
    delay(200);
    digitalWrite(pneu_tz2_up,LOW);
  }
  else if(Source == 6)
  {
    flash_flash();
    digitalWrite(pneu_tz3_down,HIGH);
    delay(1000);
    digitalWrite(pneu_tz3_down,LOW);
    delay(1000);
    digitalWrite(pneu_tz3_up,HIGH);
    delay(200);
    digitalWrite(pneu_tz3_up,LOW);
  }
  digitalWrite(flash,HIGH);
} // end of throw

void flash_flash()
{
    while(i <= 15)
    {
      digitalWrite(flash,LOW);
      delay(50);
      digitalWrite(flash,HIGH);
      delay(50);
      i++;
    }
    i = 0;
    digitalWrite(flash,LOW);
  }
