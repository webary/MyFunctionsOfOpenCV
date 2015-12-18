/**
	该头文件使用OpenCV库实现读取图片并保存像素信息、直接输出一幅图片、
	对图像进行一些常见处理操作。
	@author : webary
	@time (last update time): 2014.9.12  19:00
**/
#pragma once
#ifndef _IMGPRO_H_
#define _IMGPRO_H_

#include<windows.h>
#include<iostream>
#include<iomanip>
#include<cstdlib>
#include<cstdio>
#include<string>
#include<cv.h>
#include<highgui.h>
#include"opencv2/legacy/legacy.hpp"	//openCV 2.3上的目录,支持函数cvCalcPGH
using namespace std;
typedef const char cchar;

#ifndef _IMGPRO_H_VAR_
#define _IMGPRO_H_VAR_

extern IplImage *pGray;
extern char info[10000];
extern IplImage* find_obj_match;
#endif

const int MAXSIZE = 500;
//Image class
class Image {
public:
    int width, height, depth;
    IplImage* pimg;
    uchar R[MAXSIZE][MAXSIZE], G[MAXSIZE][MAXSIZE], B[MAXSIZE][MAXSIZE];

    Image() {
        pimg=0;
    }
    Image(const Image& img);
    ~Image() {
        releaseImg();
    }
    Image(IplImage* p);
    Image(cchar* tmpimgName,int ask=1,int cls=1,int color=1,int show=1);
    IplImage *loadImg(cchar* tmpimgName,int ask=1,int cls=1,int color=1,int show=1);
#ifdef __AFXWIN_H__
    Image(CString& tmpimgstring,int ask=1,int cls=1,int color=1,int show=1);
    IplImage *loadImg(CString& tmpimgstring,int ask=1,int cls=1,int color=1,int show=1);
#endif
    void saveRGB(cchar *fileOutName = 0) ;
    void outputImg(cchar* imgOutName,int readAgain = 1) ;
    void saveIplImage();
    void saveImg(cchar* imgSaveName);
    int  showImg(cchar* str = "图片显示",cchar* ss="显示图像出错",int pause=1) ;
    void releaseImg() ;
    inline uchar getGray(unsigned i,unsigned j)const;
    void reverseRGB(int th=255);
    void toGray();
    void controlWhtieNoise(int th);
    int  Otsu()const;
    IplImage* equalizeHist(int saveFile=0)const;
    void Histeq_self(bool save=0);
};

#ifdef __AFXWIN_H__
#pragma region Image Class
#endif
Image::Image(IplImage* p) {
    if(p) {
        memcpy(pimg,p,sizeof(IplImage));
        width = p->width;
        height = p->height;
        depth = p->depth;
    } else
        pimg = 0;
}

