// EXAMPLE USAGE

/*
		[EXISTING PARAMETERS]

-p {path} (path to the image/video file)
-c {number} (the darkenify of the frame, the more number is, the more dark parts become more darker)
-fps {number} (thats supposed to be FPS but image prints to the console already kinda slow, default is 60)
-s {number} (frame size multiplier, suggesting to not exceed 1, this parameter can be floating point number)
--video (to pass videos)
--advanced (to use all 92 ASCII printable characters)

the {} symbols should not be added

DEAD BRAINCELLS CODE JUMPSCARE ALERT
*/

#include "consoleplayer.hpp"

int main(int argc, char* argv[]) {
	Start(argc, argv);
	return 0;
}

/*
	The consoleplayer.hpp part firstly was the .cpp file, moved it into .hpp to just show you can use this as a library if needed.
*/