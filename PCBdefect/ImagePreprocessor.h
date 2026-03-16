#pragma once
#include <opencv2/opencv.hpp>
//-------------------------------------------------------------
// 역할 : 영상 전처리 파이프라인을 제공하는 클래스
// 매칭 정확도를 높이기 위한 다양한 전처리 기법을 단일 호출로 제공
//
// 목적
// - 조명 변화, 노이즈, 작은 차이에도 안정적인 매칭 성능 확보
// - PCB/제품 검사에서 기준 이미지와 실제 검사 이미지 간 일관성 맞추기
//-------------------------------------------------------------
class ImagePreprocessor {
public:
	cv::Mat grayScale(const cv::Mat& input);		// 컬러(BGR 3채널) → 그레이스케일(1채널) 변환
	cv::Mat gaussianBlur(const cv::Mat& input, int ksize = 5);	//  노이즈 제거 및 엣지 부드럽게 처리
	cv::Mat histogramEq(const cv::Mat& input);		// 대비 향상 및 조명 정규화
	cv::Mat morphologyEx(const cv::Mat& input);		// 침식, 팽창, 열기, 닫기 등의 형태학 연산 수행
	cv::Mat all(const cv::Mat& input);				// 모든 전처리를 순차적으로 적용하는 일괄 함수
};