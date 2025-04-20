#include <string>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

#include "defs.hpp"

double* ArgProcess(int argc, char* argv[]);
int PowerCheck(int power);
void FrameShow(cv::Mat image, int contrast, bool advanced, bool video);
int to_92(int index);

void Start(int argc, char* argv[]) {

	double* args = ArgProcess(argc, argv);
	int contrast = 1;
	std::string path = "";
	double s = 1;
	int fps = 60;
	bool video = false;
	bool advanced = false;

	for (int i = 0; i < 7; i++) {
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
			default:
				break;
			}
		}
	}

	if (path.size() == 0) {
		std::cout << "No path has been passed." << path << std::endl;
		exit(0);
	}
	if (s < 0.1) {
		s = 0.1;
	}

	fps = 1000 / fps * 2;

	if (!video) {
		cv::Mat mat = cv::imread(path, 0);
		cv::Mat image;
		cv::Size startSize = mat.size();
		cv::Size endsize((startSize.width / 12) * s, (startSize.height / 24) * s);

		cv::resize(mat, image, endsize, 0.5, 0.5, cv::INTER_LINEAR);

		FrameShow(image, contrast, advanced, video);
	}
	else {
		cv::VideoCapture cap(path);
		cv::Mat img;
		cv::Mat frame;

		std::ios_base::sync_with_stdio(false);
		std::cin.tie(NULL);

		while (true) {
			cap.read(img);
			cv::Size startSize = img.size();
			cv::Size endsize((startSize.width / 24) * s, (startSize.height / 24) * s);

			cv::resize(img, frame, endsize, 0.5, 0.5, cv::INTER_LINEAR);
			FrameShow(frame, contrast, advanced, video);
			cv::waitKey(fps);
			system("cls");
		}
	}

	exit(0);
}

double* ArgProcess(int argc, char* argv[]) {
	double args[6] = { -1, -1, -1, -1, -1, -1 };

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i - 1], "-p") == 0) {
			args[0] = i;
		}
		else if (strcmp(argv[i - 1], "-c") == 0) {
			args[1] = atoi(argv[i]);
		}
		else if (strcmp(argv[i], "--video") == 0) {
			args[2] = 1;
		}
		else if (strcmp(argv[i - 1], "-s") == 0) {
			args[3] = atof(argv[i]);
		}
		else if (strcmp(argv[i - 1], "-fps") == 0) {
			args[4] = atof(argv[i]);
		}
		else if (strcmp(argv[i], "--advanced") == 0) {
			args[5] = 1;
		}
	}

	return args;
}

int PowerCheck(int power) { return (0 <= power <= 8) ? abs(power) : ((power < 0) ? 0 : ((power > 8) ? 8 : 0)); }

void FrameShow(cv::Mat image, int contrast, bool advanced, bool video) {
	int width = (!video) ? image.size().width : image.size().width*3;
	int height = image.size().height;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int power = (advanced) ? to_92(abs(image.at<uint_fast8_t>(i, j))) : ceil(abs(log2(image.at<uint_fast8_t>(i, j))));
			char c = (power > 0 && power > contrast) ? ((!advanced) ? alphabet[power] : advancedAlphabet[power]) : ' ';
			std::cerr << c;
		}
		std::cerr << std::endl;
	}
}

int to_92(int index) {
	return (92 * ((100 * index) / 256)) / 100;
}