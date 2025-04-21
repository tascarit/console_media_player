# console_media_player
Can show images or play mp4 files in console using OpenCV

### DEAD BRAINCELLS CODE JUMPSCARE ALERT!!!

## [Basic and currently existing parameters]

-p {path} *(path to the image/video file)*

-c {number} *(the darkenify of the frame, the more number is, the more dark parts become more darker)*

-fps {number} *(thats supposed to be FPS but image prints to the console already kinda slow, default is 60)*

-s {number} *(frame size multiplier, suggesting to not exceed 1, this parameter can be floating point number)*

--video *(to pass videos)*

--advanced *(to use 34 ASCII symbols including 0 and 1)*

--realtime *(no general video buffering, but frame buffering will still work)*

--old *(use old logariphmic pixel to character converting)*

**the {} symbols should not be added**

Preview image:

![alt text](https://i.imgur.com/4xVG5bI.jpeg)


Linux (Debian 12) installation guide:

Dependencies : git 2.39.5 or higher, cmake 3.25.1 or higher, make v4.3 or higher, g++ v12.2 or higher  

Download and build [opencv](https://github.com/opencv/opencv)

Example:
```
cd ~/Downloads
mkdir opencv_installation
cd opencv_installation
git clone https://github.com/opencv/opencv
cd opencv
mkdir build
cd build
cmake ..
make -j4
sudo make install
```

When OpenCV is installed, download and build CMP:

```
cd ~/Downloads
git clone https://github.com/tascarit/console_media_player
cd console_media_player
g++ main.cpp -o cmp -I/usr/local/include/opencv4 -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui -lopencv_videoio -lopencv_core
```

Example usage:

```
./main -p PATH --video --realtime
```

