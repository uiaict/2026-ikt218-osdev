# Pulseaudio

to start:

```sh
brew services start pulseaudio
pactl load-module module-native-protocol-tcp auth-anonymous=1
lsof -i -P | grep -i "listen"
```