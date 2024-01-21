<template>
  <div class="content-divider">   
    <div class="top">
      <div class="center-container">
        <img class="icon" src="./assets/icon-only.png">
      </div>
      <div class="center-container">
        <div class="connection-status">
          <div>Connection</div>
          <div v-if="connected" class="green-circle"></div>
          <div v-else           class="red-circle"></div>
        </div>
        <div>{{ mode }} Mode</div>
      </div>
    </div>  
    <div class="section-title">Selected</div>
    <div class="devices-list">
      <div v-for="speaker,index in chosen" class="device" :class="{'not-last':index!=chosen.length-1}"
        @click="remove(index)"  @touchend="console.log('hey')"
      >
        <div class="name">{{ speaker.ssid }}</div> 
        <div class="action"><svg viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg" stroke="#e50606"><g id="SVGRepo_bgCarrier" stroke-width="0"></g><g id="SVGRepo_tracerCarrier" stroke-linecap="round" stroke-linejoin="round"></g><g id="SVGRepo_iconCarrier"> <path d="M7 12L17 12" stroke="#c80e0e" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"></path> <circle cx="12" cy="12" r="9" stroke="#c80e0e" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"></circle> </g></svg>
        </div>
      </div>
    </div>
    <div class="section-title">Other Devices</div>
    <div class="devices-list">
      <div v-for="speaker,index in devices" class="device" :class="{'not-last':index!=devices.length-1}"
      @click="event => add(speaker.ssid, index, event)" @touchend="console.log('hey')"
      >
        <div class="name">{{ speaker.ssid }}</div>
        <div class="action">
          <svg viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg"><g id="SVGRepo_bgCarrier" stroke-width="0"></g><g id="SVGRepo_tracerCarrier" stroke-linecap="round" stroke-linejoin="round"></g><g id="SVGRepo_iconCarrier"> <path d="M11 8C11 7.44772 11.4477 7 12 7C12.5523 7 13 7.44772 13 8V11H16C16.5523 11 17 11.4477 17 12C17 12.5523 16.5523 13 16 13H13V16C13 16.5523 12.5523 17 12 17C11.4477 17 11 16.5523 11 16V13H8C7.44771 13 7 12.5523 7 12C7 11.4477 7.44772 11 8 11H11V8Z" fill="#339127"></path> <path fill-rule="evenodd" clip-rule="evenodd" d="M23 12C23 18.0751 18.0751 23 12 23C5.92487 23 1 18.0751 1 12C1 5.92487 5.92487 1 12 1C18.0751 1 23 5.92487 23 12ZM3.00683 12C3.00683 16.9668 7.03321 20.9932 12 20.9932C16.9668 20.9932 20.9932 16.9668 20.9932 12C20.9932 7.03321 16.9668 3.00683 12 3.00683C7.03321 3.00683 3.00683 7.03321 3.00683 12Z" fill="#339127"></path> </g></svg>
        </div>
      </div>
    </div>
    <div class="space"></div>
  </div>
</template>
  
<script>
import ble from "./assets/ble.js" 

export default{
  data(){
    return {
      config      : "",
      mode        : "Config",
      deviceNames : [],
      deviceId : "",
      connected : false,
      data_recieved : "",
      out: "",
      chosen : [],
      devices: []
    }
  },
  async mounted(){
    console.log("starting up...")
    const onChange        = (connected)=>{this.connected = connected}
    const onResult        = async (result)=>{
      const res           = JSON.parse(result)
      if (res?.connection){
        this.handleConnectionMessage(res)
      }else{
        this.handleNewDevice(res)
      }
    }
    this.config = new ble(onChange, onResult)
    await this.config.init()
  },
  methods:{
    async sendConfigMessage(speakerNum, speakerName){
      const message = `${speakerNum} ${speakerName}`
      this.config.sendMessage(message)
    },
    async handleNewDevice(device){
      const foundInDevices = this.devices.find(item=>{
        return item.ssid==device.ssid && item.address==device.address
      })
      if (!foundInDevices){
        this.devices.unshift({ssid:device.ssid,address:device.address})
      }
    },
    async handleConnectionMessage(connection){
      const id   = connection.connection
      const ssid = connection.ssid
      if (ssid.replaceAll(' ', '').length>0){
        this.chosen[Number(id)] = {ssid:ssid}
      }
    },
    async add(ssid, index){
      if (this.connected==true && this.mode=="Config"){
        const length = this.chosen.length
        if (length>1){
          alert("Please remove a connection before adding a new one")
        }else{
          this.chosen.push({ssid:ssid})
          this.devices.splice(index,1)
          // message logic
          this.config.sendMessage(`${length} ${ssid}`)
        }
      }
    },
    async remove(index){
      if (this.connected==true && this.mode=="Config"){
        await new Promise(res=>setTimeout(res,250))
        console.log(`remove from ${index}`)
        // message logic
        if (index==0 && this.chosen?.[1]){
          const device = this.chosen[1].ssid
          this.config.sendMessage(`0  ${device}`)
          this.config.sendMessage(`1   `)
        }else{
          this.config.sendMessage(`${index}   `)
        }
        this.chosen.splice(index,1)
      }
    }
  }
}
</script>

