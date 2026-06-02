// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include <opencv2/core/utils/logger.hpp>

#include <queue>
#include <vector>
#include <cmath> 

wchar_t* projectPath;


///SECTIUNE PENTRU HEADERS LA FUNCTII
void filterSmallComponents(Mat& img, int areaThreshold);


///SFARSIT SECTIUNE PENTRU HEADERS LA FUNCTII

void testOpenImage()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		Mat src;
		src = imread(fname, IMREAD_GRAYSCALE);
		printf("type = %d\n", src.type());
		imshow("image",src);
		waitKey();
	}
}

void testOpenImagesFld()
{
	char folderName[MAX_PATH];
	if (openFolderDlg(folderName)==0)
		return;
	char fname[MAX_PATH];
	FileGetter fg(folderName,"bmp");
	while(fg.getNextAbsFile(fname))
	{
		Mat src;
		src = imread(fname);
		imshow(fg.getFoundFileName(),src);
		if (waitKey()==27) //ESC pressed
			break;
	}
}

void testImageOpenAndSave()
{
	_wchdir(projectPath);

	Mat src, dst;

	src = imread("Images/Lena_24bits.bmp", IMREAD_COLOR);	// Read the image

	if (!src.data)	// Check for invalid input
	{
		printf("Could not open or find the image\n");
		return;
	}

	// Get the image resolution
	Size src_size = Size(src.cols, src.rows);

	// Display window
	const char* WIN_SRC = "Src"; //window for the source image
	namedWindow(WIN_SRC, WINDOW_AUTOSIZE);
	moveWindow(WIN_SRC, 0, 0);

	const char* WIN_DST = "Dst"; //window for the destination (processed) image
	namedWindow(WIN_DST, WINDOW_AUTOSIZE);
	moveWindow(WIN_DST, src_size.width + 10, 0);

	cvtColor(src, dst, COLOR_BGR2GRAY); //converts the source image to a grayscale one

	imwrite("Images/Lena_24bits_gray.bmp", dst); //writes the destination to file

	imshow(WIN_SRC, src);
	imshow(WIN_DST, dst);

	waitKey(0);
}

void testNegativeImage()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		double t = (double)getTickCount(); // Get the current time [s]
		
		Mat src = imread(fname,IMREAD_GRAYSCALE);
		int height = src.rows;
		int width = src.cols;
		Mat dst = Mat(height,width,CV_8UC1);
		// Accessing individual pixels in an 8 bits/pixel image
		// Inefficient way -> slow
		for (int i=0; i<height; i++)
		{
			for (int j=0; j<width; j++)
			{
				uchar val = src.at<uchar>(i,j);
				uchar neg = 255 - val;
				dst.at<uchar>(i,j) = neg;
			}
		}

		// Get the current time again and compute the time difference [s]
		t = ((double)getTickCount() - t) / getTickFrequency();
		// Print (in the console window) the processing time in [ms] 
		printf("Time = %.3f [ms]\n", t * 1000);

		imshow("input image",src);
		imshow("negative image",dst);
		waitKey();
	}
}

void testNegativeImageFast()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_GRAYSCALE);
		int height = src.rows;
		int width = src.cols;
		Mat dst = src.clone();

		double t = (double)getTickCount(); // Get the current time [s]

		// The fastest approach of accessing the pixels -> using pointers
		uchar *lpSrc = src.data;
		uchar *lpDst = dst.data;
		int w = (int) src.step; // no dword alignment is done !!!
		for (int i = 0; i<height; i++)
			for (int j = 0; j < width; j++) {
				uchar val = lpSrc[i*w + j];
				lpDst[i*w + j] = 255 - val;
			}

		// Get the current time again and compute the time difference [s]
		t = ((double)getTickCount() - t) / getTickFrequency();
		// Print (in the console window) the processing time in [ms] 
		printf("Time = %.3f [ms]\n", t * 1000);

		imshow("input image",src);
		imshow("negative image",dst);
		waitKey();
	}
}

void testColor2Gray()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		Mat src = imread(fname);

		int height = src.rows;
		int width = src.cols;

		Mat dst = Mat(height,width,CV_8UC1);

		// Accessing individual pixels in a RGB 24 bits/pixel image
		// Inefficient way -> slow
		for (int i=0; i<height; i++)
		{
			for (int j=0; j<width; j++)
			{
				Vec3b v3 = src.at<Vec3b>(i,j);
				uchar b = v3[0];
				uchar g = v3[1];
				uchar r = v3[2];
				dst.at<uchar>(i,j) = (r+g+b)/3;
			}
		}
		
		imshow("input image",src);
		imshow("gray image",dst);
		waitKey();
	}
}

