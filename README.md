![alt text](https://i.imgur.com/QtJHdie.png)

## About
Просмотр результато сканирования пока только nesca4, в gui на qt.
Для работы поместите ваши файлы nesca4 json в папку data.
Также что бы были картинки, нужно в nesca4 включить -s.

## Compile
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
