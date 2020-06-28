#include "mbed.h"
#include "bbcar.h"
#include "mbed_rpc.h"

Thread thread1;
Thread thread2;
Thread thread3;
Ticker servo_ticker;
Ticker encoder_ticker;

EventQueue queue(32 * EVENTS_EVENT_SIZE);

PwmOut pin8(D8), pin9(D9);
Serial pc(USBTX,USBRX); //tx,rx
Serial uart(D1,D0); //tx,rx
RawSerial xbee(D12, D11);
DigitalIn pin3(D3);   // encoder
DigitalInOut pin10(D10);   // encoder
DigitalOut run_led(LED1); // red: run
DigitalOut turn_led(LED2); // green: turn
DigitalOut img_led(LED3); // blue: img
BBCar car(pin8, pin9, servo_ticker);

void log(void);
//RPCFunction rpclog(&log, "log");

void recieve_thread();
void send_thread();
void mission2(void);
void minst(void);

void xbee_rx_interrupt(void);
void xbee_rx(void);
void reply_messange(char *xbee_reply, char *messange);
void check_addr(char *xbee_reply, char *messenger);

bool photo = false;
bool recieve = false;
bool run = false;
bool turn = false;
bool img = false;
bool ping = false;

parallax_ping ping1(pin10);
parallax_encoder encoder0(pin3, encoder_ticker);

