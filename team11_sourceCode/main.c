#include <stdio.h> 
#include <stdlib.h> 
#include <termios.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <pthread.h> 
#include <time.h> 
#include <ctype.h> 
#include <linux/fd.h> 
#include <fcntl.h> 
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <arpa/inet.h>
#include <sys/stat.h>

#include "bitmap.h"
#include "dipswtest.h"
#include "buzzertest.h"
#include "keytest.h"
#include "touchapp.h" // touch
#include "fndtest.h" //7seg
#include "tlcdtest.h" // day, record
#include "mledtest.h" //<< , >>
#include "ledtest.h" //0000000
#include "cledtest.h" //color
#include "oledtest.h" //oled

#define BAUDRATE B9600      /* baud rate */ 
#define ARUDUINO_DEV_NAME "/dev/ttyACM0"   /*  device */ 
#define BUFFERSIZE 256  
#define TRUE 1
#define FALSE 0
#define DATA_SIZE 1

//scereen
#define screen_width = 1280;
#define screen_height = 800;
#define SCREEN "bitLCD.bmp"
#define MANUAL "bitManual.bmp"
#define NORMAL "carNormal.bmp"
#define LEFT "carLeft.bmp"
#define RIGHT "carRight.bmp"


//oled setting
#define OLED_MODE_WRITE 0
#define OLED_MODE_READ 1
#define OLED_MODE_CMD 2
#define OLED_MODE_RESET	3
#define OLED_MODE_NORMAL 4
#define OLED_MODE_INIT 7

#define CameraViewX 0
#define CameraViewY 200 //카메라 출력위치


pthread_mutex_t mled_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t mled_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t buzzer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buzzer_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t led_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t led_cond = PTHREAD_COND_INITIALIZER;

struct fb_var_screeninfo fbvar;
struct fb_fix_screeninfo fbfix;

//공유가 필요한 변수
int dip_state;
int arduino_fd = 0;
int val_l = 0;
int val_r = 0;
int v_val= 0;
int pre_v_val =0;
int v_count;
int check_l = 0;
int check_r = 0;
int password = 4412; //저장된 패스워드
int enter_pass;
int pass_check;
int key_lock=0;
int sys_init =0;
int  mled_init_flag =0;
int  f_init_flag =0;

//fd
int fnd_fd;
int led_fd;
int key_fd;
int dip_fd;
int mled_fd;
int buzzer_fd;
int tlcd_fd;
int cled_fd;
int oled_fd;
int touch_fd;
int bit_fd;

char nettmp[BUFFERSIZE] = { 0, };
unsigned char* pfbmap;
unsigned char* cfbmap;

void init();
void start();
int Arduino(int *_fd);
void* DeviceThrd(void *ptr);
void* fndThrd(void *ptr);
void* ledThrd(void *ptr);
void* mledThrd(void *ptr);
void* keyThrd(void *ptr);
void* touchThrd(void* ptr);
void* network(void* ptr);
void *buzzerThrd(void* ptr);


pthread_t threadpool[7]; // thread_pool

void main(void) {
	init();
	while(1) {
		
		start();
	}
}

void init() { // 파일들을 열어주고 스레드 세팅
	


	dip_fd = openDipsw();  //dip switch
	cled_fd = opencled(); //cled
	tlcd_fd = opentlcd();  //text lcd
   	mled_fd = openMled(); //dot matrix
	oled_fd = openOledDev(); //oled
   	fnd_fd = openFnd(); // sevensegment
	led_fd = openLed();  //bus led
	key_fd = openKeypad();  //key pad
	buzzer_fd = openBuzzer();  //buzzer 
	touch_fd = openTouch();
	bit_fd = openBMP();  /// add
	
	usleep(10000);
	oledtest(OLED_MODE_RESET); //reset
	usleep(10000);
   	oledtest(OLED_MODE_INIT);  //init
	usleep(10000);
	usleep(10000);
   	oledtest(OLED_MODE_NORMAL);  //normal image
	usleep(10000);
	usleep(10000);
	usleep(10000);

	pthread_create(&threadpool[0], NULL, DeviceThrd, (void*)0); //dipsw
	pthread_detach(threadpool[0]);
	pthread_create(&threadpool[1], NULL, ledThrd, (void*)0); //busled 
	pthread_detach(threadpool[1]);
	pthread_create(&threadpool[2], NULL, fndThrd, (void*)0); //sevensegment 
	pthread_detach(threadpool[2]);
	pthread_create(&threadpool[3], NULL, mledThrd, (void*)0);//dot matrix 
	pthread_detach(threadpool[3]);
	pthread_create(&threadpool[4], NULL, keyThrd, (void*)0); //key pad; 
	pthread_detach(threadpool[4]);
	pthread_create(&threadpool[5], NULL, touchThrd, (void*)0); //touch 
	pthread_detach(threadpool[5]);
	pthread_create(&threadpool[6], NULL, buzzerThrd, (void*)0); //buzzer 
	pthread_detach(threadpool[6]);

	
}