void testBGR2HSV()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname);
		int height = src.rows;
		int width = src.cols;

		// HSV components
		Mat H = Mat(height, width, CV_8UC1);
		Mat S = Mat(height, width, CV_8UC1);
		Mat V = Mat(height, width, CV_8UC1);

		// Defining pointers to each matrix (8 bits/pixels) of the individual components H, S, V 
		uchar* lpH = H.data;
		uchar* lpS = S.data;
		uchar* lpV = V.data;

		Mat hsvImg;
		cvtColor(src, hsvImg, COLOR_BGR2HSV);

		// Defining the pointer to the HSV image matrix (24 bits/pixel)
		uchar* hsvDataPtr = hsvImg.data;

		for (int i = 0; i<height; i++)
		{
			for (int j = 0; j<width; j++)
			{
				int hi = i*width * 3 + j * 3;
				int gi = i*width + j;

				lpH[gi] = hsvDataPtr[hi] * 510 / 360;	// lpH = 0 .. 255
				lpS[gi] = hsvDataPtr[hi + 1];			// lpS = 0 .. 255
				lpV[gi] = hsvDataPtr[hi + 2];			// lpV = 0 .. 255
			}
		}

		imshow("input image", src);
		imshow("H", H);
		imshow("S", S);
		imshow("V", V);

		waitKey();
	}
}

void testResize()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		Mat dst1,dst2;
		//without interpolation
		resizeImg(src,dst1,320,false);
		//with interpolation
		resizeImg(src,dst2,320,true);
		imshow("input image",src);
		imshow("resized image (without interpolation)",dst1);
		imshow("resized image (with interpolation)",dst2);
		waitKey();
	}
}

void testCanny()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		Mat src,dst,gauss;
		src = imread(fname,IMREAD_GRAYSCALE);
		double k = 0.4;
		int pH = 50;
		int pL = (int) k*pH;
		GaussianBlur(src, gauss, Size(5, 5), 0.8, 0.8);
		Canny(gauss,dst,pL,pH,3);
		imshow("input image",src);
		imshow("canny",dst);
		waitKey();
	}
}

void testVideoSequence()
{
	_wchdir(projectPath);

	VideoCapture cap("Videos/rubic.avi"); // off-line video from file
	//VideoCapture cap(0);	// live video from web cam
	if (!cap.isOpened()) {
		printf("Cannot open video capture device.\n");
		waitKey(0);
		return;
	}
		
	Mat edges;
	Mat frame;
	char c;

	while (cap.read(frame))
	{
		Mat grayFrame;
		cvtColor(frame, grayFrame, COLOR_BGR2GRAY);
		Canny(grayFrame,edges,40,100,3);
		imshow("source", frame);
		imshow("gray", grayFrame);
		imshow("edges", edges);
		c = waitKey(100);  // waits 100ms and advances to the next frame
		if (c == 27) {
			// press ESC to exit
			printf("ESC pressed - capture finished\n"); 
			break;  //ESC pressed
		};
	}
}


void testSnap()
{
	_wchdir(projectPath);

	VideoCapture cap(0); // open the deafult camera (i.e. the built in web cam)
	if (!cap.isOpened()) // openenig the video device failed
	{
		printf("Cannot open video capture device.\n");
		return;
	}

	Mat frame;
	char numberStr[256];
	char fileName[256];
	
	// video resolution
	Size capS = Size((int)cap.get(CAP_PROP_FRAME_WIDTH),
		(int)cap.get(CAP_PROP_FRAME_HEIGHT));

	// Display window
	const char* WIN_SRC = "Src"; //window for the source frame
	namedWindow(WIN_SRC, WINDOW_AUTOSIZE);
	moveWindow(WIN_SRC, 0, 0);

	const char* WIN_DST = "Snapped"; //window for showing the snapped frame
	namedWindow(WIN_DST, WINDOW_AUTOSIZE);
	moveWindow(WIN_DST, capS.width + 10, 0);

	char c;
	int frameNum = -1;
	int frameCount = 0;

	for (;;)
	{
		cap >> frame; // get a new frame from camera
		if (frame.empty())
		{
			printf("End of the video file\n");
			break;
		}

		++frameNum;
		
		imshow(WIN_SRC, frame);

		c = waitKey(10);  // waits a key press to advance to the next frame
		if (c == 27) {
			// press ESC to exit
			printf("ESC pressed - capture finished");
			break;  //ESC pressed
		}
		if (c == 115){ //'s' pressed - snap the image to a file
			frameCount++;
			fileName[0] = NULL;
			sprintf(numberStr, "%d", frameCount);
			strcat(fileName, "Images/A");
			strcat(fileName, numberStr);
			strcat(fileName, ".bmp");
			bool bSuccess = imwrite(fileName, frame);
			if (!bSuccess) 
			{
				printf("Error writing the snapped image\n");
			}
			else
				imshow(WIN_DST, frame);
		}
	}

}

void MyCallBackFunc(int event, int x, int y, int flags, void* param)
{
	//More examples: http://opencvexamples.blogspot.com/2014/01/detect-mouse-clicks-and-moves-on-image.html
	Mat* src = (Mat*)param;
	if (event == EVENT_LBUTTONDOWN)
		{
			printf("Pos(x,y): %d,%d  Color(RGB): %d,%d,%d\n",
				x, y,
				(int)(*src).at<Vec3b>(y, x)[2],
				(int)(*src).at<Vec3b>(y, x)[1],
				(int)(*src).at<Vec3b>(y, x)[0]);
		}
}

void testMouseClick()
{
	Mat src;
	// Read image from file 
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		src = imread(fname);
		//Create a window
		namedWindow("My Window", 1);

		//set the callback function for any mouse event
		setMouseCallback("My Window", MyCallBackFunc, &src);

		//show the image
		imshow("My Window", src);

		// Wait until user press some key
		waitKey(0);
	}
}

