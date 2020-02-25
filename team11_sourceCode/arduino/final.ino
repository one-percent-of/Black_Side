// follow 함수는 http://fribot.blog.me/220454988902 사이트 참고했습니다.




#include <Servo.h> // 모터는 반시계방향이 +방향

#define trigPin_l1 2 // Trigger Pin of left1
#define echoPin_l1 3 // Echo Pin of left1
//letf1
#define trigPin_l2 4 // Trigger Pin of left2
#define echoPin_l2 5 // Echo Pin of left2
//left2
#define trigPin_r1 6 // Trigger Pin of right1
#define echoPin_r1 7 // Echo Pin of right1
//right1
#define trigPin_r2 8 // Trigger Pin of right2
#define echoPin_r2 9 // Echo Pin of right2
//right2

Servo mid_servo; // servo motor of camera
Servo left_servo; // servo motor of left ultrasonic sensors
Servo right_servo; // servo motor of right ultrasonic sensors

int mid_init = 80; // 0~160도 회전, 가운데 80도
int left_init = 45; // 0~90도 회전, 가운데 45도
int right_init = 45; // 0~90도 회전, 가운데 45도
int mid_angle = 80; // 0~160도 회전, 가운데 80도
int left_angle = 45; // 0~90도 회전, 가운데 45도
int right_angle = 45; // 0~90도 회전, 가운데 45도
int limit_distance = 15; // 초음파센서가 측정할 수 있는 최대거리 제한 15cm

int duration_l1,duration_l2,distance_l1,distance_l2; //duration:초음파가 갔다오는데 왕복으로 걸린시간, distance : duration을 2로 나누어 편도로 만들어 초음파속도 나누어주면 초음파센서와 물체가의 거리가 구해진다.
int duration_r1,duration_r2,distance_r1,distance_r2;
int find_l;
int find_r;

int left_val = 0; // 왼쪽 진동센서
int right_val = 0; // 오른쪽 진동센서

int periods = 15000; // 초음파센서가 거리측정하는 최대시간 제한 15초
float pre_time, current_time; // periods만큼 제어하기 위해 time을 받아오기 위한 변수

void setup() {
  Serial.begin (9600);
  //모터 셋팅
  mid_servo.attach(10);
  left_servo.attach(11);
  right_servo.attach(12);

  //모터값 초기화
  mid_servo.write(mid_init);
  left_servo.write(left_init);
  right_servo.write(right_init);
  delay(2000);
}

void loop() {
  
  left_val = analogRead(0);
  right_val = analogRead(1);
  //진동값 받음

  Serial.print(left_val);
  Serial.print("L");
  Serial.print(right_val);
  Serial.println("R");

  if(left_val < right_val) {
    mid_angle = 45;
  }
  if(left_val > right_val) {
    mid_angle = 135;
  }

  //왼쪽 진동값이 클경우
  pre_time = millis();
  current_time = millis();
  delay(100);
  if(left_val > 20  && (left_val > right_val)){
     while((current_time - pre_time) <= periods){ //10초동안 서보모터 움직임
      //eCube에 들어오는 버퍼값을 0으로 해주기 위함
      Serial.print(0);
      Serial.print("L");
      Serial.print(0);
      Serial.println("R");
      Serial.print(0);
      Serial.print("L");
      Serial.print(0);
      Serial.println("R");
      current_time = millis();
      //초음파 측정
      pinMode(trigPin_l1, OUTPUT);
      digitalWrite(trigPin_l1, LOW);
      delayMicroseconds(2);          
      digitalWrite(trigPin_l1, HIGH);   
      delayMicroseconds(10);       
      digitalWrite(trigPin_l1, LOW);
      pinMode(echoPin_l1, INPUT);
      duration_l1 = pulseIn(echoPin_l1, HIGH);
      distance_l1 = (duration_l1)/2 /29.1;
      delay(100);
      pinMode(trigPin_l2, OUTPUT);
      digitalWrite(trigPin_l2, LOW);  
      delayMicroseconds(2);          
      digitalWrite(trigPin_l2, HIGH);   
      delayMicroseconds(10);       
      digitalWrite(trigPin_l2, LOW);
      pinMode(echoPin_l2, INPUT);
      duration_l2 = pulseIn(echoPin_l2, HIGH);
      distance_l2 = (duration_l2)/2 /29.1;
      delay(100);
      //Serial.print("left of left = ");
      //Serial.println(distance_l1);
      //Serial.print("right of left = ");
      //Serial.println(distance_l2);
      followLeft();
      }
  }

  //오른쪽 진동값이 클경우
  pre_time = millis();
  current_time = millis();
  delay(100);
  if(right_val > 20&& (right_val > left_val)){  
    while((current_time - pre_time) <= periods){//10초동안 서보모터 움직임
      //eCube에 들어오는 버퍼값을 0으로 해주기 위함
      Serial.print(0);
      Serial.print("L");
      Serial.print(0);
      Serial.println("R");
      Serial.print(0);
      Serial.print("L");
      Serial.print(0);
      Serial.println("R");
      current_time = millis();
      //초음파 측정
      pinMode(trigPin_r1, OUTPUT);
      digitalWrite(trigPin_r1, LOW);
      delayMicroseconds(2);          
      digitalWrite(trigPin_r1, HIGH);   
      delayMicroseconds(10);       
      digitalWrite(trigPin_r1, LOW);
      pinMode(echoPin_r1, INPUT);
      duration_r1 = pulseIn(echoPin_r1, HIGH);
      distance_r1 = (duration_r1)/2 /29.1;
      delay(100);
      pinMode(trigPin_r2, OUTPUT);
      digitalWrite(trigPin_r2, LOW);  
      delayMicroseconds(2);          
      digitalWrite(trigPin_r2, HIGH);   
      delayMicroseconds(10);       
      digitalWrite(trigPin_r2, LOW);
      pinMode(echoPin_r2, INPUT);
      duration_r2 = pulseIn(echoPin_r2, HIGH);
      distance_r2 = (duration_r2)/2 /29.1;
      delay(100);
      //Serial.print("left of right = ");
      //Serial.println(distance_r1);
      //Serial.print("right of right = ");
      //Serial.println(distance_r2);
      followRight();
     }
   }
   
  mid_servo.write(mid_init);
  left_servo.write(left_init);
  right_servo.write(right_init);
}



