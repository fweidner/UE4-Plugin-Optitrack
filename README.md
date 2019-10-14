# UE4-Plugin-Optitrack
Bascially just simple implementation of the SimpleClient in the NatNetSDK.

## UE4 version
* Tested with 4.23

## Use with packaged game
* If you use the plugin with a packaged game, place the NatNetSDK in a folder called ThirdParty in the root directory of your app.
* Currently just supports RigidBodies

## Known issues:
* If nothing works but connection can be established, check connection type in Motive (Unicast/Multicast) and switch it.