/* Histogram display function - display a histogram using bars (simlilar to L3 / Image Processing)
Input:
name - destination (output) window name
hist - pointer to the vector containing the histogram values
hist_cols - no. of bins (elements) in the histogram = histogram image width
hist_height - height of the histogram image
Call example:
showHistogram ("MyHist", hist_dir, 255, 200);
*/
void showHistogram(const std::string& name, int* hist, const int  hist_cols, const int hist_height)
{
	Mat imgHist(hist_height, hist_cols, CV_8UC3, CV_RGB(255, 255, 255)); // constructs a white image

	//computes histogram maximum
	int max_hist = 0;
	for (int i = 0; i<hist_cols; i++)
	if (hist[i] > max_hist)
		max_hist = hist[i];
	double scale = 1.0;
	scale = (double)hist_height / max_hist;
	int baseline = hist_height - 1;

	for (int x = 0; x < hist_cols; x++) {
		Point p1 = Point(x, baseline);
		Point p2 = Point(x, baseline - cvRound(hist[x] * scale));
		line(imgHist, p1, p2, CV_RGB(255, 0, 255)); // histogram bins colored in magenta
	}

	imshow(name, imgHist);
}





//PROJECT DENDRITES SPINES !!!! 




///MORFOLOGICAL OPERATIONS.

Mat applyDilation(const Mat& input) {

    Mat output = Mat::zeros(input.size(), CV_8UC1);

    int di[3][3] = {
        {-1,-1,-1},
        { 0, 0, 0},
        { 1, 1, 1}
    };

    int dj[3][3] = {
        {-1,0,1},
        {-1,0,1},
        {-1,0,1}
    };

    for (int i = 1; i < input.rows - 1; i++) {
        for (int j = 1; j < input.cols - 1; j++) {

            if (input.at<uchar>(i,j) == 255) {

                for (int ni = 0; ni < 3; ni++) {
                    for (int nj = 0; nj < 3; nj++) {

                        output.at<uchar>(
                            i + di[ni][nj],
                            j + dj[ni][nj]
                        ) = 255;
                    }
                }
            }
        }
    }

    return output;
}





Mat applyErosion(const Mat& input) {
	Mat output = Mat::zeros(input.size(), CV_8UC1);

	int di[3][3] = {
			{-1, -1, -1},
			{ 0, 0, 0},
			{1,1,1}
	};

	int dj[3][3] = {
			{-1,0,1},
			{-1,0,1},
			{-1,0,1}
	};
	bool allIsObject = true;
	for (int i = 1; i < input.rows-1; i++) {
		for (int j = 1; j < input.cols-1; j++) {
			allIsObject = 1;
			for (int ni = 0; ni < 3; ni++) {
				for (int nj = 0; nj < 3; nj++) {
					if (input.at<uchar>(i + di[ni][nj], j + dj[ni][nj]) == 0) {
						allIsObject = false; 
						break;
					}
				}
			}
			if (allIsObject) {
				output.at<uchar>(i, j) = 255;
			}
			else 
				output.at<uchar>(i, j) = 0;
		}
	}
	return output;
}






////HISTOGRAM AND GLOBAL BINARIZATION

std::vector<int> getHistogram(Mat src) {
	int height = src.rows;
	int width = src.cols;
	int M = height * width;
	std::vector<int> hist(256, 0);


	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			hist[src.at<uchar>(i, j)]++;
		}
	}

	for (int i = 0; i < 256; i++) {
		printf("%d, ", hist[i]);
		printf("\n");
	}

	showHistogram("histogram", hist.data(), 256, 256);


	return hist;

}


Mat binarization()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		double t = (double)getTickCount(); 

		Mat src = imread(fname, IMREAD_GRAYSCALE);
		int height = src.rows;
		int width = src.cols;
		Mat bin = Mat::zeros(height, width, CV_8UC1);


		int treshold_image1 = 22;
		int treshold_image2 = 38;
		int treshold_image3 = 32;

		
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {

				if (src.at<uchar>(i, j) > 70) {
					bin.at<uchar>(i, j) = 255;
				}
				else {
					bin.at<uchar>(i, j) = 0;
				}

			}
		}
		

		bin = applyDilation(bin);
		bin = applyErosion(bin);

	    filterSmallComponents(bin, 200);

		std::vector<int> hist = getHistogram(src);
		
		


		t = ((double)getTickCount() - t) / getTickFrequency();
		
		printf("Time = %.3f [ms]\n", t * 1000);

		showHistogram("histograma initiala", hist.data(), 256, 300);
		imshow("binary", bin);
	

		waitKey();

		return bin;
	}

	return Mat();

}










//// ADAPTIVE BINARIZATION

Mat myIntegralImage(const Mat& src) {
	int height = src.rows;
	int width = src.cols;

	//sumImage should be (height + 1, width + 1) to cover the entire src_image
	Mat sumImage = Mat::zeros(height + 1, width + 1, CV_32S);

	for (int i = 1; i <= height; i++) {
		for (int j = 1; j <= width; j++) {

			
			int pixelValue = src.at<uchar>(i - 1, j - 1);

			
			sumImage.at<int>(i, j) = pixelValue
				+ sumImage.at<int>(i - 1, j)
				+ sumImage.at<int>(i, j - 1)
				- sumImage.at<int>(i - 1, j - 1);
		}
	}

	return sumImage;
}