Image::Image(cchar* tmpimgstring,int ask,int cls,int color,int show) {
    pimg = 0;
    loadImg(tmpimgstring, ask, cls, color, show);
}
#ifdef __AFXWIN_H__
Image::Image(CString& tmpimgstring,int ask,int cls,int color,int show) {
    pimg = 0;
    loadImg((LPSTR)(LPCTSTR)tmpimgstring, ask, cls, color, show);
}
#endif
//载入图片
#ifndef __AFXWIN_H__
#include<io.h>			//access()
IplImage *Image::loadImg(cchar* tmpimgName,int ask,int cls,int color,int show) {
    releaseImg();
    if(tmpimgName==0 || tmpimgName[0]==0)
        return 0;
    char imgName[200], tempName[200];
    strcpy(imgName,tmpimgName);
    int first = 1,flag;
    do {
        flag = 0;
        if(first==0) {
            cout<<"请输入需要载入的图片: ";
            gets(imgName);
        }
        if(access(imgName,4)) {							//打开失败,没有读取权限
            strcpy(tempName,imgName);
            if(strstr(tempName,".jpg")==0) {			//文件名没有以.jpg结尾
                strcat(tempName,".jpg");
                if(access(tempName,1)) {				//打开失败,没有读取权限
                    strcpy(tempName,imgName);
                    if(strstr(tempName,".png")==0) {	//文件名没有以.png结尾
                        strcat(tempName,".png");
                        if(access(tempName,1)) {		//打开失败,没有读取权限
                            strcpy(tempName,imgName);
                            if(strstr(tempName,".bmp")==0) { //文件名没有以.bmp结尾
                                strcat(tempName,".bmp");
                                if(access(tempName,1)==0)
                                    strcpy(imgName,tempName);
                            }
                        } else
                            strcpy(imgName,tempName);
                    }
                } else
                    strcpy(imgName,tempName);
            }
            if(access(imgName,1)) {						//打开失败,没有读取权限
                if(first) {
                    first = 0;
                    flag = 1;
                    continue;
                }
                cout<<"打开图片 \""<<imgName<<"\" 失败 ！"<<endl;
                flag = 1;
            }
        }
        if(access(imgName,1)==0 && (strstr(imgName,".jpg") || strstr(imgName,".bmp") || strstr(imgName,".png"))) {						//图片可以读取
            pimg = cvLoadImage(imgName,color);
            if(cls) system("cls");
            char tips[200] = "已成功载入图片 ";
            strcat(tips,imgName);
            if(show) cout<<tips<<endl;
            if(first && ask && MessageBox(NULL,"是否重新载入其他图片？",tips,MB_YESNO)==IDYES) {
                if(cls) system("cls");
                flag = 1;
            }
        }
        first = 0;
    } while(flag);
    if(show)
        cout<<"width(宽) : "<<pimg->width<<"    height(高) : "<<pimg->height<<endl;
    //    <<"    depth(位深度) : "<<img->depth
    width  = pimg->width;
    height = pimg->height;
    depth  = pimg->depth;
	if(width>MAXSIZE || height>MAXSIZE){
		cout<<"图像过大，暂时无法处理"<<endl;
		releaseImg();
	}
    return pimg;
}
#else
#include"Shlwapi.h"		//PathFileExists()
IplImage *Image::loadImg(cchar* tmpimgName,int ask/*=1*/,int cls/*=1*/,int color/*=1*/,int show/*=1*/) {
    if(tmpimgName==0 || tmpimgName[0]==0)
        return 0;
    char imgName[200], tempName[200];
    strcpy(imgName,tmpimgName);
    int first = 1,flag;
    do {
        flag = 0;
        if(first==0) {
            CString warn;
            warn.Format(_T("无法载入\"%s\",请先指定正确的图像路径，然后载入图像"),tmpimgName);
            MessageBox(NULL,warn,"载入图像失败",MB_ICONERROR);
            return 0;
        }
        if(!PathFileExists(imgName)) {					//打开失败,没有读取权限
            strcpy(tempName,imgName);
            if(strstr(tempName,".jpg")==0) {			//文件名没有以.jpg结尾
                strcat(tempName,".jpg");
                if(!PathFileExists(tempName)) {			//打开失败,没有读取权限
                    strcpy(tempName,imgName);
                    if(strstr(tempName,".png")==0) {	//文件名没有以.png结尾
                        strcat(tempName,".png");
                        if(!PathFileExists(tempName)) {		//打开失败,没有读取权限
                            strcpy(tempName,imgName);
                            if(strstr(tempName,".bmp")==0) { //文件名没有以.bmp结尾
                                strcat(tempName,".bmp");
                                if(PathFileExists(tempName))//文件可访问
                                    strcpy(imgName,tempName);
                            }
                        } else
                            strcpy(imgName,tempName);
                    }
                } else
                    strcpy(imgName,tempName);
            }
            if(!PathFileExists(imgName)) {						//打开失败,没有读取权限
                flag = 1;
                if(first) {
                    first = 0;
                    continue;
                }
                sprintf(info,"%s打开图片 \"%s\" 失败 ！\r\n",info,imgName);
                MessageBox(NULL,"打开图像失败","出错提示",0);
            }
        }
        if(PathFileExists(imgName) && (strstr(imgName,".jpg") || strstr(imgName,".bmp") || strstr(imgName,".png"))) {	//图片可以读取
            pimg = cvLoadImage(imgName,color);
            if(show) {
                char tips[200] = "已成功载入图像文件 ";
                strcat(tips,imgName);
                sprintf(info,"%s%s\r\n",info,tips);
            }
        } else
            flag = 1;
        first = 0;
    } while(flag);
    if(show)
        sprintf(info,"%swidth(宽) : %d    height(高) : %d\r\n",info,pimg->width,pimg->height);
    width  = pimg->width;
    height = pimg->height;
    depth  = pimg->depth;
    return pimg;
}

