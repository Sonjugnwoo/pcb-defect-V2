#include "pch.h"
#include "PCBdefectDlg.h"
#include "match.h"
std::vector<CString> CPCBdefectDlg::m_vecClassName; //  클래스명 저장소 (pcb-error.names 파일에서 로드)
Detector* CPCBdefectDlg::m_pDetector;   // YOLOv3 검출기 포인터
// 클래스별 고정 색상 정의
static cv::Scalar classColors[] = {
    cv::Scalar(0, 255, 0),      // 0: Green  - open_circuit 
    cv::Scalar(0, 0, 255),      // 1: Red    - short
    cv::Scalar(255, 0, 0),      // 2: Blue   - mouse_bite
    cv::Scalar(0, 255, 255),    // 3: Yellow - spurious_copper 
    cv::Scalar(255, 0, 255),    // 4: Magenta- spur
    cv::Scalar(255, 255, 0)     // 5: Cyan   - pin_hole 

};
//-------------------------------------------------------------
// 템플릿 매칭 기반 차이 검출
// 기준 이미지와 테스트 이미지 비교하여 불일치 영역 검출
// PCB 불량 검출, 제품 외관 검사에 최적화
//-------------------------------------------------------------
cv::Mat Match::matchTemple(const cv::Mat& tempImg, const cv::Mat& testImg) {
    // 입력 이미지 유효성 검사
    // 비어있는 이미지는 MFC 메시지박스로 사용자에게 알림
    if (tempImg.empty() || testImg.empty()) {
        AfxMessageBox(_T("이미지 비어있음"));
        return cv::Mat();
    }

    // 결과 이미지 초기화 (testImg 복사본)
    cv::Mat result = testImg.clone();

    // 두 이미지의 픽셀별 절대 차이 계산
    cv::Mat diff;
    cv::absdiff(tempImg, testImg, diff);

    // 차이 이미지를 그레이스케일로 변환 (1채널 분석용)
    cv::cvtColor(diff, diff, cv::COLOR_BGR2GRAY);

    // 이진화: 차이가 30 이상인 픽셀만 흰색(255)으로 강조
    cv::Mat thresh;
    cv::threshold(diff, thresh, 30, 255, cv::THRESH_BINARY);

    // 5x5 사각형 커널로 Morphological Opening 적용
    // 작은 노이즈 제거를 위한 침식 → 팽창
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::morphologyEx(thresh, thresh, cv::MORPH_OPEN, kernel);

    cv::Mat frame = result.clone();
    std::vector<bbox_t> defects = CPCBdefectDlg::m_pDetector->detect(frame, 0.5f);
    
    m_classCount.clear();

    drawDetection(defects, result);

    // 멤버 변수에 최종 검출 개수 저장
    return result;
}

//-------------------------------------------------------------
// YOLO 검출 결과 시각화 함수
//-------------------------------------------------------------
void Match::drawDetection(std::vector<bbox_t> &result, cv::Mat& img) {
    for (int i = 0; i < result.size(); i++) {    // 각 검출된 객체 순회
        //  클래스명 추출 (전역 벡터에서 obj_id로 조회)
        CString strName = CPCBdefectDlg::m_vecClassName[result[i].obj_id];
        std::string stringText = std::string(CT2CA(strName.operator LPCWSTR()));

        // 클래스별 색상 선택 (6개 색상 배열 순환)
        cv::Scalar color = classColors[(result[i].obj_id) % 6];
        
        //  클래스별 검출 개수 카운터 증가
        m_classCount[result[i].obj_id]++;

        //  바운딩 박스 그리기
        // 좌상단(x,y) → 우하단(x+w, y+h), 두께=2px
        cv::rectangle(img,
            cv::Point(result[i].x, result[i].y), // 좌상단
            cv::Point(result[i].x + result[i].w, result[i].y + result[i].h), // 우하단
            color, 2);                                                     // 색상, 두께

        // 클래스명 라벨 표시
        cv::putText(img, stringText,
            cv::Point(result[i].x, result[i].y - 10),   // 텍스트 시작 위치
            cv::FONT_HERSHEY_SIMPLEX, 0.6, color, 2);   // 폰트, 크기, 색상, 두께
    }
}

//-------------------------------------------------------------
//검출 결과 요약 문자열 생성 함수
//-------------------------------------------------------------
CString Match::GetClassCount() {
    CString text;
    // 총 결함 수 계산 (모든 클래스 개수 합산)
    int totalDefects = 0;
    for (const auto& p : m_classCount) {
        totalDefects += p.second;   // 각 클래스별 개수 누적
    }
    text.Format(_T("발견된 결함: 총 %d개\n"), totalDefects);   //  총 결함 수 헤더 라인

    // 클래스명 + 개수 표시
    for (const auto& p : m_classCount) {
        int classId = p.first;   // 클래스 ID
        int count = p.second;    // 해당 클래스 검출 개수
        CString className = CPCBdefectDlg::m_vecClassName[classId]; // 클래스명 조회(pcb - error.names에서)
        text.AppendFormat(_T("  - %s : %d개\n"), className, count);  //   - 클래스명: N개\n"
    }


    return text;
}