int main() {
    //char xbee_reply[4];
    xbee.baud(9600);
    run_led = 1;
    turn_led = 1;
    img_led = 1;
    
    uart.baud(9600);
    thread1.start(send_thread);
    thread2.start(recieve_thread);
    thread3.start(log);

    // go straight
    wait_ms(2000);
    run_led = 0;
    run = true;
    car.servo0.set_speed(-125);
    car.servo1.set_speed(82);
    //car.goStraight(100);
    // 125cm
    wait_ms(7500);
    //while(encoder0.get_cm() < 88) {};
    car.stop();
    //encoder0.reset();
    run = false;
    run_led = 1;
    wait_ms(50);

    // turn left
    wait_ms(1500);
    turn_led = 0;
    turn = true;
    //car.turn(-55, -0.25);
    car.turn(-100, -0.3);
    wait_ms(2100);
    //while(encoder0.get_cm() < 8) {};
    car.stop();
    //encoder0.reset();
    turn_led = 1;
    turn = false;
    wait_ms(50);

/*
/////////////////
    // back left
    //car.turn(55, -0.25);
    wait_ms(500);
    car.turn(100, -0.3);
    wait_ms(1900);
    //while(encoder0.get_cm() < 4.5) {};
    encoder0.reset();
    car.stop();
    turn_led = 0;
    wait_ms(50);
    
    // turn right 
    //car.turn(-55, 0.25);
    wait_ms(500);
    car.turn(-100, 0.3);
    wait_ms(2000);
    //while(encoder0.get_cm() < 20) {};
    encoder0.reset();
    car.stop();
    wait_ms(50);

    // back right 
    //car.turn(55, 0.25);
    wait_ms(500);
    car.turn(100, 0.3);
    wait_ms(2000);
    //while(encoder0.get_cm() < 19) {};
    encoder0.reset();
    car.stop();
    wait_ms(50);
*/
/////////////////////

    // go straight to park
    wait_ms(1000);
    run = true;
    run_led = 0;
    car.servo0.set_speed(-130);
    car.servo1.set_speed(82);
    //car.goStraight(100);
    // 70 cm
    //while(encoder0.get_cm()< 48) {};
    wait_ms(4300);
    car.stop();
    wait_ms(50);
    run_led = 1;
    run = false;
    
    // parking
    // back turn
    turn_led = 0;
    turn = true;
    wait_ms(500);
    car.turn(100, -0.3);
    wait_ms(2000);
    //encoder0.reset();
    car.stop();
    turn_led = 1;
    turn = false;
    wait_ms(50);

    // back
    wait(1);
    run_led = 0;
    turn = true;
    car.servo0.set_speed(125);
    car.servo1.set_speed(-82);
    wait_ms(2000);
    //while(encoder0.get_cm() < 10) {};
    //encoder0.reset();
    car.stop();
    run_led = 1;
    turn = false;
    
    // leave parking
    wait(1);
    run_led = 0;
    turn = true;
    car.servo0.set_speed(-125);
    car.servo1.set_speed(82);
    // ? cm
    wait_ms(1500);
    //while(encoder0.get_cm() < 10) {};
    //encoder0.reset();
    car.stop();
    run_led = 1;
    turn = false;
    
    // turn left
    wait_ms(1000);
    turn_led = 0;
    turn = true;
    //car.turn(-55, -0.25);
    car.turn(-100, -0.3);
    wait_ms(2100);
    //while(encoder0.get_cm() < 8) {};
    car.stop();
    //encoder0.reset();
    turn_led = 1;
    turn = false;
    wait_ms(50);

    // go straight to img
    wait(1);
    run_led = 0;
    car.servo0.set_speed(-130);
    car.servo1.set_speed(90);
    wait_ms(1000);
    // 50cm
    //while(encoder0.get_cm() < 15) {
        // ping
    //};
   // encoder0.reset();
    car.stop();
    wait_ms(50);
    run_led = 1;

    // back turn
    wait_ms(1500);
    turn = true;
    turn_led = 0;
    car.turn(100, -0.3);
    wait_ms(2000);
    //while(encoder0.get_cm() < 4.5) {};
    //encoder0.reset();
    car.stop();
    turn_led = 1;
    turn = false;
    wait_ms(50);

    // mnist
    img = true;
    img_led = 0;
    photo = true;
    wait(7);
    while(!recieve) {
        wait(5);
    }
    img_led = 1;
    img = false;

    // turn right
    wait_ms(2000);
    turn_led = 0;
    turn = true;
    wait_ms(500);
    car.turn(-100, 0.3);
    wait_ms(2000);
    //while(encoder0.get_cm() < 20) {};
    encoder0.reset();
    car.stop();
    turn_led = 1;
    turn = false;
    wait_ms(50);

    // go straight to leave
    wait_ms(2000);
    run_led = 0;
    run = true;
    car.servo0.set_speed(-125);
    car.servo1.set_speed(82);
    // 55cm
    wait_ms(2500);
    car.stop();
    wait_ms(50);
    run_led = 1;
    run = false;

    // turn right
    wait_ms(1500);
    turn_led =  0;
    turn = true;
    wait_ms(900);
    car.turn(-100, 0.3);
    wait_ms(2000);
    //while(encoder0.get_cm() < 20) {};
    encoder0.reset();
    car.stop();
    turn_led = 1;
    turn = false;
    wait_ms(50);

    // straight
    wait_ms(1500);
    run_led = 0;
    car.servo0.set_speed(-125);
    car.servo1.set_speed(82);
    // 100cm
    wait_ms(7000);
    car.stop();
    wait_ms(50);
    run_led = 1; 

    // turn right
    wait_ms(1500);
    turn_led =  0;
    turn = true;
    wait_ms(1000);
    car.turn(-100, 0.3);
    wait_ms(2000);
    car.stop();
    turn_led = 1;
    turn = false;
    wait_ms(50);

    // straight to 1
    wait_ms(1500);
    run_led = 1;
    car.servo0.set_speed(-125);
    car.servo1.set_speed(82);
    // 50cm
    wait_ms(3000);
    car.stop();
    wait_ms(50);
    run_led = 0; 

    // turn right
    wait_ms(1500);
    turn_led = 0;
    turn = true;
    wait_ms(1000);
    car.turn(-100, 0.3);
    wait_ms(2000);
    car.stop();
    turn_led = 1;
    turn = false;
    wait_ms(50);

    // ping
    ping = true;
    mission2();
    ping = false;

    // back right 
    wait_ms(500);
    turn = true;
    turn_led = 0;
    car.turn(100, 0.3);
    wait_ms(2000);
    //while(encoder0.get_cm() < 19) {};
    encoder0.reset();
    car.stop();
    turn = false;
    turn_led = 1;
    wait_ms(50);

    // straight to end
    wait_ms(1500);
    run_led = 0;
    run = true;
    car.servo0.set_speed(-125);
    car.servo1.set_speed(82);
    // 50cm
    wait_ms(5000);
    car.stop();
    run_led = 1;
    run = false;    
    wait_ms(50);

    // turn right
    wait_ms(1500);
    turn_led =  0;
    turn = true;
    wait_ms(1000);
    car.turn(-100, 0.3);
    wait_ms(2000);
    car.stop();
    turn_led = 1;
    turn = false;
    wait_ms(50);

    // end
    wait_ms(1500);
    run_led = 0;
    run = true;
    car.servo0.set_speed(-125);
    car.servo1.set_speed(82);
    // 125cm
    wait_ms(8000);
    car.stop();
    run_led = 1;
    run = false;

    xbee.attach(xbee_rx_interrupt, Serial::RxIrq);
}