IplImage *Image::loadImg(CString& tmpimgstring,int ask/*=1*/,int cls/*=1*/,int color/*=1*/,int show/*=1*/) {
    return loadImg((LPSTR)(LPCTSTR)tmpimgstring,ask,cls,color,show);
}
#endif
//保存图片像素信息到文件或数组
void Image::saveRGB(cchar *fileOutName/* = 0*/) {
    if(pimg==0)
        return;
    uchar* data = (uchar*)pimg->imageData;
    if(fileOutName && fileOutName[0]) {
        FILE *fp1;
        if(fp1 = fopen(fileOutName,"w")) {
            if(pimg->nChannels==3) {
                for( int y = 0; y < pimg->height; y++ )
                    for( int x = 0,yy = y * pimg->widthStep; x < pimg->width; x++ )
                        fprintf(fp1,"%d\t%d\t%d\n",data[yy+3*x+2],data[yy+3*x+1],data[yy+3*x]);
            } else if(pimg->nChannels==1) {
                for( int y = 0; y < pimg->height; y++ )
                    for( int x = 0,yy = y * pimg->widthStep; x < pimg->width; x++ )
                        fprintf(fp1,"%d\n",data[yy+x]);
            }
            fclose(fp1);
        }
    } else {
        memset(R,0,sizeof(R));
        memset(G,0,sizeof(G));
        memset(B,0,sizeof(B));
        if(pimg->nChannels==3) {
            for( int y = 0; y < pimg->height; y++ )
                for( int x = 0,yy = y * pimg->widthStep; x < pimg->width; x++ ) {
                    R[y][x] = data[yy+3*x+2];
                    G[y][x] = data[yy+3*x+1];
                    B[y][x] = data[yy+3*x];
                }
        } else if(pimg->nChannels==1) {
            for( int y = 0; y < pimg->height; y++ )
                for( int x = 0,yy = y * pimg->widthStep; x < pimg->width; x++ )
                    R[y][x] = data[yy+x];
        }
    }
}

//保存当前的RGB数组到IplImage指针中
void Image::saveIplImage() {
    uchar* data = (uchar*)pimg->imageData;
    if(pimg->nChannels==3) {
        for( int y = 0; y < pimg->height; y++ )
            for( int x = 0,yy = y * pimg->widthStep; x < pimg->width; x++ ) {
                data[yy+3*x+2] = R[y][x];
                data[yy+3*x+1] = G[y][x];
                data[yy+3*x]   = B[y][x];
            }
    } else if(pimg->nChannels==1) {
        for( int y = 0; y < pimg->height; y++ )
            for( int x = 0,yy = y * pimg->widthStep; x < pimg->width; x++ )
                data[yy+x] = R[y][x];
    }
}

//输出图片到本地文件，第二个参数是选择是否再次读取RGB值
void Image::outputImg(cchar* imgOutName,int readAgain/* = 1*/) {
    if(imgOutName[0]==0 || pimg==0)
        return;
    if(readAgain)
        saveIplImage();
    if(strstr(imgOutName,".jpg")==0 && strstr(imgOutName,".bmp")==0) {
        char name[200];
        strcpy(name,imgOutName);
        strcat(name,".jpg");
        cvSaveImage(name,pimg);
    } else
        cvSaveImage(imgOutName,pimg);
}

//保存图片到文件
void Image::saveImg(cchar* imgSaveName) {
    outputImg(imgSaveName,0);
}

//显示图片
int Image::showImg(cchar* str/* = "图片显示"*/,cchar* ss/*"显示图像出错"*/,int pause/*=1*/) {
    if(pimg==0) {
#ifdef __AFXWIN_H__
        MessageBox(NULL,"图像指针无效，请先载入有效图像后再使用该功能",ss,MB_ICONSTOP);
#endif
        return 0;
    }
    cvNamedWindow(str);
    cvShowImage(str,pimg);
    if(pause==1) {
        cvWaitKey(0);
        cvDestroyWindow(str);//销毁窗口
    }
    return 1;
}

