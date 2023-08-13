![alt text](https://i.imgur.com/QtJHdie.png)

# Compile
**Arch 2023**:
```
sudo pacman -S g++ qt5-base qt5-tools cmake make
git clone https://github.com/oldteamhost/nesca-viewer.git
cd nesca-viewer
cmake .
make -j12
```
**Debian 12**:
```
sudo apt-get install g++ qt5-base qt5-tools cmake make
git clone https://github.com/oldteamhost/nesca-viewer.git
cd nesca-viewer
cmake .
make -j12
```
