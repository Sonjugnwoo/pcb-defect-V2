#pragma once
#include "../Yolo/yolo_v2_class.hpp"
#include <opencv2/opencv.hpp>

// 역할 : 두 이미지(템플릿 vs 테스트)를 비교하여 차이가 있는 영역을 검출하고,
// 검출된 영역을 빨간색 바운딩 박스로 표시하는 영상 매칭 엔진 클래스.
class Match {
private:
    std::map <int, int> m_classCount;   // 클래스별 검출 개수 저장 (key: 클래스ID, value: 개수)
public:
   //-------------------------------------------------------------
   // 검출 결과 개수 반환 함수
   // m_classCount 맵에서 총 검출된 결함 개수를 문자열로 반환
   //-------------------------------------------------------------
    CString GetClassCount();

    //---------------------------------------------------------
    // 핵심 매칭 함수
    // **입력**:
    //   - tempImg: 기준(템플릿) 이미지 - cv::Mat (BGR 컬러 이미지)
    //   - testImg: 비교 대상(테스트) 이미지 - cv::Mat (BGR 컬러 이미지)
    //     * 주의: 두 이미지의 크기와 채널이 동일해야 함
    //
    // **처리 과정**:
    //   1. absdiff()로 픽셀 단위 절대 차이 계산
    //   2. 그레이스케일 변환 후 threshold 이진화(30 이상)
    //   3. Morphology Opening으로 노이즈 제거
    //---------------------------------------------------------
    cv::Mat matchTemple(const cv::Mat& tempImg, const cv::Mat& testtImg);


    //-------------------------------------------------------------
    // YOLO 검출 결과 시각화 함수
    //-------------------------------------------------------------
    void drawDetection(std::vector<bbox_t> &result, cv::Mat& img);


};