void start() {
	
	time_t startTime = 0, endTime = 0;
    float gap;
    Arduino(&arduino_fd);
    memset(nettmp, 0, BUFFERSIZE);
     
    gap = 0;
    mled_init_flag =0; //mled 튀는값 잡아주기 위함
    int i;
    int entcheck; // 초기에 들어오는 아두이노의 쓰래기값 정리 위함
    int parse_l=0; // 파싱을 위해서
    val_l = 0;
    val_r = 0;
    v_val= 0;
    v_count=0;
    check_l = 0; //진동위치 확인 
    check_r = 0; //진동위치 확인
    pre_v_val =0; //새로들어온 값과 저장된 값 비교를 위함
    pass_check=0; // 비밀번호가 맞다면 1
    cledtest(0, 0, 50, 0); // 사고가 나지 않았다면 초록불
    tlcdtest('c',1,1); // tlcd 클리어
    tlcdtest('w',2,1); // tlcd에 시스템 동작 확인 메세지 출력
    f_init_flag =0; 
    
	//그림 사이즈
    int mem_size = 1280 * 800 * 4;
    pfbmap = (unsigned char*)mmap(0, mem_size, PROT_READ | PROT_WRITE, MAP_SHARED, bit_fd, 0);
	//카메라 버퍼 사이즈
    int camera_mem_size = 1280 * 960 * 4;
    cfbmap = (unsigned char*)mmap(0, camera_mem_size, PROT_READ | PROT_WRITE, MAP_SHARED, bit_fd, 0);

    //터치스크린에 띄울 사진
    bitmaptest(SCREEN, pfbmap, 640, 0);
	
    while (1) {
		startTime = clock(); 
		tlcdtest('w',1,1); //tlcd에 현재시간 출력
        if (gap > 2) startTime = clock();  
        char buffer[BUFFERSIZE] = { 0, };
        int readIndex = 0;
        
        readIndex = read(arduino_fd, buffer, BUFFERSIZE); //아두이노에서 센서 값을 읽어서 버퍼에 저장
        printf("read buffer = %s\n" , buffer); 
        entcheck = 0;
        int cnt = 0;
        char tmp[10] = { 0, };
        usleep(1000);
        
        
        for (i = 0; i < strlen(buffer); i++) {  
            if (buffer[i] == 13 && entcheck ==0) { 
                entcheck =1;
                memset(tmp, 0, cnt);
                cnt = 0;
                continue;
            }
            if (buffer[i] == 13 && entcheck == 1) { 
                break;
            }
            if (buffer[i] != 13) tmp[cnt++] = buffer[i]; 
        } // 첫 쓰래기값을 없애주기 위함
        

        printf("tmp = %s\n" , tmp);
        char vibtmp_l[3] = { 0, };
        char vibtmp_r[3] = { 0, };
        cnt = 0;

        
        usleep(1000);
        for (i = 0; i < strlen(tmp); i++) {
            if (tmp[i] != 'L' && tmp[i] != 'R' && parse_l ==0) 
                vibtmp_l[cnt++] = tmp[i]; //왼쪽의 진동값
            if (tmp[i] == 'L') {
                parse_l = 1; // 왼쪽값 확인
                cnt = 0;
            }
            if(tmp[i] != 'L' && tmp[i] != 'R' && parse_l ==1 ) vibtmp_r[cnt++] = tmp[i]; //오른쪽 진동값
            if (tmp[i] == 'R'){
                parse_l = 0; //오른쪽값 확인후 다시 0으로
                cnt = 0;
                break;
            }
            
        }
        
        //진동 값 int로 변환
        val_l = atoi(vibtmp_l);
        val_r = atoi(vibtmp_r);
        
        
        if(val_l > val_r) { //진동값 비교후 어느쪽에서 충격일어났는지 확인
            check_l = 1;
            check_r = 0;
            v_val = val_l;
         }
        if(val_l < val_r) {
            check_l = 0;
            check_r = 1;
            v_val = val_r;
        }

        printf("v_val = %d\n", v_val);

		if(pre_v_val!=v_val) {
		pre_v_val = v_val; // 진동 확인
		}
        
        if (pre_v_val > 20) { // 진동이 20 이상일때
		usleep(100000);
            if(f_init_flag ==0){
            	pthread_cond_signal(&buzzer_cond);
	    		pthread_cond_signal(&led_cond);  //led 뮤택스 해제
				camera1(); // 2초후 사진찍음
				cledtest(0, 0, 0, 50); //cled 파란색 출력
				memset(nettmp, 0, BUFFERSIZE);
				strcpy(nettmp, tmp);
				strcat(nettmp, "\r\n");
				printf("%s\n", nettmp);
				f_init_flag =1; //사용자의 확인이 있을때 까지 보드에 변동값 저장
			} 
        }
        
        endTime = clock();
        gap = (float)(endTime - startTime) / (CLOCKS_PER_SEC);
		if (gap > 15) {
			mled_init_flag = 0; //mled 튀는값 방지
		}
		
		if(gap > 60) f_init_flag =0; // 사용자의 확인이 장시간 없을시 보드에 다른 변동값을 받는다.
        usleep(10);
		if(sys_init ==1) break; //dip스위치를 통해 초기화
    }
}
    
    void* mledThrd(void* ptr)
    { //충격이 왼쪽인지 오른쪽인지 mled에 출력 left(<) or right(>)
		while (1)
        {
			if(mled_init_flag ==0 && v_val < 20) mledDisplay(0, 0);
			if(v_val >20&&mled_init_flag ==0){		
				if(val_l > val_r)	mledDisplay(1, 0);
           		if(val_l < val_r)	mledDisplay(0, 1);
				mled_init_flag =1;
			}
        }
    }
   
    
    void* ledThrd(void* ptr) { // busled에 충격강도 표시
        while (1) {
           pthread_cond_wait(&led_cond, &led_mutex);
		   ledDisplay(v_val);
        }
    }
    
    void* fndThrd(void* ptr) { // 키패드로 입력받은 비밀번호를 보여준다. 엔터키를 누르면 다시 0으로 초기화
        while (1) {
			if(pass_check !=1){
                fndDisplay(enter_pass);
			}
			else{
				usleep(100);
				fndDisplay(00000);	
			}
		}
    }
    
    //Dipswitch
    void* DeviceThrd(void *ptr) {
        int key_num = 0;
        int dip_newstate = 0;
        while (1) {
            dip_state = getDipState();  //딥스위치의 값 받음
            
            if (dip_state != dip_newstate) { // 딥스위치의 값이 변했다면
                if (dip_state == 0) {
                    sys_init = 0; 
                    key_lock = 0; //키패드 사용불가
                }
                if (dip_state == 1 || dip_state == 3) {
                    sys_init = 1; //시스템을 초기화함
                }
                if (dip_state == 3 || dip_state == 2) {
                    key_lock = 1; //키패드 사용가능
					printf("key_lock=%d\n" ,key_lock);
                }
                dip_newstate = dip_state;  
				printf("dip_state=%d\n" ,dip_state);
            }
        }
    }

    
