#include "mbed.h"
#include "bbcar.h"

Thread thread1;
Thread thread2;
Ticker servo_ticker;
Ticker encoder_ticker;
PwmOut pin8(D8), pin9(D9);
Serial pc(USBTX,USBRX); //tx,rx
Serial uart(D12,D11); //tx,rx
DigitalIn pin3(D3);
DigitalOut run_led(LED1); // red: run
DigitalOut turn_led(LED2); // : turn
BBCar car(pin8, pin9, servo_ticker);

bool photo = false;

int main() {
    // openmv
    uart.baud(9600);
    thread1.start(send_thread);
    thread2.start(recieve_thread);

    parallax_encoder encoder0(pin3, encoder_ticker);
    encoder0.reset();

    // go straight
    run_led = 1;
    car.servo0.set_speed(130);
    car.servo1.set_speed(-90);
    //car.goStraight(100);
    // 125cm
    while(encoder0.get_cm()< 40) {};
    encoder0.reset();
    car.stop();
    wait_ms(50);
    run_led = 0;
    
    // turn left
    // turn(speed, factor)
    // left factor +
    // right factor -
    turn_led = 1;
    car.turn(55, 0.25);
    while(encoder0.get_cm() < 18) {};
    encoder0.reset();
    car.stop();
    turn_led = 0;
    wait_ms(50);

    // go straight to park
    run_led = 1;
    car.servo0.set_speed(130);
    car.servo1.set_speed(-90);
    // bottom line with right
    // 70cm
    while(encoder0.get_cm() < 20) {
        // ping
    };
    encoder0.reset();
    car.stop();
    wait_ms(50);
    run_led = 0;
    
    // parking
    // back turn 
    car.turn(-55, 0.25);
    while(encoder0.get_cm() < 20) {};
    encoder0.reset();
    car.stop();
    wait_ms(50);

    // back
    car.servo0.set_speed(-130);
    car.servo1.set_speed(90);
    // 40 cm
    while(encoder0.get_cm() < 15) {};
    encoder0.reset();
    car.stop();
    
    // leave parking
     car.servo0.set_speed(130);
    car.servo1.set_speed(-90);
    // ? cm
    while(encoder0.get_cm() < 15) {};
    encoder0.reset();
    car.stop();

    // turn left
    turn_led = 1;
    car.turn(55, 0.25);
    while(encoder0.get_cm() < 18) {};
    encoder0.reset();
    car.stop();
    turn_led = 0;
    wait_ms(50);

    // go straight to img
    run_led = 1;
    car.servo0.set_speed(130);
    car.servo1.set_speed(-90);
    // 50cm
    while(encoder0.get_cm() < 20) {
        // ping
    };
    encoder0.reset();
    car.stop();
    wait_ms(50);
    run_led = 0;

    // turn 
    car.turn(-55, 0.25);
    while(encoder0.get_cm() < 20) {};
    encoder0.reset();
    car.stop();
    wait_ms(50);

    // mnist
    photo = true;

    // turn right
    turn_led = 1;
    car.turn(55, -0.25);
    while(encoder0.get_cm() < 18) {};
    encoder0.reset();
    car.stop();
    turn_led = 0;
    wait_ms(50);

    // go straight to leave
    run_led = 1;
    car.servo0.set_speed(130);
    car.servo1.set_speed(-90);
    // 55cm
    while(encoder0.get_cm() < 20) {
        // ping
    };
    encoder0.reset();
    car.stop();
    wait_ms(50);
    run_led = 0;

    // left
    turn_led = 1;
    car.turn(55, 0.25);
    while(encoder0.get_cm() < 18) {};
    encoder0.reset();
    car.stop();
    turn_led = 0;
    wait_ms(50); 

    // straight
    run_led = 1;
    car.servo0.set_speed(130);
    car.servo1.set_speed(-90);
    // 100cm
    while(encoder0.get_cm() < 20) {
        // ping
    };
    encoder0.reset();
    car.stop();
    wait_ms(50);
    run_led = 0; 

    // right
    turn_led = 1;
    car.turn(55, -0.25);
    while(encoder0.get_cm() < 18) {};
    encoder0.reset();
    car.stop();
    turn_led = 0;
    wait_ms(50); 

    // straight to 1
    run_led = 1;
    car.servo0.set_speed(130);
    car.servo1.set_speed(-90);
    // 50cm
    while(encoder0.get_cm() < 20) {
        // ping
    };
    encoder0.reset();
    car.stop();
    wait_ms(50);
    run_led = 0; 

    // right
    turn_led = 1;
    car.turn(55, -0.25);
    while(encoder0.get_cm() < 18) {};
    encoder0.reset();
    car.stop();
    turn_led = 0;
    wait_ms(50);

    // ping

    // back turn
    car.turn(-55, -0.25);
    while(encoder0.get_cm() < 18) {};
    encoder0.reset();
    car.stop();
    wait_ms(50);

    // straight  to end
    run_led = 1;
    car.servo0.set_speed(130);
    car.servo1.set_speed(-90);
    // 50cm
    while(encoder0.get_cm() < 20) {
        // ping
    };
    encoder0.reset();
    car.stop();
    wait_ms(50);
    run_led = 0;

    // right
    turn_led = 1;
    car.turn(55, -0.25);
    while(encoder0.get_cm() < 18) {};
    encoder0.reset();
    car.stop();
    turn_led = 0;
    wait_ms(50);

    // end
    run_led = 1;
    car.servo0.set_speed(130);
    car.servo1.set_speed(-90);
    // 125cm
    while(encoder0.get_cm() < 20) {
        // ping
    };
    encoder0.reset();
    car.stop();
    run_led = 0;
}

void recieve_thread(){
   while(1) {
      if(uart.readable()){
            char recv = uart.getc();
            pc.putc(recv);
            pc.printf("\r\n");
      }
   }
}

void send_thread(){
   while(1){
      if(photo){
         char s[10];
         sprintf(s,"img");
         uart.puts(s);
         //pc.printf("send\r\n");
         wait(0.5);
      }
   }
}