//C represents offset
Mat myAdaptiveThreshold(Mat src, int windowsSize, int C) {
	int height = src.rows;
	int width = src.cols;
	int radius = windowsSize / 2;
	Mat dst = Mat::zeros(height, width, CV_8UC1);
	

	Mat sumImage = myIntegralImage(src); 

	for (int i = radius; i < height - radius; i++) {
		for (int j = radius; j < width - radius; j++) {

			int r1 = max(0, i - radius);
			int c1 = max(0, j - radius);

			int r2 = min(height-1, i + radius);
			int c2 = min(width - 1, j + radius);

			int A = sumImage.at<int>(r1, c1);
			int B = sumImage.at<int>(r1, c2 + 1);
			int C_val = sumImage.at<int>(r2 + 1, c1); 
			int D = sumImage.at<int>(r2 + 1, c2 + 1);

			int sum = D + A - B - C_val;
			int area = (r2 - r1 + 1) * (c2 - c1 + 1);

			int localT = sum / area;
			localT = localT - C;

			if (src.at<uchar>(i, j) > localT) {
				dst.at<uchar>(i, j) = 255;
			}
			else
				dst.at<uchar>(i, j) = 0;

		}
	}

	return dst;
}



Mat adaptiveBinarization() {

	char fname[MAX_PATH];

	while (openFileDlg(fname))
	{
		double t = (double)getTickCount();

		Mat src = imread(fname, IMREAD_GRAYSCALE);
		int height = src.rows;
		int width = src.cols;
		
		Mat bin = Mat::zeros(height, width, CV_8UC1);

		
		
		bin = myAdaptiveThreshold(src, 20, -2); 

		bin = applyDilation(bin);
		bin = applyDilation(bin);
		
		
		bin = applyErosion(bin);
		bin = applyErosion(bin);

		Mat unfiltered = bin.clone();

		filterSmallComponents(bin, 3000);
		
		

		t = ((double)getTickCount() - t) / getTickFrequency();

		printf("Time = %.3f [ms]\n", t * 1000);

		//imshow("input image", src);
		//imshow("median filter", median_filter);
		//imshow("unfiltered binary", unfiltered);
		imshow("binary", bin);
		waitKey();

		return bin;
		
	}

	return Mat();

}




void filterSmallComponents(Mat& img, int areaThreshold) {
	int height = img.rows;
	int width = img.cols;

	
	Mat labels = Mat::zeros(height, width, CV_32SC1);
	int label = 0;

	
	int di[8] = { -1, -1, -1,  0, 0,  1, 1, 1 };
	int dj[8] = { -1,  0,  1, -1, 1, -1, 0, 1 };

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {

			if (img.at<uchar>(i, j) == 255 && labels.at<int>(i, j) == 0) {
				label++;

				
				std::queue<std::pair<int, int>> Q;

				
				std::vector<std::pair<int, int>> componentPixels;

				
				labels.at<int>(i, j) = label;
				Q.push({ i, j });
				componentPixels.push_back({ i, j });

				
				while (!Q.empty()) {
					std::pair<int, int> q = Q.front();
					Q.pop();

					
					for (int k = 0; k < 8; k++) {
						int ni = q.first + di[k];
						int nj = q.second + dj[k];

						
						if (ni >= 0 && ni < height && nj >= 0 && nj < width) {
							
							if (img.at<uchar>(ni, nj) == 255 && labels.at<int>(ni, nj) == 0) {
								labels.at<int>(ni, nj) = label;
								Q.push({ ni, nj });
								componentPixels.push_back({ ni, nj });
							}
						}
					}
				}

				
				int area = componentPixels.size();

			
				if (area < areaThreshold) {
					
					for (size_t k = 0; k < componentPixels.size(); k++) {
						int r = componentPixels[k].first;
						int c = componentPixels[k].second;
						img.at<uchar>(r, c) = 0; 
					}
				}
			}
		}
	}
}








///DISTANCE TRANSFORM  

Mat distanceTransform(const Mat& src)
{
	Mat dist(src.size(), CV_32F);

	// init
	for (int i = 0; i < src.rows; i++)
		for (int j = 0; j < src.cols; j++)
			dist.at<float>(i, j) = (src.at<uchar>(i, j) == 0) ? 0 : 1e9;

	// forward
	for (int i = 0; i < src.rows; i++)
		for (int j = 0; j < src.cols; j++) {
			float d = dist.at<float>(i, j);

			if (i > 0)
				d = min(d, dist.at<float>(i - 1, j) + 1);
			if (j > 0)
				d = min(d, dist.at<float>(i, j - 1) + 1);
			if (i > 0 && j > 0)
				d = min(d, dist.at<float>(i - 1, j - 1) + sqrt(2));
			if (i > 0 && j < src.cols - 1)
				d = min(d, dist.at<float>(i - 1, j + 1) + sqrt(2));

			dist.at<float>(i, j) = d;
		}

	// backward
	for (int i = src.rows - 1; i >= 0; i--)
		for (int j = src.cols - 1; j >= 0; j--) {
			float d = dist.at<float>(i, j);

			if (i < src.rows - 1)
				d = min(d, dist.at<float>(i + 1, j) + 1);
			if (j < src.cols - 1)
				d = min(d, dist.at<float>(i, j + 1) + 1);
			if (i < src.rows - 1 && j < src.cols - 1)
				d = min(d, dist.at<float>(i + 1, j + 1) + sqrt(2));
			if (i < src.rows - 1 && j > 0)
				d = min(d, dist.at<float>(i + 1, j - 1) + sqrt(2));

			dist.at<float>(i, j) = d;
		}

	return dist;
}



