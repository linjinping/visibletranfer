#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <math.h>

using namespace cv;
using namespace std;
//����͸�ӱ任,ÿ���ά���size�����Լ�����
int point_size = 4;
//�ҵ�����ȡ���������ĵ�


/*
�����Ϸ�Ϊ����
1.Ѱ�ҳ��ĸ�����
2.ȷ��ÿ�������λ��
3.�����ҷ�ζ��ʶ��ÿ����
*/
int main(int argc, char** argv[])
{
	Mat src = imread("C:\\Users\\14769\\Desktop\\qrcodew.jpg");
	Mat src_gray;
	Mat src_new;
	Mat src_all = src.clone();
	Mat threshold_output;
	vector<vector<Point> > contours, contours2;
	vector<Vec4i> hierarchy;
	
	//Ԥ����
	cvtColor(src, src_gray, CV_BGR2GRAY);
	blur(src_gray, src_gray, Size(3, 3)); //ģ����ȥ��ë��
	threshold(src_gray, threshold_output, 100, 255, THRESH_OTSU);
	//Ѱ������ 
	//��һ������������ͼ�� 2ֵ����
	//�ڶ����������ڴ�洢����FindContours�ҵ��������ŵ��ڴ����档
	//�����������ǲ㼶��**[Next, Previous, First_Child, Parent]** ��vector
	//���ĸ����������ͣ��������ṹ
	//����������ǽڵ����ģʽ��������ȫ��Ѱ��
	findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CHAIN_APPROX_NONE, Point(0, 0));
	//����ɸѡ
	int c = 0, ic = 0, area = 0;
	int parentIdx = -1;
	for (int i = 0; i < contours.size(); i++)
	{
		//hierarchy[i][2] != -1 ��ʾ���������������
		if (hierarchy[i][2] != -1 && ic == 0)
		{
			parentIdx = i;
			ic++;
		}
		else if (hierarchy[i][2] != -1)
		{
			ic++;
		}
		//���������0
		else if (hierarchy[i][2] == -1)
		{
			ic = 0;
			parentIdx = -1;
		}
		//�ҵ���λ����Ϣ
		if (ic >= 2)
		{
			contours2.push_back(contours[parentIdx]);
			ic = 0;
			parentIdx = -1;
		}
	}
	//��䶨λ��
	for (int i = 0; i < contours2.size(); i++)drawContours(src_all, contours2, i, CV_RGB(0, 255, 0), -1);
	//���Ӷ�λ��
	int maxx = 1E9 * -1, minx = 1E9;
	int maxsub = 1E9 * -1, minsub = 1E9;
	int lu[2] = { 0 }, ru[2] = { 0 }, ld[2] = { 0 }, rd[2] = { 0 };//left right up down
	Point point[5];
	int size_n;
	//��Ϊ�Ĳ��ж��ĸ��� ���Ͻ�Ϊ���е����С ####���Ͻ� ���½�Ϊy-x(x-y?)��������С#####  ���½�Ϊ�����
	//��һ���ܳ�����,�����ú���,����ֱ������,������Ǹ����ص�ֿ鲢�Ҵ���������͸�ӱ任
	for (int i = 0; i < contours2.size(); i++)
	{

		for (int j = 0;j < contours2[i].size();j++)
		{
			if (contours2[i][j].x + contours2[i][j].y < minx)
			{
				minx = contours2[i][j].x + contours2[i][j].y;
				lu[0] = i;
				lu[1] = j;
			}
			if (contours2[i][j].x + contours2[i][j].y > maxx)
			{
				maxx = contours2[i][j].x + contours2[i][j].y;
				rd[0] = i;
				rd[1] = j;
			}
			if (contours2[i][j].x - contours2[i][j].y > maxsub)
			{
				maxsub = contours2[i][j].x - contours2[i][j].y;
				ru[0] = i;
				ru[1] = j;
			}
			if (contours2[i][j].x - contours2[i][j].y < minsub)
			{
				minsub = contours2[i][j].x - contours2[i][j].y;
				ld[0] = i;
				ld[1] = j;
			}
		}
		//point[i] = Center_cal(contours2, i);
	}
	vector<Point2f> srcTri(4);
	vector<Point2f> dstTri(4);
	srcTri[0] = Point(contours2[lu[0]][lu[1]].x, contours2[lu[0]][lu[1]].y);
	srcTri[2] = Point(contours2[ru[0]][ru[1]].x, contours2[ru[0]][ru[1]].y);
	srcTri[1] = Point(contours2[ld[0]][ld[1]].x, contours2[ld[0]][ld[1]].y);
	srcTri[3] = Point(contours2[rd[0]][rd[1]].x, contours2[rd[0]][rd[1]].y);
	vector<Point2f>src_coners(4);
	vector<Point2f>dst_coners(4);
	dst_coners[0] = Point2f(0, 0);
	dst_coners[1] = Point2f(0, 48 * 8 * point_size);
	dst_coners[2] = Point2f(48 * 8 * point_size, 0);
	dst_coners[3] = Point2f(48 * 8 * point_size, 48 * 8 * point_size);
	Mat warpMatrix = getPerspectiveTransform(srcTri, dst_coners);
	warpPerspective(src, src_new, warpMatrix, src_new.size(), INTER_LINEAR, BORDER_CONSTANT);
	//����͸�ӱ任���,��ʱ��(0,0)�Ƕ�ά�����Ͻ�,48*8*point_size �Ƕ�ά����ܿ��,point_size��ÿ����Ŀ��
	//line(src, Point(contours2[lu[0]][lu[1]].x, contours2[lu[0]][lu[1]].y), Point(contours2[ru[0]][ru[1]].x, contours2[ru[0]][ru[1]].y), Scalar(0, 0, 255), 2);
	//line(src, Point(contours2[lu[0]][lu[1]].x, contours2[lu[0]][lu[1]].y), Point(contours2[rd[0]][rd[1]].x, contours2[rd[0]][rd[1]].y), Scalar(0, 0, 255), 2);
	// ����ȥ�ǿ���ĶԵ����ʶ��,���������õĵ�һһ����,Ϊ�˷���,Ӧ������һ����Χȡƽ��ֵ,ͬʱ�Ҹ�ÿ���㶼��������ɫ,�������ж�,���ڿ���ȥ��
	int Data;
	int Base;
	for (int i = 0;i <= 7;i++)
	{
		for (int k = 1;k <= 46;k++)
		{
			Data = 0;
			Base = 1;
			for (int j = 0 + k * 8;j <= 7 + k * 8;j++)
			{
				if (src_new.at<Vec3b>((i + 0.5) * point_size, (j + 0.5) * point_size)[0] > 124)
					;else Data += Base;
				Base *= 2;
				//������������������ɫ
				src_new.at<Vec3b>((i + 0.5) * point_size, (j + 0.5) * point_size)[0] = 120;
				src_new.at<Vec3b>((i + 0.5) * point_size, (j + 0.5) * point_size)[1] = 120;
				src_new.at<Vec3b>((i + 0.5) * point_size, (j + 0.5) * point_size)[2] = 120;
			}
			cout << char(Data);
		}
	}
	//����Ķ��м䲿��ʶ��
	for (int i = 8;i <= 55 + 5 * 64;i++)
	{

		for (int k = 0;k <= 47;k++)
		{
			Data = 0;
			Base = 1;
			for (int j = 0 + k * 8;j <= 7 + k * 8;j++)
			{
				if (src_new.at<Vec3b>((i + 0.5) * point_size, (j + 0.5) * point_size)[0] > 124)
					;else Data += Base;
				Base *= 2;
				src_new.at<Vec3b>((i + 0.5) * point_size, (j + 0.5) * point_size)[0] = 120;
				src_new.at<Vec3b>((i + 0.5) * point_size, (j + 0.5) * point_size)[1] = 120;
				src_new.at<Vec3b>((i + 0.5) * point_size, (j + 0.5) * point_size)[2] = 120;
			}
			cout << char(Data);
		}

	}
	//��ζ�Ķ��¶�ʶ��
	for (int i = 56 + 5 * 64;i <= 63 + 5 * 64;i++)
	{

		for (int k = 1;k < 47;k++)
		{
			Data = 0;
			Base = 1;
			for (int j = 0 + k * 8;j <= 7 + k * 8;j++)
			{
				if (src_new.at<Vec3b>((i + 0.5) * point_size, (j + 0.5) * point_size)[0] > 124)
					;else Data += Base;
				Base *= 2;
				src_new.at<Vec3b>((i + 0.5) * point_size, (j + 0.5) * point_size)[0] = 120;
				src_new.at<Vec3b>((i + 0.5) * point_size, (j + 0.5) * point_size)[1] = 120;
				src_new.at<Vec3b>((i + 0.5) * point_size, (j + 0.5) * point_size)[2] = 120;
			}
			cout << char(Data);
		}
	}
	cout << endl;
	//�ҵĴ������*/
	imshow("���", src_new);
	imwrite("C:\\Users\\14769\\Desktop\\qrcoder.jpg", src_new);
	waitKey(0);
	return(0);
}