//释放图片内存指针
void Image::releaseImg() {
    if(pimg) {
        cvReleaseImage(&pimg);
        pimg = 0;
    }
}

inline uchar Image::getGray(unsigned i,unsigned j)const {
    //return (.3*R[i][j]+.6*G[i][j]+.1*B[i][j]+1)/3;
    return i<MAXSIZE && j<MAXSIZE ? (R[i][j]+G[i][j]+B[i][j]+1)/3 : 0;
}

//像素取反
void Image::reverseRGB(int th) {
    for(int i=0; i<height; i++)
        for(int j=0; j<width; j++) {
            R[i][j] = R[i][j]>th?0 : 255 - R[i][j];
            G[i][j] = G[i][j]>th?0 : 255 - G[i][j];
            B[i][j] = B[i][j]>th?0 : 255 - B[i][j];
        }
}

//灰度化图像
void Image::toGray() {
    for(int i=0; i<height; i++)
        for(int j=0; j<width; j++)
            R[i][j]=G[i][j]=B[i][j] = getGray(i,j);
}

//控制并滤除偏白（亮度较高）的噪声点
void Image::controlWhtieNoise(int th) {
    for(int i=0; i<height; i++)
        for(int j=0; j<width; j++)
            //if(R[i][j]>th && G[i][j]>th && B[i][j]>th)
            if(R[i][j]+G[i][j]+B[i][j]>3*th)
                R[i][j] = G[i][j] = B[i][j] = 255;
            else
                R[i][j] = G[i][j] = B[i][j] = (R[i][j]+G[i][j]+B[i][j]+1)/3;
}

Image::Image(const Image& img) {
    memcpy(pimg,img.pimg,sizeof(*pimg));
    width  = pimg->width;
    height = pimg->height;
    depth  = pimg->depth;
}

//最大类间方差算法 将灰度图像灰度级分割(注意:传入的需要是灰度图像数据)
int Image::Otsu()const {
    int height=pimg->height, width=pimg->width, threshold, i, j;
    double histogram[256] = {0};
    for(i=0; i < height; i++) {
        uchar* p=(uchar*)pimg->imageData + pimg->widthStep * i;
        for(j = 0; j < width; j++)
            histogram[*p++]++;
    }
    int size = height * width;
    for(i = 0; i < 256; i++)
        histogram[i] /= size;
    double avgValue=0, maxVariance=0, w = 0, u = 0;
    for(i=0; i < 256; i++)
        avgValue += i * histogram[i];  //整幅图像的平均灰度
    for(i = 0; i < 256; i++) {
        w += histogram[i];
        u += i * histogram[i];
        double t = avgValue * w - u;
        double variance = t * t / (w * (1 - w) );
        if(variance > maxVariance) {
            maxVariance = variance;
            threshold = i;
        }
    }
    return threshold;
}

//将图片直方图均衡化处理--利用cv库函数--需要释放该指针
IplImage* Image::equalizeHist(int saveFile)const {
    if(pimg==0)
        return 0;
    IplImage* pGray = cvCreateImage(cvGetSize(pimg),pimg->depth,1);
    if(pimg->nChannels==3)
        cvCvtColor(pimg,pGray,CV_BGR2GRAY);
    else if(pimg->nChannels==1)
        cvCopy(pimg,pGray);
    else {
        //pGray.releaseImg();
        return 0;
    }
    cvEqualizeHist(pGray,pGray);
    if(saveFile) {
        Image img1(pGray);
        img1.outputImg("均衡化.jpg",0);
    }
    return pGray;
}