//SKELETON LOCAL MAX VERIFY IF THE CURENT PIXEL IS LOCAL MAX. 
//IF IT SO, WE MARK IT AS SKELETON. 
//THE ALGORITHM DOESN'T KEEP CONECTIVITY. 

Mat skeleton_local_max() {
	Mat src = binarization();
	Mat dt = distanceTransform(src);

	int height = src.rows;
	int width = src.cols;

	Mat dst(height, width, CV_8UC3);

	int di[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };
	int dj[8] = { -1,  0,  1, -1, 1, -1, 0, 1 };

	for (int i = 1; i < height - 1; i++) {
		for (int j = 1; j < width - 1; j++) {

			uchar val_src = src.at<uchar>(i, j);
			float val = dt.at<float>(i, j);

			bool isMax = true;

			for (int k = 0; k < 8; k++) {
				int ni = i + di[k];
				int nj = j + dj[k];

				if (dt.at<float>(ni, nj) > val) {
					isMax = false;
					break;
				}
			}

			if (val_src != 0 && isMax)
				dst.at<Vec3b>(i, j) = Vec3b(255, 0, 255);
			else
				dst.at<Vec3b>(i, j) = Vec3b(val_src, val_src, val_src);
		}
	}

	

	imshow("skeleton", dst);
	waitKey();

	return dst;
}



///ZANG-SUEN THINNING 

void thinningIteration(Mat& img, int iter) {
	
	Mat marker = Mat::zeros(img.size(), CV_8UC1);

	for (int i = 1; i < img.rows - 1; i++) {
		for (int j = 1; j < img.cols - 1; j++) {

			uchar p1 = img.at<uchar>(i, j);
			if (p1 == 0) continue; 

			
			uchar p2 = img.at<uchar>(i - 1, j);
			uchar p3 = img.at<uchar>(i - 1, j + 1);
			uchar p4 = img.at<uchar>(i, j + 1);
			uchar p5 = img.at<uchar>(i + 1, j + 1);
			uchar p6 = img.at<uchar>(i + 1, j);
			uchar p7 = img.at<uchar>(i + 1, j - 1);
			uchar p8 = img.at<uchar>(i, j - 1);
			uchar p9 = img.at<uchar>(i - 1, j - 1);

			
			int B = p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9;

			
			int A = (p2 == 0 && p3 == 1) + (p3 == 0 && p4 == 1) +
				(p4 == 0 && p5 == 1) + (p5 == 0 && p6 == 1) +
				(p6 == 0 && p7 == 1) + (p7 == 0 && p8 == 1) +
				(p8 == 0 && p9 == 1) + (p9 == 0 && p2 == 1);

			
			int m1 = iter == 0 ? (p2 * p4 * p6) : (p2 * p4 * p8);
			int m2 = iter == 0 ? (p4 * p6 * p8) : (p2 * p6 * p8);

			
			if (A == 1 && (B >= 2 && B <= 6) && m1 == 0 && m2 == 0) {
				marker.at<uchar>(i, j) = 1;
			}
		}
	}


	img &= ~marker;
}


Mat zhangSuenThinning(Mat src) {
	Mat skel = src.clone();

	
	skel /= 255;

	Mat prev = Mat::zeros(skel.size(), CV_8UC1);
	Mat diff;

	do {
		
		thinningIteration(skel, 0);

		
		thinningIteration(skel, 1);

		
		absdiff(skel, prev, diff);
		skel.copyTo(prev);

	
	} while (countNonZero(diff) > 0);


	skel *= 255;

	return skel;
}

void zangSuen() {

	Mat bin = adaptiveBinarization();
	Mat zang = zhangSuenThinning(bin);

	int height = bin.rows;
	int width = bin.cols;

	Mat colorSkeleton = Mat::zeros(height, width, CV_8UC3);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (zang.at<uchar>(i, j) == 0) {
				colorSkeleton.at<Vec3b>(i, j) = Vec3b(bin.at<uchar>(i, j), bin.at<uchar>(i, j), bin.at<uchar>(i, j));
			}
			else {
				colorSkeleton.at<Vec3b>(i, j) = Vec3b(255, 0, 255);
			}
		}
	}

	imshow("zang-suen", zang);
	imshow("color-skeleton", colorSkeleton);

	waitKey();
	

}


///FINDING THE KEY POINTS IN THE SKELETON 

//*
// labelMat este matricea de etichete. 
// label = 1 inseamna ca punctul are doar un vecin, este end-point
// label = 2 simple-point
// label = 3 jonction-point
// 
// endPoints este un vector cu toate end-points ale scheletului. 
// Acesta structura ajuta la separarea trunchiului central de spini. 
// 
// */
struct SkeletonData {
	Mat labelMat;
	std::vector<cv::Point> endPoints; 
};

