
// PCBdefectDlg.h: 헤더 파일
//

#pragma once


// CPCBdefectDlg 대화 상자
class CPCBdefectDlg : public CDialogEx
{
private:
	//---------------------------------------------------------
	// 주요 모듈 객체 멤버
	//---------------------------------------------------------
	ImagePreprocessor m_preprocessor;  // 영상 전처리 담당 클래스 (Grayscale, Blur 등)
	Match m_match;                     // 영상 매칭 담당 클래스 
	cv::Mat temple_Mat;                // 템플릿(기준) 이미지 원본
	cv::Mat test_Mat;                  // 테스트(비교 대상) 이미지 원본
	cv::Mat m_processedTemplate;       // 전처리된 템플릿 이미지
	cv::Mat m_processedTest;           // 전처리된 테스트 이미지
	cv::Mat boundingImg;               // 매칭(차이 검출) 결과 이미지 (박스 표시 포함)
// 생성입니다.
public:
	CPCBdefectDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.
	CString m_strHome; // 실행파일(exe)이 저장된 디렉토리 경로 저장 변수
	CString Out;  // 배치 파일 저장 경로 
	static Detector* m_pDetector; // YOLO object detector
	static std::vector<CString> m_vecClassName; // 클래스 이름 목록을 저장하는 벡터
	
	void imgLoad(CString fileName);            // 리스트박스에서 선택한 이미지 파일을 로드하여 화면에 표시
	void DrawMat(cv::Mat& mat, CWnd& control); // 이미지 컨트롤에 이미지를 그리기 위한 공통 함수

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PCBDEFECT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//---------------------------------------------------------
	// 파일 및 UI 관련 멤버
	//---------------------------------------------------------
	CString m_selectedFolder;  // 현재 선택된 폴더 경로 (이미지 리스트 검색용)
	CListBox curDir;          // 폴더 내 이미지 파일 이름을 보여주는 리스트 박스
	CStatic m_pic_temple;      // 템플릿(기준 이미지) 표시용 Picture Control
	CStatic m_pic_test;        // 테스트(비교 이미지) 표시용 Picture Control

	afx_msg void OnBnClickedOpen();			// [열기] 버튼 – 폴더 선택 및 이미지 목록 로드
	afx_msg void OnLbnDblclkDir();			// 이미지 리스트 더블클릭 시 이미지 로드 
	afx_msg void OnBnClickedGrayscale();	// [Grayscale] 버튼 – 흑백 변환 수행
	afx_msg void OnBnClickedGaussianblur(); // [Blur] 버튼 – 가우시안 블러 전처리
	afx_msg void OnBnClickedHistogram();	// [Histogram Equalization] 버튼
	afx_msg void OnBnClickedMorphology();	// [Morphology] 버튼 – 침식/팽창/열기/닫기 등 수행
	afx_msg void OnBnClickedAll();			// [All] 버튼 – 모든 전처리 일괄 수행
	afx_msg void OnBnClickedMatch();		// [Match] 버튼 – 영상 매칭 실행
	afx_msg void OnBnClickedClear();		// [Clear] 버튼 – 결과 초기화
	afx_msg void OnBnClickedClose();		// [Close] 버튼 – 프로그램 종료 또는 다이얼로그 닫기
	afx_msg void OnBnClickedExpand();		// [Expand] 버튼 - yolo 처리 결과 확대 
	afx_msg void OnBnClickedBacth();
};