// http://fribot.blog.me/220454988902 사이트 참고했습니다.

// 왼쪽과 가운데 모터를 돌리기 위한 함수
void followLeft() {

  if((distance_l1 <= limit_distance) || (distance_l2 <= limit_distance)){
    if(distance_l1 + 2 < distance_l2){
      left_angle = left_angle + 2;
      mid_angle = mid_angle + 3;
    }
    if(distance_l2 + 2 < distance_l1){
      left_angle = left_angle - 2;
      mid_angle = mid_angle - 3;
    } 
  }
  if(left_angle > 90){
    left_angle = 90;
  }
  if(mid_angle > 160){
    mid_angle = 160;
  }
  if(left_angle < 0 ){
    left_angle = 0;
  }
  if(mid_angle < 0 ){
    mid_angle = 0;
  }
  
  left_servo.write(left_angle);
  //Serial.print("left_angle = ");
  //Serial.println(left_angle);
  
  delay(20);
  mid_servo.write(mid_angle);
  //Serial.print("mid_angle = ");
  //Serial.println(mid_angle);
}

// 오른쪽과 가운데 모터를 돌리기 위한 함수
void followRight() {

  if((distance_r1 <= limit_distance) || (distance_r2 <= limit_distance)){
    if(distance_r1 + 2 < distance_r2){
      right_angle = right_angle + 2;
      mid_angle = mid_angle + 3;
    }
    if(distance_r2 + 2 < distance_r1){
      right_angle = right_angle - 2;
      mid_angle = mid_angle - 3;
    } 
  }
  if(right_angle > 90){
    right_angle = 90;
  }
  if(mid_angle > 160){
    mid_angle = 160;
  }
  if(right_angle < 0 ){
    right_angle = 0;
  }
  if(mid_angle < 0 ){
    mid_angle = 0;
  }
  
  //Serial.print("right_angle = ");
  //Serial.println(right_angle);
  right_servo.write(right_angle);

  delay(20);
  //Serial.print("mid_angle = ");
  //Serial.println(mid_angle);
  mid_servo.write(mid_angle);
}

// 공식을 만들어 mid motor의 각도를 구하기 위한 함수, 그러나 이렇게 찾은 각도만큼 모터가 회전하지 않아서 구현하지 못한 부분으로 남겨뒀다.
/*void findMidangle() {
  if(left_val > right_val) {
    mid_angle = 135;
    if(distance_l1 <= limit_distance && distance_l1 >= 0) {
      find_l = distance_l1;
    }
    else if(distance_l2 <= limit_distance && distance_l2 >= 0) {
      find_l = distance_l2;
    }
  
    mid_angle = atan((find_l*sin(left_angle)+20)/(find_l*cos(left_angle)));
    mid_angle = mid_angle*180/PI + 90;
  }
  else {
    mid_angle = 45;
    if(distance_r1 <= limit_distance && distance_r1 >= 0) {
      find_r = distance_r1;
    }
    else if(distance_r2 <= limit_distance && distance_r2 >= 0) {
      find_r = distance_r2;
    }
    
    mid_angle = atan((find_r*sin(right_angle))/(find_r*cos(right_angle)+20));
    mid_angle = mid_angle*180/PI;
  }
}*/

// 맨처음에 세웠던 공식으로 차량의 앞과 뒤에 초음파 센서를 달아 mid motor의 각도를 찾고자 했다. 하지만 물체의 측정 범위가 작다는 단점이 있어서 쓰지 않았다.
/*
float distance1 ,distance2, duration1, duration2; // Duration used to calculate distance, distance1 : object and ultrasonic sensor2, distance2 : object and ultrasonic sensor1
float distance3 = 10, distance4 = 10; // distance3 : servo motor and ultrasonic sensor2, distance4 : servo motor and ultrasonic sensor1
float h, s, r, r2; //h:삼각형의 높이, s:세변을 알 때 삼각형의 넓이를 구할 때 쓰는 변수, r:전체삼각형넓이, r2=부분삼각형넓이
float t1, t2;
float theta;

    // servo motor의 theta를 구하기 위한 공식
    s = (distance1 + distance2 + (distance3 + distance4)) / 2;
    r = sqrt(s*(s-distance1)*(s-distance2)*(s-(distance3 + distance4)));
    r2=(r*distance4/(distance3+distance4));
    h=2*r2/distance4;
    t2=sqrt((distance2*distance2)-(h*h));
    t1=distance4-t2;
    theta = atan(h/t1)*180/PI;
    */