struct SkeletonData classifySkeletonPointsNeighbours(const Mat& ske) {

	int di[8] = {-1, -1, -1, 0, 0, 1,1,1};
	int dj[8] = {-1,0,1, -1, 1, -1,0, 1};

	int height = ske.rows;
	int width = ske.cols;


	struct SkeletonData result;

	result.labelMat = Mat::zeros(height, width, CV_8UC1);

	int neighbours = 0;

	for (int i = 1; i < height - 1; i++) {
		for(int j = 1; j < width - 1; j++){
			
			//only if the pixel is part of skeleton
			if (ske.at<uchar>(i, j) == 255) {
				neighbours = 0;
				for (int k = 0; k < 8; k++) {
					if (ske.at<uchar>(i + di[k], j + dj[k]) == 255) {
						neighbours++;
					}
				}
				if (neighbours == 1) {
					result.labelMat.at<uchar>(i, j) = 1; //end-point
					result.endPoints.push_back(cv::Point(j, i)); 
				}
				else if (neighbours == 2) {
					result.labelMat.at<uchar>(i, j) = 2; //simple point
				}
				else if (neighbours >= 3) {
					//jonction
					result.labelMat.at<uchar>(i, j) = 3;
				}
			}
				
		}
	}

	Mat colorTest = Mat::zeros(height, width, CV_8UC3); 
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {

			if(ske.at<uchar>(i, j) == 0) continue; 

			if (result.labelMat.at<uchar>(i, j) == 1) {
				//end point
				colorTest.at<Vec3b>(i, j) = Vec3b(0, 0, 255);
			}
			else if (result.labelMat.at<uchar>(i, j) == 2) {
				//simple point 
				colorTest.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
			}
			else if (result.labelMat.at<uchar>(i, j) == 3) {
				//jonction point
				colorTest.at<Vec3b>(i, j) = Vec3b(255, 0, 0);
			}
		}
	}
	return result;

}

///NUMARAM NUMARUL DE TRANZITII, NU NUMARUL DE VECINI DIN V8 !! 
///ALTFEL ALGORITMUL CLASIFICA PUNCTELE GRESIT SI VOM AVEA BLOCURI MARI DE JONCTIUNI. 

struct SkeletonData classifySkeletonPoints(const Mat& ske) {

	//coordonatele trebuie sa fie in ordinea acelor de ceasornic
	int cn_di[8] = { -1, -1,  0,  1,  1,  1,  0, -1 };
	int cn_dj[8] = { 0,  1,  1,  1,  0, -1, -1, -1 };

	int height = ske.rows;
	int width = ske.cols;

	struct SkeletonData result;
	result.labelMat = Mat::zeros(height, width, CV_8UC1);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {

			
			if (ske.at<uchar>(i, j) == 255) {

				int neighbors[8] = { 0 };
				int abs_neighbors = 0;

				
				for (int k = 0; k < 8; k++) {
					int ni = i + cn_di[k];
					int nj = j + cn_dj[k];

					// Daca nu iesim din imagine
					if (ni >= 0 && ni < height && nj >= 0 && nj < width) {
						if (ske.at<uchar>(ni, nj) == 255) {
							neighbors[k] = 1;
							abs_neighbors++;
						}
					}
				}

				//numaram tranzitiile de la 0-1
				int transitions = 0;
				for (int k = 0; k < 8; k++) {
					int current = neighbors[k];
					int next = neighbors[(k + 1) % 8];

					if (current == 0 && next == 1) {
						transitions++;
					}
				}

				
				if (abs_neighbors == 0) {
					//noise 
					result.labelMat.at<uchar>(i, j) = 0;
				}
				else if (transitions == 1) {
					//end-point
					result.labelMat.at<uchar>(i, j) = 1; 
					result.endPoints.push_back(cv::Point(j, i));
				}
				else if (transitions == 2) {
					//simple poine
					result.labelMat.at<uchar>(i, j) = 2; 
				}
				else if (transitions >= 3) {
					//jonction point 
					result.labelMat.at<uchar>(i, j) = 3; 
				}
			}
		}
	}

	return result;
}