<style>
* {
  box-sizing: border-box;
}
:root{
  --main-font-family : "Helvetica Neue", helvetica, arial, "Hiragino Kaku Gothic ProN", Meiryo, "MS Gothic";
  --main-font-weight : normal;
  --main-font-size   : 15px;
  --main-bg-color    : #050404;
  --second-bg-color  : #121212;
  --third-bg-color   : #252326;
  --fouth-bg-color   : #322f33;
  --main-font-color  : rgb(245, 245, 245, 0.9);
  --second-font-color: #959292;
}

body,.content-divider {
  margin: env(safe-area-inset-top, 20px) env(safe-area-inset-right, 20px) 
   env(safe-area-inset-left, 20px);
  background-color: var(--main-bg-color);
}
.content-divider{
  height: calc(100vh - env(safe-area-inset-top, 20px));
  overflow: scroll;
}

html, body,#app{ height: 100%; } 
html,body, #app{
  background: var(--main-bg-color);
  color     : var(--second-font-color);
  font-family: var(--main-font-family);
  font-weight: var(--main-font-weight);
  font-size  : var(--main-font-size);
  overflow: hidden;
  scroll-behavior: smooth;
  transition: background-color 1s ease-in-out;
}

.top{
  margin:0;
  position: -webkit-sticky;
  position: sticky;
  top: 0;
  background-color: var(--main-bg-color);
  padding-bottom: 10px;
  z-index: 3;
}
.center-container{
  display: grid;
  align-items:center;
  justify-items:center;
  width: 100%;
  overflow: hidden;
  grid-gap: 5px;
}
.icon{
  height: 100px;
  width : 100px;
}
.connection-status{
  padding-top: 10px;
  display: grid;
  align-items:center;
  justify-items:center;
  justify-content:center;
  align-content:center;
  grid-template-columns: auto auto;
  width: 100%;
  grid-gap: 8px;
}
.green-circle{
  height: 15px;
  width: 15px;
  border-radius: 100%;
  background: green;
}
.red-circle{
  height: 15px;
  width: 15px;
  border-radius: 100%;
  background: red;
}
.section-title{
  text-transform: uppercase;
  padding-left: 30px;
  padding-bottom: 10px;
  padding-top: 10px;
}
.devices-list{
  padding:0px;
  display: grid;
  width: calc(100% - 40px);
  border-radius: 10px;
  margin-left: 20px;
  margin-right: 20px;
  overflow:hidden;
  background-color: var(--third-bg-color);
}
.device{
  padding:0px;
  display: grid;
  grid-template-columns: calc(100% - 50px) 50px;
  height: 50px;
  width: calc(100%);
  background-color: var(--third-bg-color);
  color: var(--main-font-color);
  padding-left: 40px;
  transition: background-color .05s ease-in-out;
  border: none;
}
.device:active{ background-color: var(--fouth-bg-color);}

.not-last{
  border-bottom: 1px solid #3e3434;
}

.name{
  display: grid;
  justify-content: left;
  align-content: center;
  height: 100%;
  width: 100%;
}
.action{
  width: 25px;
  height: 25px;
  justify-self: center;
  align-self: center;
}
.space{
  height: 150px;
}

::-webkit-scrollbar {
  display: none;
  -webkit-appearance: none;
  width: 7px;
  height: 4px;
}
::-webkit-scrollbar-thumb {
  border-radius: 4px;
  background-color: grey;
  box-shadow: 0 0 1px rgba(255, 255, 255, .5);
}
::-webkit-scrollbar-thumb:horizontal{
    height: 1px;
    width: 1px;
    border-radius: 4px;
}
</style>
