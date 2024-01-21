import { BleClient, numbersToDataView, numberToUUID, dataViewToText, textToDataView } from '@capacitor-community/bluetooth-le'

const CONNECT_TO             =  "BluetoothSplitter"
const HEART_RATE_SERVICE     = '0000180d-0000-1000-8000-00805f9b34fb'
const CHARACTERISTIC_UUID_RX = '00002a37-0000-1000-8000-00805f9b34fb'
const CHARACTERISTIC_UUID_TX = 'fb005c81-02e7-f387-1cad-8acd2d8df0c8'

// #StateMachine
// State      Transistion    Next State  
// Idle       --Start-->     Scanning
// Scanning   --Found-->     Connecting
// Connecting --Connected--> Connected
// Connected  --Disconnect-->Scanning

let onResult = false

class ble{
    constructor(onChange, onRes){
        this.onChange         = onChange
        onResult              = onRes
        console.log('HERERREE', onResult)
        this.connected        = false
        this.data_recieved    = ""
        this.messages         = []
        this.deviceId         = ""
    }

    // For some reason this one function has no "this" context?
    async onMessage(dataview){
        const classy  = this
        const data    = dataViewToText(dataview)
        onResult(data)
    }

    async sendMessage(message){
        const classy = this
        if (classy.connected){
            try {
                await BleClient.write(classy.deviceId, HEART_RATE_SERVICE, CHARACTERISTIC_UUID_RX, textToDataView(message));
            }catch(err){
                alert(`Error Sending Message to ${CONNECT_TO}`)
            }
        }else{
            alert("Not connected, or in config mode")
        }
    }

    async updateConnectionState(state){
        const classy   = this
        this.connected = state
        classy.onChange(this.connected) 
    }
    
    async onDisconnect(deviceId){
        const classy   = this
        const connected=false
        classy.updateConnectionState(connected)
        classy.init()
    }

    async connect(device){
        const classy = this
        try{ 
            await BleClient.connect(device.deviceId, (deviceId) => classy.onDisconnect(deviceId));
            await BleClient.startNotifications(device.deviceId, HEART_RATE_SERVICE, CHARACTERISTIC_UUID_TX, classy.onMessage)
            const connected = true
            classy.updateConnectionState(connected) 
        }catch(err){
            alert(err)
            classy.start()
        }
    }

    async init(){
        const classy    = this
        const connected = false
        await classy.updateConnectionState(connected)
        try {
            await BleClient.initialize();
            const res = await BleClient.requestLEScan({},
              async (result) => {
                const deviceName = result?.device?.name
                if (deviceName?.includes(CONNECT_TO)){
                  await BleClient.stopLEScan();
                  classy.deviceId = result.device.deviceId
                  classy.connect(result.device)
                }
              }
            )
            console.log(res)
        }catch(err){
            alert("Error Occured while attempting to scan for BLE devices...")
        }
    }
}

export default ble;