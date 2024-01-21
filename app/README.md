## Getting started  
1. Get all the dependencies
2. copy both configs, change the appId to something else in [capacitor.config.json](capacitor.config.json), config is final so mae sure the app name is what you want
3. Add vite/vue to top level directory (app source will be view/dist), `npm run build`
4. `npx cap add ios`
5. Change stuff from xcode `npx cap open ios`, select the `APP` folder, a tab should open up, go to `signing and capabilities`, make sure a developer is selected an the appId is in the budle identifier spot, you probably dont need to do this again, choose the simulator/device (extra work for first time dev on new device)
7. Add the following to [app/info.plist](ios/App/App/Info.plist)

Right under
```xml
<key>CFBundleDisplayName</key>
<string>Pebble</string>
```
Add
```xml
<key>NSBluetoothAlwaysUsageDescription</key>
<string>Uses Bluetooth to connect and interact with peripheral BLE devices.</string>
<key>UIBackgroundModes</key>
<array>
<string>bluetooth-central</string>
</array>
```
6. close xcode
7. `npx cap run ios`

## Dev workflow
1. Edit /view
2. Edit /ios (mostly plist stuff),  `npm run build`
2. npx cap sync ios
3. npx cap run ios

# Resources
- [https://capacitorjs.com/docs/ios](https://capacitorjs.com/docs/ios)
- [How to do generate and use icons with capacitor](https://capacitorjs.com/docs/guides/splash-screens-and-icons)
- [BLE plugin](https://github.com/capacitor-community/bluetooth-le)
- [live updates](https://capacitorjs.com/docs/guides/live-reload)
- [capacitor.config.json](https://capacitorjs.com/docs/config)