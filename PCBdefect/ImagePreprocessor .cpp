#include "pch.h"
#include "ImagePreprocessor.h"
//-------------------------------------------------------------
// 그레이스케일 변환 (BGR -> GRAY -> BGR)
// 컬러 정보를 제거하고 밝기 정보만 유지
// 입력/출력 모두 3채널 BGR 형식 유지 (Match 클래스와 호환)
//-------------------------------------------------------------
cv::Mat ImagePreprocessor::grayScale(const cv::Mat& input) {
	cv::Mat result, gray;
	// BGR(3채널) → GRAY(1채널)
	cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);

	// GRAY(1채널) → BGR(3채널)로 복원 (채널 수 통일)
	cv::cvtColor(gray, result, cv::COLOR_GRAY2BGR);

	return result;  // 3채널 BGR 출력 (원본과 동일한 포맷)
}

//-------------------------------------------------------------
// 가우시안 블러 적용 (노이즈 제거)
// ksize x ksize 커널 사용, sigma는 자동 계산
// 고주파 노이즈 제거 및 작은 픽셀 차이 무시
//-------------------------------------------------------------
cv::Mat ImagePreprocessor::gaussianBlur(const cv::Mat& input, int ksize) {
	cv::Mat result;
	// ksize×ksize 커널로 가우시안 블러 적용
	// sigmaX=0, sigmaY=0: 자동 계산 (ksize/6 근사값)
	cv::GaussianBlur(input, result, cv::Size(ksize, ksize), 0, 0);

	return result;  // 원본과 동일 채널의 블러 처리 이미지
}

//-------------------------------------------------------------
// CLAHE (대비 제한 적응형 히스토그램 평활화)
// 조명 불균일 보정 및 대비 향상
// clipLimit=1.5, 8x8 타일로 국부적 처리
//-------------------------------------------------------------
cv::Mat ImagePreprocessor::histogramEq(const cv::Mat& input) {
	cv::Mat result, grayImg;
	// BGR → GRAY 변환 (히스토그램 평활화는 1채널에서만 동작)
	cv::cvtColor(input, grayImg, cv::COLOR_BGR2GRAY);

	// CLAHE 객체 생성
	// clipLimit=1.5: 대비 증폭 제한 (노이즈 방지)
	// tileGridSize=8x8: 64개 타일로 국부적 처리
	cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(1.5, cv::Size(8, 8));

	// CLAHE 적용
	clahe->apply(grayImg, result);

	// GRAY → BGR 복원 (3채널 출력)
	cv::cvtColor(result, result, cv::COLOR_GRAY2BGR);

	return result;
}

//-------------------------------------------------------------
// 형태학적 연산 (Morphological Opening)
// 작은 노이즈 제거를 위한 침식 후 팽창
// 3x3 타원형 커널 사용
//-------------------------------------------------------------
cv::Mat ImagePreprocessor::morphologyEx(const cv::Mat& input) {
	cv::Mat result;
	// 구조적 요소(커널) 생성
	// MORPH_ELLIPSE: 타원형 (사각형보다 자연스러운 결과)
	// Size(3,3): 3x3 크기의 작은 노이즈 제거에 적합
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));

	// MORPH_OPEN: 침식 → 팽창 (작은 흰색 점 제거)
	// 내부 노이즈만 제거하고 주요 객체는 보존
	cv::morphologyEx(input, result, cv::MORPH_OPEN, kernel);

	return result;
}

//-------------------------------------------------------------
// 모든 전처리 파이프라인 일괄 실행
// 처리 순서: gaussianBlur -> histogramEq -> morphologyEx
// 단일 호출로 최적화된 전처리 체인 실행
//-------------------------------------------------------------
cv::Mat ImagePreprocessor::all(const cv::Mat& input) {
	cv::Mat result = input.clone();

	result = gaussianBlur(result);
	result = histogramEq(result);
	result = morphologyEx(result);

	return result;
}

