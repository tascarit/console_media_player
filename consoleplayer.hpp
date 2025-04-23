#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>
#include <chrono>
#include <utility>
#include <thread>
#include <string>

#include "defs.hpp"
#include "yt_extractor.hpp"
#include "utility.hpp"

#if defined(__linux__)
#define PLATFORM "linux"
#elif defined(_WIN32) 
#define PLATFORM "windows"
#else
#define PLATFORM "windows"
#endif

#define ARGS_COUNT 10

int PowerCheck(int power);
std::vector<std::string> Bufferize(cv::VideoCapture cap, int contrast, std::vector<int> args);
int to_34(int index);
std::string TranslateToAscii(cv::Mat image, int contrast, cv::Mat color, int width, int height, std::vector<int> args);
std::string GetANSIICode(cv::Vec3b pixel, bool pixelated);
std::string Pixelated(cv::Mat image, cv::Mat color, int width, int height, std::vector<int> args);

void Start(int argc, char* argv[]) {
	int contrast = 1;
	std::string path = "";
	int fps = 60;

	std::vector<int> args(ARGS_COUNT);

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i - 1], "-p") == 0) {
			path = argv[(int)i];
		}
		else if (strcmp(argv[i - 1], "-c") == 0) {
			contrast = atoi(argv[i]);
		}
		else if (strcmp(argv[i], "--video") == 0) {
			args[0] = 1;
		}
		else if (strcmp(argv[i - 1], "-s") == 0) {
			args[7] = atof(argv[i]);
		}
		else if (strcmp(argv[i - 1], "-fps") == 0) {
			fps = atof(argv[i]);
		}
		else if (strcmp(argv[i], "--advanced") == 0) {
			args[1] = 1;
		}
		else if (strcmp(argv[i], "--old") == 0) {
			args[2] = 1;
		}
		else if (strcmp(argv[i], "--realtime") == 0) {
			args[3] = 1;
		}
		else if (strcmp(argv[i], "--color") == 0) {
			args[4] = 1;
		}
		else if (strcmp(argv[i], "--link") == 0) {
			args[5] = 1;
		}
		else if (strcmp(argv[i], "--pixel") == 0) {
			args[6] = 1;
		}
		else if (strcmp(argv[i], "--audio") == 0) {
			args[8] = 1;
		}
		else if (strcmp(argv[i], "--help") == 0) {
			args[9] = 1;
		}
	}

	if (args[9] == 1)
	{
		HelpCommand();
		return;
	}

	if (path.size() == 0) {
		std::cout << "No path has been passed." << path << std::endl;
		exit(0);
	}
	if (args[7] < 0.1) {
		args[7] = 0.1;
	}
	
	// IDK Should i keep or delete this part because for me it works fine without enabling terminal processing... (windows 11)

	/*if (colored) {
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hConsole == INVALID_HANDLE_VALUE) {
			std::cerr << "Error Getting console handle\n";
			return;
		}

		DWORD mode;
		if (!GetConsoleMode(hConsole, &mode)) {
			std::cerr << "Error Getting console mode\n";
			return;
		}

		if (!SetConsoleMode(hConsole, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
			std::cerr << "Error setting console mode\n";
			return;
		}

		CloseHandle(hConsole);
	}*/

	fps = 1000 / fps;

	if (args[0] != 1) {
		cv::Mat image = cv::imread(path);
		cv::Mat mat;

		int width = (image.size().width / 12)* args[7];
		int height = (image.size().height / 24) * args[7];

		cv::resize(image, image, cv::Size(width, height), 0.5, 0.5, cv::INTER_LINEAR);

		cv::cvtColor(image, mat, cv::COLOR_BGR2GRAY);

		std::string frame = (args[6] == 1) ? Pixelated(mat, image, width, height, args) : TranslateToAscii(mat, contrast, image, width, height, args);

		std::cerr << frame << std::endl;

		mat.release();
		image.release();
	}
	else {
		if (args[5])
			path = GetStreamUrl(path);

		cv::VideoCapture cap(path, cv::CAP_FFMPEG);

		if (!cap.isOpened()) {
			std::cerr << "Error opening stream" << std::endl;
			return;
		}

		std::ios_base::sync_with_stdio(false);
		std::cin.tie(NULL);

		if (args[3] == 1) {
			cv::Mat frame;

			while (cap.read(frame)) {

				cv::Mat gray;

				int w = (frame.size().width / 12) * args[7];
				int h = (frame.size().height / 24) * args[7];

				std::pair<int, int> wh = GetTerminalSize();

				double width = wh.first;
				double height = h*(width/w);

				cv::resize(frame, frame, cv::Size(width, height), 0.5, 0.5);
				cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

				std::string output = (args[6] == 1) ? Pixelated(gray, frame, width, height, args) : TranslateToAscii(gray, contrast, frame, width, height, args);
				std::cerr << output << std::endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(fps*2));
					
				frame.release();
				gray.release();

				(PLATFORM == "windows") ? system("cls") : system("clear");
			}

			cap.release();
		}
		else {
			std::vector<std::string> videoMatrix = Bufferize(cap, contrast, args);

			cap.release();

			for (size_t i = 0; i < videoMatrix.size(); i++) {
				std::cerr << videoMatrix[i] << std::endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(fps*2));

				(PLATFORM == "windows") ? system("cls") : system("clear");
			}
		}
	}

	exit(0);
}

