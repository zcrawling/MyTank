//
// Created by sb on 2/3/26.
//

#include "Arduino_RouterBridge.h"

void setup() {
    Bridge.begin();
    Bridge.provide("loops", loops);
    Monitor.begin();
    delay(100);
    Monitor.println("init");
}
void loop() {

}
void loops() {
    Monitor.println("Hi");
    delay(50);
}