void minst(void)
{
    while (!recieve){};
}

void log(void)
{
 //   int  flag = 0;
  //  char t[2];
    while(1) {
        if (run) {
            xbee.printf("run\r\n");
            //flag = 1;
            //sprintf(t, "%1d", flag);
            //xbee.printf("%d", 1);
            //xbee.printf("%s", t);
            wait(0.2);
        }
        else if (turn) { 
            xbee.printf("turn\r\n");
            //flag = 2;
            //sprintf(t, "%1d", flag);
            //xbee.printf("%s", t);
            wait(0.2);
        }
        else if (img) { 
            xbee.printf("photo\r\n");
            //flag = 3;
            //sprintf(t, "%1d", flag);
            //xbee.printf("%s", t);
            wait(0.2);
        }
        else if (ping) {
            xbee.printf("ping\r\n");
            //flag = 4;
            //sprintf(t, "%1d", flag);
            //xbee.printf("%s", t);
            wait(0.2);
        }
        else {
            xbee.printf("stop\r\n");
            //flag = 5;
            //sprintf(t, "%1d", flag);
            //xbee.printf("%s", t);
            wait(0.2);
        }
        wait_ms(1000);
    }
}

void mission2(void)
{
    wait_ms(500);
    if ((float)ping1 < 35)
        xbee.printf("square\r\n");
    else {
        if ((float)ping1 > 45)
            xbee.printf("double triangle\r\n");
        else {
            if ((float)ping1 < 40)
                xbee.printf("triangle\r\n");
            else
                xbee.printf("right triangle\r\n");
        }
    }
    // led
    for (int i = 0; i < 3; i++) {
        run_led = 0;
        wait_ms(100);
        run_led = 1;
        wait_ms(100);
    }

    wait_ms(2000);
}

void recieve_thread(){
   while(1) {
      if(uart.readable()){
            recieve = true;
            char recv = uart.getc();
            xbee.putc(recv);
            xbee.printf("\r\n");
      }
   }
}

void send_thread(){
    while(1){
        wait(3);
        //img_led = 0;
        if(photo){
            char s[10];
            sprintf(s,"img");
            uart.puts(s);
            xbee.printf("send\r\n");
            photo = false;
            wait(0.5);
        }
        //else pc.printf("not\r\n");
        //pc.printf("%d\r\n", recieve);
    }
}

void xbee_rx_interrupt(void)
{
  xbee.attach(NULL, Serial::RxIrq); // detach interrupt
  queue.call(&xbee_rx);
}

void xbee_rx(void){
    char buf[100] = {0};
    char outbuf[100] = {0};

    if(xbee.readable()){
        for (int i=0; ; i++) {
            char recv = xbee.getc();
            if (recv == '\r') {
                break;
            }
            buf[i] = pc.putc(recv);
            //buf[i] = recv;
        } 
    }
    xbee.attach(xbee_rx_interrupt, Serial::RxIrq); // reattach interrupt
}

void reply_messange(char *xbee_reply, char *messange)
{
  xbee_reply[0] = xbee.getc();
  xbee_reply[1] = xbee.getc();
  xbee_reply[2] = xbee.getc();
  if(xbee_reply[1] == 'O' && xbee_reply[2] == 'K'){
    pc.printf("%s\r\n", messange);
    xbee_reply[0] = '\0';
    xbee_reply[1] = '\0';
    xbee_reply[2] = '\0';
  }
}

void check_addr(char *xbee_reply, char *messenger)
{
  xbee_reply[0] = xbee.getc();
  xbee_reply[1] = xbee.getc();
  xbee_reply[2] = xbee.getc();
  xbee_reply[3] = xbee.getc();
  pc.printf("%s = %c%c%c\r\n", messenger, xbee_reply[1], xbee_reply[2], xbee_reply[3]);
  xbee_reply[0] = '\0';
  xbee_reply[1] = '\0';
  xbee_reply[2] = '\0';
  xbee_reply[3] = '\0';
}