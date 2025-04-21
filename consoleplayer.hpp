#include <string>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <chrono>
#include <thread>

#include "defs.hpp"

#if defined(__linux__)
#define PLATFORM "linux"
#elif defined(_WIN32) 
#define PLATFORM "windows"
#include <Windows.h>
#else
#define PLATFORM "windows"
#endif

int PowerCheck(int power);
std::vector<std::string> Bufferize(cv::VideoCapture cap, int contrast, bool advanced, bool old, double s, bool colored);
int to_34(int index);
std::string TranslateToAscii(cv::Mat image, int contrast, bool advanced, bool video, bool old, cv::Mat color, bool colored);
std::string GetANSIICode(cv::Vec3b pixel);

void Start(int argc, char* argv[]) {
	int contrast = 1;
	std::string path = "";
	double s = 1;
	int fps = 60;
	bool video = false;
	bool old = false;
	bool advanced = false;
	bool realtime = false;
	bool colored = false;

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i - 1], "-p") == 0) {
			path = argv[(int)i];
		}
		else if (strcmp(argv[i - 1], "-c") == 0) {
			contrast = atoi(argv[i]);
		}
		else if (strcmp(argv[i], "--video") == 0) {
			video = true;
		}
		else if (strcmp(argv[i - 1], "-s") == 0) {
			s = atof(argv[i]);
		}
		else if (strcmp(argv[i - 1], "-fps") == 0) {
			fps = atof(argv[i]);
		}
		else if (strcmp(argv[i], "--advanced") == 0) {
			advanced = true;
		}
		else if (strcmp(argv[i], "--old") == 0) {
			old = true;
		}
		else if (strcmp(argv[i], "--realtime") == 0) {
			realtime=true;
		}
		else if (strcmp(argv[i], "--color") == 0) {
			colored = true;
		}
	}

	if (path.size() == 0) {
		std::cout << "No path has been passed." << path << std::endl;
		exit(0);
	}
	if (s < 0.1) {
		s = 0.1;
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

	if (!video) {
		cv::Mat image = cv::imread(path);
		cv::Mat mat;

		cv::resize(image, image, cv::Size((image.size().width / 12) * s, (image.size().height / 24) * s), 0.5, 0.5, cv::INTER_LINEAR);

		cv::cvtColor(image, mat, cv::COLOR_BGR2GRAY);

		std::string frame = TranslateToAscii(mat, contrast, advanced, false, old, image, colored);

		std::cerr << frame << std::endl;

		mat.release();
		image.release();
	}
	else {
		cv::VideoCapture cap(path);

		std::ios_base::sync_with_stdio(false);
		std::cin.tie(NULL);

		if (realtime) {
			cv::Mat frame;

			while (cap.read(frame)) {

				cv::Mat gray;

				cv::resize(frame, frame, cv::Size((frame.size().width / 12) * s, (frame.size().height / 24) * s), 0.5, 0.5);
				cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

				std::string output = TranslateToAscii(gray, contrast, advanced, video, old, frame, colored);
				std::cerr << output << std::endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(fps*2));
					
				frame.release();
				gray.release();

				(PLATFORM == "windows") ? system("cls") : system("clear");
			}

			cap.release();
		}
		else {
			std::vector<std::string> videoMatrix = Bufferize(cap, contrast, advanced, old, s, colored);

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

std::vector<std::string> Bufferize(cv::VideoCapture cap, int contrast, bool advanced, bool old, double s, bool colored) {
	std::vector<std::string> ret;
	cv::Mat image;
	int totalFrames = cap.get(cv::CAP_PROP_FRAME_COUNT);
	int currentFrame = 1;

	while (cap.read(image)) {

		cv::Mat gray;

		cv::resize(image, image, cv::Size(round((image.size().width / 12) * s), round((image.size().height / 24) * s)), 0.5, 0.5, cv::INTER_LINEAR);
		cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

		std::string frame = TranslateToAscii(gray, contrast, advanced, true, old, image, colored);
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

std::string TranslateToAscii(cv::Mat image, int contrast, bool advanced, bool video, bool old, cv::Mat color, bool colored) {
	std::string ret;
	int width = image.size().width;
	int height = image.size().height;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int power = (old) ? round(abs(log2(image.at<uint_fast8_t>(i, j)))) : ((advanced) ? to_34(abs(image.at<uint_fast8_t>(i, j))) : image.at<uint_fast8_t>(i, j) / (256 / strlen(alphabet)));
			char c = (power > 0 && power > contrast) ? ((!advanced) ? alphabet[power] : advancedAlphabet[power]) : ' ';

			cv::Vec3b pixel = color.at<cv::Vec3b>(i, j);
			std::string color = GetANSIICode(pixel);

			ret += (colored) ? ((c != ' ') ? (color + c + "\033[0m") : (" ")) : std::string(1,c);
		}
		ret += "\n";
	}

	return ret;
}

int to_34(int index) {
	return (34 * ((100 * index) / 256)) / 100;
}

std::string GetANSIICode(cv::Vec3b pixel) {
	int blue = pixel[0];
	int green = pixel[1];
	int red = pixel[2];

	return "\033[38;2;" + std::to_string(red) + ';' + std::to_string(green) + ';' + std::to_string(blue) + 'm';
}