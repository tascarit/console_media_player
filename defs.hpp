#pragma once

const char alphabet[11] = " .:-=+*#%@";
const char advancedAlphabet[34] = "      ;*~-+^!\\/|1()[]{}<>?&%0$#@";

std::string helpCommand = "--help (shows this text)\n"
"-p {path} (path to the image / video file)\n"
"-c {number} (darkenify the frame, the more number is, the more dark parts become more darker)\n"
"-fps {number} (thats supposed to be FPS but image prints to the console already kinda slow, default is 60)\n"
"-s {number} (frame size multiplier, this parameter can be floating point number)\n"
"--video (to pass videos)\n"
"--advanced (to use 34 ASCII symbols including 0 and 1)\n"
"--realtime (no general video buffering, but frame buffering will still work)\n"
"--old (use old logariphmic pixel to character converting)\n"
"--color (translates to colored symbols, this works only on windows 10 + / linux / unix and only in console)\n"
"--pixel (doesn't print ASCII characters, prints colored whitespaces instead)\n"
"--gs (gray scale colored format)\n"
"--link (use this flag if the path provided is youtube video link)\n\n";