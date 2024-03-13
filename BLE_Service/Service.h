
#include "Characteristic.h"
#include "events/EventQueue.h"

using mbed::callback;
class SendRandomNumberService{
    typedef SendRandomNumberService Self;

public:
    SendRandomNumberService():
        _notification("5a3b0203-f6dd-4c45-b31f-e89c05ae3390",0),
        _SRNS(
            /* uuid */ "5a3b0003-f6dd-4c45-b31f-e89c05ae3390",
            /* characteristics */ _SRNS_characteristics,
            /* numCharacteristics */ sizeof(_SRNS_characteristics) /
                                     sizeof(_SRNS_characteristics[0])        
        ),
        _server(NULL),
        _event_queue(NULL)        
    {
        _SRNS_characteristics[0] = &_notification;
    }

    void start(BLE &ble_interface, events::EventQueue &event_queue)
    {
         if (_event_queue) {
            return;
        }

        _server = &ble_interface.gattServer();
     
        _event_queue = &event_queue;

        // register the service
        printf("Adding demo service\r\n");
        ble_error_t err = _server->addService(_SRNS);

        if (err) {
            printf("Error %u during demo service registration.\r\n", err);
            return;
        }

        // read write handler
        _server->onDataSent(as_cb(&Self::when_data_sent));
        //_server->onDataWritten(as_cb(&Self::when_data_written));
        //_server->onDataRead(as_cb(&Self::when_data_read));


        // updates subscribtion handlers
        _server->onUpdatesEnabled(as_cb(&Self::when_update_enabled));
        _server->onUpdatesDisabled(as_cb(&Self::when_update_disabled));
        _server->onConfirmationReceived(as_cb(&Self::when_confirmation_received));

        // print the handles
        printf("Send Random number registered\r\n");
        printf("service handle: %u\r\n", _SRNS.getHandle());
        printf("\t notification characteristic value handle %u\r\n", _notification.getValueHandle());

        _event_queue->call_every(100/* ms */, callback(this, &Self::test));
    }
    
    void test(void){
        
        ble_error_t err = _notification.get(*_server, *second);
        if (err) {
            printf("read of the second value returned error %u\r\n", err);
            return;
        }

        second[0] = (second[0]  + 1) % 60;
        second[1] = (second[1]  + 2) % 60;
        second[2] = (second[2]  + 3) % 60;

        err = _notification.set(*_server, second);
        if (err) {
            printf("write of the second value returned error %u\r\n", err);
            return;
        }
    }
private:
    uint8_t second [3] = {0,0,0};
    /**
     * Handler called when an indication confirmation has been received.
     *
     * @param handle Handle of the characteristic value that has emitted the
     * indication.
     */
    void when_confirmation_received(GattAttribute::Handle_t handle)
    {
        printf("confirmation received on handle %d\r\n", handle);
    }

    /**
     * Handler called when a notification or an indication has been sent.
     */
    void when_data_sent(unsigned count)
    {
        printf("sent %u updates\r\n", count);
    }
    /**
     * Handler called after a client has subscribed to notification or indication.
     *
     * @param handle Handle of the characteristic value affected by the change.
     */
    void when_update_enabled(GattAttribute::Handle_t handle)
    {
        printf("update enabled on handle %d\r\n", handle);
    }

    /**
     * Handler called after a client has cancelled his subscription from
     * notification or indication.
     *
     * @param handle Handle of the characteristic value affected by the change.
     */
    void when_update_disabled(GattAttribute::Handle_t handle)
    {
        printf("update disabled on handle %d\r\n", handle);
    }

    /**
     * Helper that construct an event handler from a member function of this
     * instance.
     */
    template<typename Arg>
    FunctionPointerWithContext<Arg> as_cb(void (Self::*member)(Arg))
    {
        return makeFunctionPointer(this, member);
    }


NotifyCharacteristic<uint8_t,3> _notification;

// list of the characteristics of the clock service
GattCharacteristic* _SRNS_characteristics[1];

// demo service
GattService _SRNS;

GattServer* _server;
events::EventQueue *_event_queue;
};