//将图片直方图均衡化处理--利用自编函数--需要释放该指针
IplImage* Histeq(const Image& _img,int saveFile=0) {
    IplImage* img = _img.pimg;
    if(img==0)
        return 0;
    IplImage* pGray = cvCreateImage(cvGetSize(img),img->depth,1);
    if(img->nChannels==3)
        cvCvtColor(img,pGray,CV_BGR2GRAY);
    else if(img->nChannels==1)
        cvCopy(img,pGray);
    else {
        //releaseImg(&pGray);
        return 0;
    }
    double histogram[256] = {0},dTemp;
    int i,j,k;
    uchar* p;
    for(i=0; i < pGray->height; i++) {
        p=(uchar*)pGray->imageData + pGray->widthStep * i;
        for(j = 0; j < pGray->width; j++)
            histogram[*p++]++;
    }
    int size = pGray->height * pGray->width;
    for(i = 0; i < 256; i++)
        histogram[i] /= size;
    for(i=0; i < pGray->height; i++) {
        p=(uchar*)pGray->imageData + pGray->widthStep * i;
        for(j = 0; j < pGray->width; j++) {
            dTemp = 0;
            int gray = _img.getGray(i,j);
            for(k=0; k<gray; ++k)
                dTemp += histogram[k];
            int tar = int(255 * dTemp+.5);
            if(tar<0) tar = 0;
            else if(tar>255) tar = 255;
            *p++ = tar;
        }
    }
    if(saveFile) {
        Image img1(pGray);
        img1.outputImg("均衡化.jpg",0);
    }
    return pGray;
}

//将图片直方图均衡化处理--结果保存在自身指针中
void Image::Histeq_self(bool save) {
    saveRGB();
    controlWhtieNoise(Otsu());//更改RGB：用自动阈值去噪
    reverseRGB();			//取反RGB
    saveIplImage();		//将RGB保存到图像指针
    showImg();
    Image gray(equalizeHist());	//均衡化
    //gray.showImg();
    gray.equalizeHist();//目标是黑色点则再进行一次均衡化
    *this = gray;///
    //gray.showImg();
    saveRGB();
    reverseRGB();
    saveIplImage();
    if(save)
        outputImg("histeq",0);
}

#ifdef __AFXWIN_H__
#pragma endregion
#endif


//轻重比比较类
class WM {
    int _n_,_m_,each_part,tran,th_;
    uchar pp[MAXSIZE*MAXSIZE];	//用来读取数组的模板
    uchar V[MAXSIZE*MAXSIZE];	//灰度数组
    const Image *_img;
public:
    WM(int n=20,int m=20) {
        _n_ = n;
        _m_ = m;
        each_part = tran=0;
    }
    // 将像素数据存入到一个整形数组中
    void saveToArr(const Image &img,int th = 180) {
        _img = &img;
        if(th>=0 && th<256)
            th_ = th;
        //V.resize(width * height);
        int i,j,row,line;
        for(i = 0; i < _img->width * _img->height; i++) {
            j = 0;
            row = i / _img->width, line = i % _img->width;
            V[i]=(_img->R[row][line]+_img->G[row][line]+_img->B[row][line]+1)/3;
            if(V[i]>th_)
                V[i] = 255;
        }
    }
    //轮廓跟踪之后3*3矩阵平滑处理，做灰度直方图，灰度值V是max（R,G,B）,对原图的数值进行处理
    void binaryzation(int a,int b,int KY) { //寻找点
        int mat=0;
        for(int i=0; i<each_part; i++)
            for(int j=0; j<tran; j++) {
                int zz = a*each_part*_img->width + b*tran + i*_img->width + j;
                if(V[zz]>KY)	//轻笔变为非笔迹
                    V[zz]=255;
                else			//重笔则统计数目
                    mat++;
            }
    }
    //统计笔迹点数目
    int mark(int a,int b) {
        int mat=0;
        for(int i=0; i<each_part; i++)
            for(int j=0; j<tran; j++)
                if(V[a*each_part*_img->width + b*tran + i*_img->width + j]!=255)
                    mat++;
        return mat;
    }
    template<class T1>
    int Otsu(T1* arr,int size,int size2) {
        double histogram[256] = {0};
        int i,threshold;
        for(i=0; i < size; i++)
            if(arr[i]<=th_)
                histogram[arr[i]]++;
        for(i = 0; i < th_; i++)
            if(histogram[i]>.99)
                histogram[i] /= size2;
        double avgValue=0, maxVariance=0, w = 0, u = 0;
        for(i=0; i < th_; i++)
            avgValue += i * histogram[i];
        for(i = 0; i < th_; i++) {
            w += histogram[i];
            u += i * histogram[i];
            double t = avgValue * w - u;
            double variance = t * t / (w * (1 - w) );
            if(variance > maxVariance) {
                maxVariance = variance;
                threshold = i;
            }
        }
        return threshold;
    }

