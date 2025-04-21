#include <string>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

#include "defs.hpp"

#if defined(__linux__)
#define PLATFORM "linux"
#elif defined(_WIN32) 
#define PLATFORM "windows"
#else
#define PLATFORM "windows"
#endif

#define ARGS_SIZE 8

void ArgProcess(int argc, char* argv[], double **args);
int PowerCheck(int power);
std::vector<std::string> Bufferize(cv::VideoCapture cap, int contrast, bool advanced, bool old, double s);
int to_34(int index);
std::string TranslateToAscii(cv::Mat image, int contrast, bool advanced, bool video, bool old);

void Start(int argc, char* argv[]) {

	double* args = (double*) malloc(sizeof(double)*ARGS_SIZE);
	
	for (int i = 0; i < ARGS_SIZE; i++) {
		args[i] = -1;
	}

	int contrast = 1;
	std::string path = "";
	double s = 1;
	int fps = 60;
	bool video = false;
	bool old = false;
	bool advanced = false;
	bool realtime = false;

	ArgProcess(argc, argv, &args);

	for (int i = 0; i < ARGS_SIZE; i++) {
		std::cout << args[i] << std::endl;
		if ((int)args[i] != -1) {
			switch (i) {
			case 0:
				path = argv[(int)args[i]];
				break;
			case 1:
				contrast = args[i];
				break;
			case 3:
				s = args[i];
				break;
			case 2:
				video = true;
				break;
			case 4:
				fps = (int)args[i];
				break;
			case 5:
				advanced = true;
				break;
			case 6:
				old = true;
				break;
			case 7:
				realtime = true;
				break;
			default:
				break;
			}
		}
	}

	free(args);

	if (path.size() == 0) {
		std::cout << "No path has been passed." << path << std::endl;
		exit(0);
	}
	if (s < 0.1) {
		s = 0.1;
	}

	fps = 1000 / fps;

	if (!video) {
		cv::Mat mat = cv::imread(path, 0);
		cv::resize(mat, mat, cv::Size((mat.size().width / 12) * s, (mat.size().height / 24) * s), 0.5, 0.5, cv::INTER_LINEAR);

		std::string frame = TranslateToAscii(mat, contrast, advanced, false, old);

		std::cerr << frame << std::endl;

		mat.release();
	}
	else {
		cv::VideoCapture cap(path);

		std::ios_base::sync_with_stdio(false);
		std::cin.tie(NULL);

		if (realtime) {
			cv::Mat frame;

			while (cap.read(frame)) {

				cv::resize(frame, frame, cv::Size((frame.size().width / 24) * s, (frame.size().height / 24) * s), 0.5, 0.5, cv::INTER_LINEAR);

				std::string output = TranslateToAscii(frame, contrast, advanced, video, old);
				std::cerr << output << std::endl;
				cv::waitKey(fps);

				frame.release();

				(PLATFORM == "windows") ? system("cls") : system("clear");
			}

			cap.release();
		}
		else {
			std::vector<std::string> videoMatrix = Bufferize(cap, contrast, advanced, old, s);

			cap.release();

			for (size_t i = 0; i < videoMatrix.size(); i++) {
				std::cerr << videoMatrix[i] << std::endl;
				cv::waitKey(fps);

				(PLATFORM == "windows") ? system("cls") : system("clear");
			}
		}
	}

	exit(0);
}

void ArgProcess(int argc, char* argv[], double** args) {

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i - 1], "-p") == 0) {
			*args[0] = i;
		}
		else if (strcmp(argv[i - 1], "-c") == 0) {
			*args[1] = atoi(argv[i]);
		}
		else if (strcmp(argv[i], "--video") == 0) {
			*args[2] = 1;
		}
		else if (strcmp(argv[i - 1], "-s") == 0) {
			*args[3] = atof(argv[i]);
		}
		else if (strcmp(argv[i - 1], "-fps") == 0) {
			*args[4] = atof(argv[i]);
		}
		else if (strcmp(argv[i], "--advanced") == 0) {
			*args[5] = 1;
		}
		else if (strcmp(argv[i], "--old") == 0) {
			*args[6] = 1;
		}
		else if (strcmp(argv[i], "--realtime") == 0) {
			*args[7] = 1;
		}
	}
}

int PowerCheck(int power) { return (0 <= power <= 8) ? abs(power) : ((power < 0) ? 0 : ((power > 8) ? 8 : 0)); }

std::vector<std::string> Bufferize(cv::VideoCapture cap, int contrast, bool advanced, bool old, double s) {
	std::vector<std::string> ret;
	cv::Mat image;
	int totalFrames = cap.get(cv::CAP_PROP_FRAME_COUNT);
	int currentFrame = 1;

	while (cap.read(image)) {
		cv::resize(image, image, cv::Size(round((image.size().width / 24) * s), round((image.size().height / 24) * s)), 0.5, 0.5, cv::INTER_LINEAR);
		std::string frame = TranslateToAscii(image, contrast, advanced, true, old);
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

std::string TranslateToAscii(cv::Mat image, int contrast, bool advanced, bool video, bool old) {
	std::string ret;
	int width = (!video) ? image.size().width : image.size().width * 3;
	int height = image.size().height;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int power = (old) ? round(abs(log2(image.at<uint_fast8_t>(i, j)))) : ((advanced) ? to_34(abs(image.at<uint_fast8_t>(i, j))) : image.at<uint_fast8_t>(i, j) / (256 / strlen(alphabet)));
			char c = (power > 0 && power > contrast) ? ((!advanced) ? alphabet[power] : advancedAlphabet[power]) : ' ';
			ret += c;
		}
		ret += "\n";
	}

	return ret;
}

int to_34(int index) {
	return (34 * ((100 * index) / 256)) / 100;
}