void *network(void* ptr) {
  	FILE *fp;
	fp = fopen("pic.bmp", "rb");	
  	int server_sockfd = 0;
	int client_sockfd = 0;
	int client_len = 0;
	int n = 0;
	char buf[64000];
 	char file_name[256];
	
	int fileSize;
	int read_cnt;
	int sendCounter;
	int status =0;
	struct sockaddr_in clientaddr, serveraddr;
	if ((server_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		perror("socket error:");
		exit(0);
	}
	
	strcpy(file_name, "pic.bmp");
        strcat(file_name, "\r\n");
	
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(8888);

	bind(server_sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	listen(server_sockfd, 5);
	int sendret;
	while (1) {
		client_sockfd = accept(server_sockfd, (struct sockaddr *)&clientaddr, &client_len);
		int soc = client_sockfd;
		printf("%d , %d\n", client_sockfd, soc);
		printf("v_vlaue pass = %s,%d \n", nettmp, strlen(nettmp));
		send(client_sockfd, nettmp, strlen(nettmp), 0);
		usleep(10000);
		send(client_sockfd, file_name, strlen(nettmp), 0);
		 while(1){
	
			status= fread(buf, 1, 256, fp);
	
			if(status ==0){
				//strcpy(buf, "\r\n");
				//send(client_sockfd, buf, BUF_LEN, 0);
				break; 
			}
	
			write(client_sockfd, buf, status);
		
			//send(client_sockfd, buf, 256, 0);
	
			
		}
		fclose(fp);
		close(client_sockfd);
	}
}

    void* keyThrd(void* ptr) {
		int key_num;
        int i;
        int tmp = 0;
        int multi= 10;
        while(1){
		while (key_lock ==1) {
			key_num = getKeyNum();
			printf("key_num = %d",key_num );
				if (key_num == 98) { //입력을 받을 준비
					enter_pass = 0;
					pass_check= 0;
					for (i = 0; tmp != 97; i++) { //97 == enter
						tmp = getKeyNum(); 
						keyBuzzer(); //키가 눌렸음을 확인겸 많이 들어오는 키패드값 제어
						if (tmp == 97) {
							tmp = 0;
							break;
						}
						else if (tmp == 99 || tmp == 98) { // 쓰지 않는 키들은 무시
							continue;
						}
                    
						else {
							enter_pass = enter_pass * multi + tmp;  //10을 곱해주면서 값을 증가
						}
						if (enter_pass > 999999) { // 999999가 넘을시 999999로 고정
							enter_pass = 999999;
						}
						printf("key_tmp = %d",tmp);
					} //end for
				} // end if
				printf("input password  is %d \n", enter_pass);
				if(enter_pass == password){
					pass_check = 1; // enter_pass == password
					lockOpen_Buzzer();
					printf("pass_lock open\n");
				}
				else{
					lockClose_Buzzer();
					printf("pass_lock not open\n");
				}
		}// end while
		}
    }// end key


void* touchThrd(void* ptr)
{
	int past_touchX = 0;
	int past_touchY = 0;//touch

	int touchX = 0;
	int touchY = 0;
	while (1)   // x, y의 값에 따라 그림 출력
	{
		
		readFirstCoordinate(&touchX, &touchY);
		
		//sleep(1);
		if ((past_touchX - 20) < touchX && touchX < (past_touchX + 20) && (past_touchY - 20) < touchY && touchY < (past_touchY + 20))
		{
			continue;			 // 큰 변동이 없음을 감지
		}
		else
		{
			past_touchX = touchX;
			past_touchY = touchY;
			
		}
		if (1030 < touchX && touchX < 1180 && 100 < touchY && touchY < 350)
		{
			usleep(50000);
			bitmaptest(SCREEN, pfbmap, 640, 0);
			bitmaptest("whitesc.bmp", pfbmap, 0, 0);
			bitmaptest("bitMenual.bmp", pfbmap, 0, 0); // 메뉴얼 그림 출력
			touchX = 0;
			touchY = 0;	
			
		}

		if ((1030 < touchX && touchX < 1180 && 450 < touchY && touchY < 700) && pass_check ==1)
		{	
			
			bitmaptest(SCREEN, pfbmap, 640, 0);
			bitmaptest("whitesc.bmp", pfbmap, 0, 0);
			usleep(100000);	
			camera(cfbmap, CameraViewX, CameraViewY);// 실시간 출력
			touchX = 0;
			touchY = 0;
		}

		if ((740 < touchX && touchX < 890 && 100 < touchY && touchY < 350) && pass_check ==1)
		{	
			bitmaptest(SCREEN, pfbmap, 640, 0);
			bitmaptest("whitesc.bmp", pfbmap, 0, 0);
			if(pre_v_val < 20){
				usleep(10000);			
				bitmaptest(NORMAL, pfbmap, 100, 150);
			} // normal
			if(check_l == 1 && pre_v_val >20){
				usleep(10000);			
				bitmaptest(LEFT, pfbmap, 100, 150);
			} // left
			if(check_r == 1 && pre_v_val >20){
				usleep(10000);			
				bitmaptest(RIGHT, pfbmap, 100, 150);
			} // right
			touchX = 0;
			touchY = 0;
		}
		if ((740 < touchX && touchX < 890 && 450 < touchY && touchY < 700) && pass_check ==1)
		{	
			usleep(10000);
			bitmaptest(SCREEN, pfbmap, 640, 0);
			bitmaptest("whitesc.bmp", pfbmap, 0, 0);
			camera1(); //사진 캡쳐
			usleep(100000);
			touchX = 0;
			touchY = 0;
		}
	}

}



void *buzzerThrd(void* ptr) {
	while (1) {
		pthread_cond_wait(&buzzer_cond, &buzzer_mutex);
		WarningBuzzer(); //사고시 버저 울림
	}
}




int Arduino(int *arduino_fd) {
    printf("arduino\n");
    struct termios oldtio, newtio;
    *arduino_fd = open(ARUDUINO_DEV_NAME, O_RDWR | O_NOCTTY);
    if (*arduino_fd < 0) {
        perror(ARUDUINO_DEV_NAME);
        return -1;
    }
    tcgetattr(*arduino_fd, &oldtio);
    
    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
    
    newtio.c_iflag = IGNPAR | ICRNL;
    
    newtio.c_oflag = 0;
    newtio.c_lflag = ICANON;
    newtio.c_cc[VINTR] = 0;     /* Ctrl-c */
    tcflush(*arduino_fd, TCIFLUSH);
    tcsetattr(*arduino_fd, TCSANOW,  &newtio);
    return 1;
}