Mat pruneSpines(Mat& skeleton, SkeletonData& skData, Mat& dt, float alpha) {
	int di[8] = { -1, -1, -1, 0, 0, 1,1,1 };
	int dj[8] = { -1,0,1, -1, 1, -1,0, 1 };

	Mat dendriteShaft = skeleton.clone();  
	std::vector<size_t> branchSize;

	//parcurgem toate end-points
	for (int i = 0; i < skData.endPoints.size(); i++) {
		cv::Point start = skData.endPoints[i];

		//daca punctul a fost sters mergem mai departe 
		if(skData.labelMat.at<uchar>(start.y, start.x) == 0) continue; 

		//avem nevoie de un vector pentru a pastra pixelii din ramura curenta 

		std::vector<cv::Point> branchPixels;
		branchPixels.push_back(start);

		 

		cv::Point curent= start;
		cv::Point prev = Point(-1,-1); 
		cv::Point jonctionPoint = Point(-1,-1); 

		bool reacheadJonction = false; 
		while (!reacheadJonction) {
			bool moved = false; //flag pentru a evita bucle infinite

			//parcurgem toti vecinii pixelului curent 
			for (int k = 0; k < 8; k++) {
				cv::Point neighbour(curent.x + dj[k], curent.y + di[k]); 

				if (neighbour.x < 0 || neighbour.x >= skeleton.cols || neighbour.y < 0 || neighbour.y >= skeleton.rows) {
					continue;
				}

				//nu e suficent sa verificam neighbour == prev, programul poate 
				//intra in bucla infinita !!! 
				//trebuie sa verificam in toata lista de elemente, nu e suficent doar prev 


				bool alreadyVisited = false; 
				for (size_t v = 0; v < branchPixels.size(); v++) {
					if (branchPixels[v] == neighbour) {
						alreadyVisited = true; 
						break; 
					}
				}
				 
				if(alreadyVisited) continue; 

				uchar label = skData.labelMat.at<uchar>(neighbour.y, neighbour.x);

				if (label == 2) {
					//simple point
					//facem un pas inainte 
					branchPixels.push_back(neighbour);
					prev = curent; 
					curent = neighbour; 
					moved = true; 
					break;
				}
				else if (label == 3) {
					//jonction
					jonctionPoint = neighbour; 
					reacheadJonction = true; 
					moved = true;
					break; //oprim cand am intalnit o jonctiune 
				}
			}
			if(!moved) break; 
		}

		//dupa acest while avem toata lista cu pixeli si putem sa ii stergem si sa facem taierea 
		if (reacheadJonction) {

			//valoarea din distance transform 
			float dt_val = dt.at<float>(jonctionPoint.y, jonctionPoint.x); 

			float limit = alpha;

			if (limit < 1.0f) {
				limit = 1.0f; 
			}

			if (branchPixels.size() < limit) {
				//daca lungimea pixelilor este mai mica decat limita, ii stergem. 
				for (int k = 0; k < branchPixels.size(); k++) {
					cv::Point p = branchPixels[k]; 
					dendriteShaft.at<uchar>(p.y, p.x) = 0; 
				}

			}
		}

		branchSize.push_back(branchPixels.size()); 
	}

	int sum = 0; 
	for (size_t s = 0; s < branchSize.size(); s++) {
		sum += branchSize[s];
	}
	printf("lungimea medie a spinilor este: %f\n", (float)sum/(float)branchSize.size());
	return dendriteShaft; 
}


Mat getDendriteShaft(const Mat &bin) {
	if (bin.empty()) return Mat::zeros(bin.rows, bin.cols, CV_8UC1);

	Mat skel = zhangSuenThinning(bin);
	Mat dst = distanceTransform(bin);

	Mat dendriteShaft = skel.clone();
	int pixelsBefore = countNonZero(dendriteShaft);
	struct SkeletonData skData; 

	

	
	for (int i = 0; i < 50; i++) {

		
		skData = classifySkeletonPoints(dendriteShaft);
		dendriteShaft = pruneSpines(dendriteShaft, skData, dst, 40.0f);

		int pixelsAfter = countNonZero(dendriteShaft);

		
		if (pixelsBefore == pixelsAfter) {
			printf("Scheletul a fost curatat complet in %d iteratii!\n", i + 1);
			break;
		}
		pixelsBefore = pixelsAfter;
	}

	filterSmallComponents(dendriteShaft, 100); 

	return dendriteShaft; 
}














//DUPA CE AM OBTINUT IMAGINEA CU SPINI TREBUIE DOAR SA MAI CALCULAM DIFERITE STATISTICI 



struct Metrics {
	int nrSpines;
	float meanHeadWidth;
	float stdDevHeadWidth;
	float meanLen;
	float stdDevLen;
	std::vector<int> lenHist;
};

