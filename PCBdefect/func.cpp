#include "pch.h"
#include "PCBdefectDlg.h"

// =====================================================
// 이미지 컨트롤에 이미지를 그리기 위한 공통 함수
// img: 표시할 이미지 객체
// control: 그림을 그릴 Picture Control
// =====================================================
void CPCBdefectDlg::DrawMat(cv::Mat& mat, CWnd& control) {
    // Picture Control의 클라이언트 영역 크기 가져오기
    CRect rect;
    control.GetClientRect(&rect);

    // DC(Device Context) 획득 (그리기용)
    CDC* dc = control.GetDC();

    //  BITMAPINFO 구조체 초기화 (Windows DIB 포맷)
    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = mat.cols;                    // 원본 너비
    bmi.bmiHeader.biHeight = -mat.rows;                  // 높이(음수=top-down)
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;
    bmi.bmiHeader.biCompression = BI_RGB;

    // 채널 수에 따른 비트 깊이 설정
    if (mat.channels() == 1) {
        bmi.bmiHeader.biBitCount = 8;     // 그레이스케일
    }
    else {
        bmi.bmiHeader.biBitCount = 24;    // 컬러 BGR
        bmi.bmiHeader.biCompression = BI_RGB;
    }

    //    StretchDIBits()로 이미지 렌더링
    //    - 목적지: (0,0) ~ (rect.Width(), rect.Height())
    //    - 원본: mat 전체 영역
    //    - DIB_RGB_COLORS: 팔레트 색상 매핑
    //    - SRCCOPY: 픽셀 복사
    ::StretchDIBits(dc->GetSafeHdc(),
        0, 0, rect.Width(), rect.Height(),      // 목적지: 컨트롤 전체 크기
        0, 0, mat.cols, mat.rows,               // 원본: 이미지 전체
        mat.data, &bmi, DIB_RGB_COLORS, SRCCOPY);
    // DC 해제 (메모리 누수 방지)
    control.ReleaseDC(dc);
}
// =====================================================
// 리스트박스에서 선택한 이미지 파일을 로드하여 화면에 표시
// fileName: 리스트박스에 표시된 파일명 (확장자 제외, 예: "template001")
// =====================================================
void CPCBdefectDlg::imgLoad(CString fileName) {
    CString fullPath;                   // 완전한 파일 경로를 저장할 변수
    // 선택된 폴더 + 파일명 + .jpg 확장자로 전체 경로 생성
    fullPath.Format(_T("%s\\%s.jpg"), m_selectedFolder, fileName);

    // CString → std::string 변환
    std::string filePath;
    CT2A converter(fullPath);  // CString → char*
    filePath = converter;      // char* → std::string

    cv::Mat mat = cv::imread(filePath);

    if (mat.empty()) {
        AfxMessageBox(_T("로드 실패! 파일 확인하세요"));
        return;
    }

    // 파일명에 "test" 문자열 포함 여부로 이미지 종류 판단
    if (fileName.Find(_T("test")) != -1) {  // "test001" → 테스트 이미지
        test_Mat = mat;
        DrawMat(mat, m_pic_test);         // 오른쪽 테스트 Picture Control에 표시
    }
    else {                                  // "template001" → 템플릿 이미지
        temple_Mat = mat;
        DrawMat(mat, m_pic_temple);       // 왼쪽 템플릿 Picture Control에 표시
    }

}