    void Outline_pretreatment(uchar *pp,int a,int b,float *res) {
        int before = mark(a,b);
        if(before==0) {
            res[a*_m_+b]=0;
            return;
        }
        int KT = Otsu(pp,each_part*tran,before);
        binaryzation(a,b,KT); //将选中的部分进行局域二值化处理
        int after = mark(a,b);
        if(after==0)
            res[a*_m_+b]=0;
        else
            res[a*_m_+b]= (float)after/before;
    }

    void subsection_mactching(float *res) {
        each_part = _img->height/_n_; //每一段有几行
        tran = _img->width/_m_; //用来记录横向的分段
        memset(pp,0,sizeof(pp));
        for(int a=0; a<_n_; a++)
            for(int b=0; b<_m_; b++) {
                for(int i=0; i<each_part; i++)
                    for(int j=0; j<tran; j++)
                        pp[i*tran+j]=V[a*each_part*_img->width+b*tran+i*_img->width+j];
                Outline_pretreatment(pp,a,b,res);
            }
    }

    void matching(float *res,float *res2,double* mw) {
        int k,j,nm = _n_*_m_/4;
        for(j=0; j<4; j++)
            mw[j] = 0;
        for(k=0; k<_n_/2; k++)
            for(j=0; j<_m_/2; j++)
                if(fabs(res[j*_n_+k]-res2[j*_n_+k])>0.3)
                    mw[0]++;
        for(k=_n_/2; k<_n_; k++)
            for(j=0; j<_m_/2; j++)
                if(fabs(res[j*_n_+k]-res2[j*_n_+k])>0.3)
                    mw[1]++;
        for(k=0; k<_n_/2; k++)
            for(j=_m_/2; j<_m_; j++)
                if(fabs(res[j*_n_+k]-res2[j*_n_+k])>0.3)
                    mw[2]++;
        for(k=_n_/2; k<_n_; k++)
            for(j=_m_/2; j<_m_; j++)
                if(fabs(res[j*_n_+k]-res2[j*_n_+k])>0.3)
                    mw[3]++;
        for(j=0; j<4; j++)
            mw[j] = 1-mw[j]/nm;
    }
};

void matchWeight(cchar* file1,cchar* file2,double* mw,int show,int n,int m) {
    WM wm1(n,m),wm2(n,m);
    float res[2][10000]= {0};
    Image img1(file1,0,0,1,show);
    img1.saveRGB();
    wm1.saveToArr(img1);
    wm1.subsection_mactching(res[0]);
    Sleep(1);
    img1.loadImg(file2,0,0,1,show);
    img1.saveRGB();
    wm2.saveToArr(img1);
    wm2.subsection_mactching(res[1]);
    wm1.matching(res[0],res[1],mw);
#ifndef __AFXWIN_H__
    cout<<"轻重比相似概率为 :\t"<<mw[0]<<","<<mw[1]<<","<<mw[2]<<","<<mw[3]<<endl;
#else
    sprintf(info,"%s轻重比相似概率为 :\t%g,%g,%g,%g\r\n",info,mw[0],mw[1],mw[2],mw[3]);
#endif
}


// 将一幅图缩放和旋转变换后保存到另一幅图中
void imRotate(const IplImage *src,IplImage *&dst,double scale=1,double angle=0,CvPoint2D32f center=cvPoint2D32f(-1,-1)) {
    if(dst==0)
        return;
    if(src->width == dst->width && src->height == dst->height
            &&src->depth == dst->depth &&src->nChannels == dst->nChannels) {
        CvMat *mapMatrix = cvCreateMat(2,3,CV_32FC1);
        if(center.x==-1 && center.y==center.x)
            center = cvPoint2D32f(src->width/2,src->height/2);
        cv2DRotationMatrix(center,angle,scale,mapMatrix);//计算二维旋转的仿射变换矩阵
        cvWarpAffine(src,dst,mapMatrix);    //对图像做仿射变换
    } else
#ifndef __AFXWIN_H__
        cout<<"两幅图类型或规格不一致,无法进行转化"<<endl;
#else
        sprintf(info,"%s两幅图类型或规格不一致,无法进行转化\r\n",info);
#endif
}
#endif // _IMGPRO_H_
