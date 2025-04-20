# console_media_player
Can show images or play mp4 files in console using OpenCV

### DEAD BRAINCELLS CODE JUMPSCARE ALERT!!!

## [Basic and currently existing parameters]

-p {path} *(path to the image/video file)*

-c {number} *(the darkenify of the frame, the more number is, the more dark parts become more darker)*

-fps {number} *(thats supposed to be FPS but image prints to the console already kinda slow, default is 60)*

-s {number} *(frame size multiplier, suggesting to not exceed 1, this parameter can be floating point number)*

--video *(to pass videos)*

--advanced *(to use all 92 ASCII printable characters)*

**the {} symbols should not be added**

Preview image:

![alt text](https://i.ibb.co/0RdCHgST/20-04-2025-203447.jpg)


Linux installation guide:

Download and build opencv: [link](https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html)

`g++ main.cpp -o cmp` if not works try this: `g++ main.cpp -o cmp -I{Path to OpenCV include} -L{Path to OpenCV Libraries}`
