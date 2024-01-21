<template>
  <div>     
   <h1 style="text-align:center">TEST!!!</h1>
      <div class="button" @touchstart="hapticsImpactMedium">
        hapticsImpactMedium
      </div>

      <div class="button" @touchstart="hapticsImpactLight">
        hapticsImpactLight
      </div >

      <div class="button" @touchstart="hapticsVibrate">
        hapticsVibrate
      </div>

      <div class="button" @touchstart="hapticsSelectionStart">
        hapticsSelectionStart
      </div>

      <div class="button" @touchstart="hapticsSelectionChanged">
        hapticsSelectionChanged
      </div>

      <div class="button" @touchstart="hapticsSelectionEnd">
        hapticsSelectionEnd
      </div>
    </div>
</template>
  
  <script>
import { Haptics, ImpactStyle } from '@capacitor/haptics'

export default{
  methods:{
    async hapticsImpactMedium() {
      await Haptics.impact({ style: ImpactStyle.Medium });
    },
    async hapticsImpactLight() {
      await Haptics.impact({ style: ImpactStyle.Light });
    },
    async hapticsVibrate() {
      await Haptics.vibrate();
    },
    async hapticsSelectionStart  (){
      await Haptics.selectionStart();
    },
    async hapticsSelectionChanged(){
      await Haptics.selectionChanged();
    },
    async hapticsSelectionEnd(){
      await Haptics.selectionEnd();
    }
  }
}
</script>

<style>
body {
  padding: env(safe-area-inset-top, 20px) env(safe-area-inset-right, 20px) env(
      safe-area-inset-bottom,
      20px
    ) env(safe-area-inset-left, 20px);
}
.button{
  background: grey;
  width: min-content;
}
 .button:active{
   opacity: .8;
 }
</style>
