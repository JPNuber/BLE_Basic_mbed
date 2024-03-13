#include <stdio.h>
#include "BLE_Service/BLEProcess.h"
#include "BLE_Service/Service.h"


// main() runs in its own thread in the OS
int main()
{

    BLE &ble_interface = BLE::Instance();
    events::EventQueue event_queue;
    SendRandomNumberService  demo_service;
    BLEProcess ble_process(event_queue, ble_interface);

    ble_process.on_init(callback(&demo_service, &SendRandomNumberService::start)); 

    // bind the event queue to the ble interface, initialize the interface
    // and start advertising
    ble_process.start();

    // Process the event queue.
    event_queue.dispatch_forever();

    return 0;
}