int PowerCheck(int power) { return (0 <= power <= 8) ? abs(power) : ((power < 0) ? 0 : ((power > 8) ? 8 : 0)); }

std::vector<std::string> Bufferize(cv::VideoCapture cap, int contrast, std::vector<int> args) {
	std::vector<std::string> ret;
	cv::Mat image;
	int totalFrames = cap.get(cv::CAP_PROP_FRAME_COUNT);
	int currentFrame = 1;

	while (cap.read(image)) {

		cv::Mat gray;

		int width = (image.size().width / 12) * args[7];
		int height = (image.size().height / 24) * args[7];

		cv::resize(image, image, cv::Size(width, height), 0.5, 0.5, cv::INTER_LINEAR);
		cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

		std::string frame = (args[6] == 1) ? Pixelated(gray, image, width, height, args) : TranslateToAscii(gray, contrast, image, width, height, args);
		ret.push_back(frame);

		std::cerr << "Buffering...\n[";
		int pos = (currentFrame * 100) / totalFrames;
		for (int i = 0; i < 100; i++) {
			if (i <= pos) std::cerr << "=";
			else std::cerr << " ";
		}
		std::cerr << "]";
		std::fflush(stdout);

		currentFrame++;
		image.release();
	}

	return ret;
}

std::string TranslateToAscii(cv::Mat image, int contrast, cv::Mat color, int width, int height, std::vector<int> args) {
	std::string ret;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			char c = ' ';
			if (args[6] != 1) {
				int power = (args[2] == 1) ? round(abs(log2(image.at<uint_fast8_t>(i, j)))) : ((args[1] == 1) ? to_34(abs(image.at<uint_fast8_t>(i, j))) : image.at<uint_fast8_t>(i, j) / (256 / strlen(alphabet)));
				c = (power > 0 && power > contrast) ? ((args[1] != 1) ? alphabet[power] : advancedAlphabet[power]) : ' ';
			}

			cv::Vec3b clr = color.at<cv::Vec3b>(i, j);
			std::string color = GetANSIICode(clr, false);

			ret += (args[4] == 1) ? (color + c + "\033[0m") : (" ");
		}
		ret += "\n";
	}

	return ret;
}

std::string Pixelated(cv::Mat image, cv::Mat color, int width, int height, std::vector<int> args) {
	std::string ret;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			char c = ' ';

			cv::Vec3b clr = color.at<cv::Vec3b>(i, j);
			std::string color = GetANSIICode(clr, true);

			ret += color + c + "\033[0m";
		}
		ret += "\n";
	}

	return ret;
}

int to_34(int index) {
	return (34 * ((100 * index) / 256)) / 100;
}

std::string GetANSIICode(cv::Vec3b pixel, bool pixelated) {
	int blue = pixel[0];
	int green = pixel[1];
	int red = pixel[2];

	return ((!pixelated) ? "\033[38;2;" : "\033[48;2;") + std::to_string(red) + ';' + std::to_string(green) + ';' + std::to_string(blue) + 'm';
}