Metrics getMetrics(Mat spines, Mat dendriteShaft, SkeletonData skData, Mat dst) {

	int height = spines.rows;
	int width = spines.cols;

	Metrics metrics = { 0, 0.0f, 0.0f, 0.0f, 0.0f, std::vector<int>() };

	Mat labels = Mat::zeros(height, width, CV_32SC1);
	int label = 0;

	int di[8] = { -1, -1, -1,  0, 0,  1, 1, 1 };
	int dj[8] = { -1,  0,  1, -1, 1, -1, 0, 1 };

	// vectori pentru a pastra temporar datele fiecarui spin
	std::vector<int> lengths;
	std::vector<float> headWidths;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {

			if (spines.at<uchar>(i, j) == 255 && labels.at<int>(i, j) == 0) {
				label++;

				std::queue<std::pair<int, int>> Q;
				std::vector<std::pair<int, int>> componentPixels;

				labels.at<int>(i, j) = label;
				Q.push({ i, j });
				componentPixels.push_back({ i, j });

				while (!Q.empty()) {
					std::pair<int, int> q = Q.front();
					Q.pop();

					for (int k = 0; k < 8; k++) {
						int ni = q.first + di[k];
						int nj = q.second + dj[k];

						if (ni >= 0 && ni < height && nj >= 0 && nj < width) {
							if (spines.at<uchar>(ni, nj) == 255 && labels.at<int>(ni, nj) == 0) {
								labels.at<int>(ni, nj) = label;
								Q.push({ ni, nj });
								componentPixels.push_back({ ni, nj });
							}
						}
					}
				}

				
				int spineLength = componentPixels.size();
				lengths.push_back(spineLength);

				
				float spineWidthSum = 0.0f;
				int endpointsFound = 0;

				for (size_t p = 0; p < componentPixels.size(); p++) {
					int r = componentPixels[p].first;
					int c = componentPixels[p].second;

					
					if (skData.labelMat.at<uchar>(r, c) == 1) {
						float dt_val = dst.at<float>(r, c);
						spineWidthSum += (dt_val * 2.0f);  
						endpointsFound++;
					}
				}

				
				if (endpointsFound > 0) {
					headWidths.push_back(spineWidthSum / endpointsFound);
				}
				else {
					
					headWidths.push_back(2.0f);
				}
			}
		}
	}

	
	metrics.nrSpines = label;

	if (metrics.nrSpines == 0) {
		return metrics; 
	}

	
	float sumLen = 0.0f;
	float sumWidth = 0.0f;
	int maxLen = 0;

	for (size_t i = 0; i < lengths.size(); i++) {
		sumLen += lengths[i];
		sumWidth += headWidths[i];
		if (lengths[i] > maxLen) maxLen = lengths[i];
	}

	
	metrics.meanLen = sumLen / metrics.nrSpines;
	metrics.meanHeadWidth = sumWidth / metrics.nrSpines;

	
	float varLen = 0.0f;
	float varWidth = 0.0f;

	for (size_t i = 0; i < lengths.size(); i++) {
		varLen += (lengths[i] - metrics.meanLen) * (lengths[i] - metrics.meanLen);
		varWidth += (headWidths[i] - metrics.meanHeadWidth) * (headWidths[i] - metrics.meanHeadWidth);
	}

	metrics.stdDevLen = std::sqrt(varLen / metrics.nrSpines);
	metrics.stdDevHeadWidth = std::sqrt(varWidth / metrics.nrSpines);

	
	metrics.lenHist.assign(maxLen + 1, 0);
	for (size_t i = 0; i < lengths.size(); i++) {
		metrics.lenHist[lengths[i]]++;
	}

	
	printf("\n--- METRICI FINALE ---\n");
	printf("Numar spini: %d\n", metrics.nrSpines);
	printf("Lungime Medie: %.2f (StdDev: %.2f)\n", metrics.meanLen, metrics.stdDevLen);
	printf("Latime Medie Cap: %.2f (StdDev: %.2f)\n", metrics.meanHeadWidth, metrics.stdDevHeadWidth);

	printf("\n--- DISTRIBUTIA LUNGIMILOR (HISTOGRAMA) ---\n");
	for (size_t i = 0; i < metrics.lenHist.size(); i++) {
		if (metrics.lenHist[i] > 0) {
			printf(" - Lungimea %d pixeli: gasita la %d spini\n", i, metrics.lenHist[i]);
		}
	}
	

	return metrics;
}

void computeSpineMetrics() {

	Mat bin = adaptiveBinarization();
	if (bin.empty()) return ;

	int height = bin.rows; 
	int width = bin.cols; 

	Mat skel = zhangSuenThinning(bin);
	SkeletonData skData = classifySkeletonPoints(skel);
	Mat dst = distanceTransform(bin);


	Mat dendriteShaft = getDendriteShaft(bin); 

	Mat spines = skel.clone(); 

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if(spines.at<uchar>(i, j) == 0) continue; 

			if (dendriteShaft.at<uchar>(i, j) == 255) {
				spines.at<uchar>(i,j) = 0; 
			}
		}
	}

	Metrics metrics = getMetrics(spines, dendriteShaft, skData, dst);

	if (!metrics.lenHist.empty()) {
		showHistogram("Histograma Lungimi Spini", metrics.lenHist.data(), metrics.lenHist.size(), 200);
	}
	



	imshow("initial-skeleton", skel); 
	imshow("dendrite-shaft", dendriteShaft); 
	imshow("spines", spines);
	waitKey(); 

	
}






int main() 
{
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_FATAL);
    projectPath = _wgetcwd(0, 0);

	int op;
	do
	{
		system("cls");
		destroyAllWindows();
		printf("Menu:\n");
		printf(" 1 - Open image\n");
		printf("2 - GLOBAL BINARIZATION + FILTER SMALL COMPONENTS\n");
		printf("3 - ADAPTIVE BINARIZATION + FILTER SMALL COMPONENTS\n");
		printf("4 - Skeleton LOCAL MAX\n");
		printf("5 - Zang-Suen Thinning For Skeleton\n");
		printf("6 - Display Dendrite Shaft, Spines and Metrics\n");
		
	
		
	

	




		printf("Option: ");
		scanf("%d",&op);



		switch (op)
		{
			case 1:
				testOpenImage();
				break;
			case 2: 
				binarization();
				break;
			case 3:
				adaptiveBinarization();
				break;
			case 4: 
				skeleton_local_max();
				break;
			case 5: 
				zangSuen();
				break;
			case 6: 
				computeSpineMetrics(); 
				break;
			case 7:		
				computeSpineMetrics();
				break; 
		
			
		
		}
	}
	while (op!=0);
	return 0;
}