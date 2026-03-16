
// PCBdefectDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "PCBdefect.h"
#include "PCBdefectDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPCBdefectDlg 대화 상자



CPCBdefectDlg::CPCBdefectDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PCBDEFECT_DIALOG, pParent)
	, m_selectedFolder(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPCBdefectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DIR, curDir);
	DDX_Control(pDX, IDC_PIC_TEM, m_pic_temple);
	DDX_Control(pDX, IDC_PIC_TEST, m_pic_test);
	DDX_Text(pDX, IDC_PATH, m_selectedFolder);
}

BEGIN_MESSAGE_MAP(CPCBdefectDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OPEN, &CPCBdefectDlg::OnBnClickedOpen)
	ON_LBN_DBLCLK(IDC_DIR, &CPCBdefectDlg::OnLbnDblclkDir)
	ON_BN_CLICKED(IDC_GRAYSCALE, &CPCBdefectDlg::OnBnClickedGrayscale)
	ON_BN_CLICKED(IDC_GAUSSIANBLUR, &CPCBdefectDlg::OnBnClickedGaussianblur)
	ON_BN_CLICKED(IDC_HISTOGRAM, &CPCBdefectDlg::OnBnClickedHistogram)
	ON_BN_CLICKED(IDC_MORPHOLOGY, &CPCBdefectDlg::OnBnClickedMorphology)
	ON_BN_CLICKED(IDC_ALL, &CPCBdefectDlg::OnBnClickedAll)
	ON_BN_CLICKED(IDC_MATCH, &CPCBdefectDlg::OnBnClickedMatch)
	ON_BN_CLICKED(IDC_CLEAR, &CPCBdefectDlg::OnBnClickedClear)
	ON_BN_CLICKED(IDC_CLOSE, &CPCBdefectDlg::OnBnClickedClose)
	ON_BN_CLICKED(IDC_EXPAND, &CPCBdefectDlg::OnBnClickedExpand)
	ON_BN_CLICKED(IDC_BACTH, &CPCBdefectDlg::OnBnClickedBacth)
END_MESSAGE_MAP()


// CPCBdefectDlg 메시지 처리기

BOOL CPCBdefectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	// -----------------------------------------
	// 실행파일(exe)이 저장된 디렉토리 경로 얻기...
	// -----------------------------------------
	wchar_t path[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, path, MAX_PATH); // "...\Exe\ObjectDetector.exe" 경로
	CString msg(path);
	int n = msg.ReverseFind('\\'); // ...\Exe\ 까지 위치, 0,1,...n
	m_strHome = msg.Left(n + 1); // ...\Exe\ 까지 문자열 복사, 좌측에서 (n+1)개 복사
	int n_out = msg.Left(n).ReverseFind('\\');
	Out = msg.Left(n_out+1) + L"Out";
	// -----------------------------------------
	// Initialize Darknet YOLOv3 detector & load model files
	// -----------------------------------------
	CString str_cfg = m_strHome + L"yolov3-pcb.cfg";
	CString str_wgt = m_strHome + L"yolov3-pcb.weights";
	std::string cfg = std::string(CT2CA(str_cfg.operator LPCWSTR()));
	std::string wgt = std::string(CT2CA(str_wgt.operator LPCWSTR()));
	m_pDetector = new Detector(cfg, wgt);

	m_vecClassName.clear();
	wchar_t buffer[125];
	// coco.names -> UTF-8 decoding 
	CString namePath = m_strHome + L"pcb-error.names";
	FILE* stream = _wfopen(namePath, L"rt+,ccs=UTF-8");
	if (stream == NULL) {
		AfxMessageBox(L"Can't open " + namePath);
		return false;
	}
	// 텍스트를 한 줄씩 읽어서 vector에 저장한다.
	// 한 줄 읽으면 끝에 dummy 문자가 추가된 것 제거
	while (fgetws(buffer, 125, stream) != NULL) {
		CString strClass(buffer);
		CString strOnlyChar = strClass.Left(strClass.GetLength() - 1);
		m_vecClassName.push_back(strOnlyChar);
	}
	fclose(stream);
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CPCBdefectDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
	SetDlgItemText(IDC_STATUS, _T("PCB 검사 가이드\r\n"
		"1-OPEN → 폴더 선택\r\n"
		"2-더블클릭 → 이미지 로드\r\n"
		"3-전처리 버튼들 순서대로\r\n"
		"4-ALL → 전체 처리\r\n"
		"5-MATCH → 부품 검출\r\n"
		"6-BATCH → 폴더 안 파일 전부 처리\r\n"));
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CPCBdefectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
// ======================================================================
// 폴더 선택 대화상자의 콜백 함수
// BFFM_INITIALIZED 메시지에서 초기 폴더 경로를 설정
// ======================================================================
int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData) {
	// 콜백 메시지별 처리
	switch (uMsg) {
	case BFFM_INITIALIZED:              // 대화상자가 초기화될 때 호출됨
		// lpData에 전달된 초기 경로(C:\)를 대화상자에 선택 상태로 설정
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
		break;
	}
	return 0;                           // 콜백 처리 완료
}


void CPCBdefectDlg::OnBnClickedOpen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ITEMIDLIST* pidlBrowes;         // 폴더 선택 결과를 담는 구조체 포인터
	WCHAR pszPathname[MAX_PATH];    // 선택된 폴더의 전체 경로를 저장할 배열

	BROWSEINFO BrInfo;              // 폴더 선택 대화상자 설정에 사용하는 구조체

	BrInfo.hwndOwner = GetSafeHwnd();   // 부모 윈도우 핸들 등록 (현재 다이얼로그)
	BrInfo.pidlRoot = NULL;             // 루트 폴더를 지정하지 않음 (기본은 "내 컴퓨터")

	memset(&BrInfo, 0x00, sizeof(BrInfo));  // 구조체를 0으로 초기화
	BrInfo.pszDisplayName = pszPathname;    // 선택된 폴더명이 저장될 버퍼
	BrInfo.ulFlags = BIF_RETURNONLYFSDIRS;  // 실제 파일 시스템 폴더만 선택 가능 (가상 폴더 제외)

	BrInfo.lpfn = BrowseCallbackProc;       // 콜백 함수 포인터 (초기 위치 설정 등에 사용 가능)

	CString strInitPath = _T("C:\\");       // 폴더 대화상자 초기 경로 설정
	BrInfo.lParam = (LPARAM)strInitPath.GetBuffer();  // 초기 경로를 콜백에 전달

	// 폴더 선택 대화상자 실행
	pidlBrowes = ::SHBrowseForFolder(&BrInfo);

	// 사용자가 폴더를 선택했을 경우
	if (pidlBrowes != NULL) {
		SHGetPathFromIDList(pidlBrowes, pszPathname);  // ITEMIDLIST → 실제 경로 문자열로 변환

		CString folderPath(pszPathname);   // 예: "C:\\PCB_images"
		CString tpath;

		SetDlgItemText(IDC_PATH, folderPath);

		tpath.Format(_T("%s\\*.jpg"), folderPath);  // 선택 폴더 내의 모든 JPG 파일 검색 경로

		m_selectedFolder = folderPath;     // 멤버 변수에 선택된 폴더 경로 저장
		CFileFind finder;                  // 파일 탐색용 CFileFind 객체
		BOOL bWorking = finder.FindFile(tpath);  // 첫 파일 검색 시작
		curDir.ResetContent();            // 리스트박스 내용 초기화
		CString fileName, DirName;

		// 폴더 내 모든 jpg 파일 순회
		while (bWorking) {
			bWorking = finder.FindNextFile();  // 다음 파일 검색

			if (finder.IsDirectory() || finder.IsDots())	// 폴더나 . .. 는 건너뛰기
				continue;

			fileName = finder.GetFileTitle();        // 확장자 제외한 파일명만 추출

			curDir.AddString(fileName);             // 리스트박스에 파일명 추가
		}
	}
}


void CPCBdefectDlg::OnLbnDblclkDir()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
		// 기존 전처리 결과 초기화 (새로운 이미지 로드시 기존 결과 삭제)
	if (!m_processedTemplate.empty() || !m_processedTest.empty()) {
		m_processedTemplate.release();
		m_processedTest.release();
		SetDlgItemText(IDC_STATUS, _T("Status\n"));
	}

	CString fileName;
	int index = curDir.GetCurSel();     // 현재 선택된 항목의 인덱스를 가져옴

	// 짝수(템플릿), 홀수(테스트) 파일이 짝으로 구성되어 있을 경우
	// 홀수를 클릭했더라도 항상 짝수부터 로드하도록 인덱스 조정
	if (index % 2 == 1)
		index -= 1;
	if (index + 1 >= curDir.GetCount()) return;
	CString templateName, testName;
	curDir.GetText(index, templateName);      // 선택된 항목의 파일명 (템플릿)
	curDir.GetText(index + 1, testName);      // 바로 다음 항목의 파일명 (테스트)

	imgLoad(templateName); // 첫 번째 이미지 로드 (템플릿)
	imgLoad(testName);     // 두 번째 이미지 로드 (테스트) 
}


//-------------------------------------------------------------
// 그레이스케일 버튼 클릭 핸들러
// 양쪽 이미지에 Grayscale 변환 적용
// 전처리 결과 Picture Control에 표시
//-------------------------------------------------------------
void CPCBdefectDlg::OnBnClickedGrayscale()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (temple_Mat.empty() || test_Mat.empty()) {
		SetDlgItemText(IDC_STATUS, _T("먼저 이미지를 더블클릭해서 로드하세요\n"));
		return;
	}
	// 템플릿과 테스트 이미지 모두 Grayscale 변환
	m_processedTemplate = m_preprocessor.grayScale(temple_Mat);
	m_processedTest = m_preprocessor.grayScale(test_Mat);

	DrawMat(m_processedTest, m_pic_test);
	SetDlgItemText(IDC_STATUS, _T("그레이 스케일 완료\n"));
}


//-------------------------------------------------------------
// 가우시안 블러 버튼 클릭 핸들러
// ksize=5로 노이즈 제거
// Grayscale 이후 단계에서만 실행 가능
//-------------------------------------------------------------
void CPCBdefectDlg::OnBnClickedGaussianblur()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_processedTest.empty()) {
		SetDlgItemText(IDC_STATUS, _T("먼저 그레이스케일 버튼을 누르세요!\n"));
		return;
	}

	// 양쪽 이미지에 동일한 블러 적용
	m_processedTemplate = m_preprocessor.gaussianBlur(m_processedTemplate);
	m_processedTest = m_preprocessor.gaussianBlur(m_processedTest, 5);

	DrawMat(m_processedTest, m_pic_test);
	SetDlgItemText(IDC_STATUS, _T("가우시안 블러 완료\n"));
}

//-------------------------------------------------------------
// 히스토그램 평활화 버튼 클릭 핸들러
// CLAHE 적용으로 대비 향상
//-------------------------------------------------------------
void CPCBdefectDlg::OnBnClickedHistogram()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_processedTest.empty()) {
		SetDlgItemText(IDC_STATUS, _T("먼저 그레이스케일 버튼을 누르세요!\n"));
		return;
	}
	m_processedTemplate = m_preprocessor.histogramEq(m_processedTemplate);
	m_processedTest = m_preprocessor.histogramEq(m_processedTest);
	DrawMat(m_processedTest, m_pic_test);

	SetDlgItemText(IDC_STATUS, _T("히스토그램 평탄화 완료\n"));
}


//-------------------------------------------------------------
// 모폴로지 연산 버튼 클릭 핸들러
// Morphological Opening으로 작은 노이즈 제거
//-------------------------------------------------------------
void CPCBdefectDlg::OnBnClickedMorphology()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_processedTest.empty()) {
		SetDlgItemText(IDC_STATUS, _T("먼저 그레이스케일 버튼을 누르세요!\n"));
		return;
	}
	m_processedTemplate = m_preprocessor.morphologyEx(m_processedTemplate);
	m_processedTest = m_preprocessor.morphologyEx(m_processedTest);
	DrawMat(m_processedTest, m_pic_test);

	SetDlgItemText(IDC_STATUS, _T("모폴로지  완료\n"));
}

//-------------------------------------------------------------
// 전체 전처리 버튼 클릭 핸들러
// 모든 전처리를 순차적으로 일괄 실행
// Grayscale이 없으면 자동으로 먼저 실행
//-------------------------------------------------------------
void CPCBdefectDlg::OnBnClickedAll()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (temple_Mat.empty() || test_Mat.empty()) {
		AfxMessageBox(_T("먼저 이미지를 더블클릭해서 로드하세요!"));
		return;
	}
	if (m_processedTest.empty()) {
		// Grayscale 전처리가 없으면 먼저 실행
		m_processedTemplate = m_preprocessor.grayScale(temple_Mat);
		m_processedTest = m_preprocessor.grayScale(test_Mat);
	}
	m_processedTemplate = m_preprocessor.all(m_processedTemplate);
	m_processedTest = m_preprocessor.all(m_processedTest);
	DrawMat(m_processedTest, m_pic_test);


	SetDlgItemText(IDC_STATUS, _T("모든 전처리 완료\n"));
}

//-------------------------------------------------------------
// 템플릿 매칭 버튼 클릭 핸들러
// 전처리된 이미지로 차이 검출 수행
// 검출된 결함 개수 UI에 표시
//-------------------------------------------------------------
void CPCBdefectDlg::OnBnClickedMatch()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_processedTemplate.empty() || m_processedTest.empty()) {
		SetDlgItemText(IDC_STATUS, _T("전처리 먼저 해주세요!\n"));
		return;
	}
	// 매칭 실행 및 바운딩 박스 결과 획득
	boundingImg = m_match.matchTemple(m_processedTemplate, m_processedTest);

	DrawMat(boundingImg, m_pic_test);

	// 검출된 결함 개수 표시
	CString text = m_match.GetClassCount();

	SetDlgItemText(IDC_STATUS,text);
}

//-------------------------------------------------------------
// 결과 초기화 버튼
// 전처리 결과만 해제하고 원본 이미지만 표시
//-------------------------------------------------------------
void CPCBdefectDlg::OnBnClickedClear()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (temple_Mat.empty() || test_Mat.empty()) {
		SetDlgItemText(IDC_STATUS, _T("먼저 이미지를 더블클릭해서 로드하세요!\n"));
		return;
	}
	if (m_processedTemplate.empty() || m_processedTest.empty()) {
		SetDlgItemText(IDC_STATUS, _T("전처리된 이미지가 없습니다 .\n"));
		return;
	}

	m_processedTemplate.release();
	m_processedTest.release();
	boundingImg.release();
	SetDlgItemText(IDC_STATUS, _T("초기화 완료\n"));

	DrawMat(test_Mat, m_pic_test);
	DrawMat(temple_Mat, m_pic_temple);
}

//-------------------------------------------------------------
// 프로그램 종료 버튼
// 모든 cv::Mat 메모리 해제 후 메인 윈도우 종료
//-------------------------------------------------------------
void CPCBdefectDlg::OnBnClickedClose()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	temple_Mat.release();
	test_Mat.release();
	m_processedTemplate.release();
	m_processedTest.release();
	boundingImg.release();
	delete m_pDetector;
	AfxGetMainWnd()->PostMessage(WM_CLOSE);
}


//-------------------------------------------------------------
// 확대 버튼 클릭 핸들러 - OnBnClickedExpand()
// YOLO 결함 검출 결과(boundingImg)를 1.5배 확대하여 별도 창으로 표시
// ESC키로 창 닫기 후 상태 메시지 업데이트
//-------------------------------------------------------------
void CPCBdefectDlg::OnBnClickedExpand()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// YOLO 처리 결과 이미지 존재 여부 검증
	// boundingImg.empty() == true면 처리된 바운딩박스 없음
	if (boundingImg.empty()) {
		SetDlgItemText(IDC_STATUS, _T("YOLO 처리된 이미지가 없습니다. \n"));
		return;  // 조기 종료
	}
	// 확대용 새 cv::Mat 생성 
	cv::Mat zoomed;
	// 1.5배 확대 리사이즈 (가로/세로 동시 확대)
	// cv::INTER_CUBIC: 부드러운 확대
	cv::resize(boundingImg, zoomed,
		cv::Size(boundingImg.cols * 1.5, boundingImg.rows * 1.5),  // 출력 크기
		0, 0,                                                    // x/y 축 스케일 팩터 (0=자동)
		cv::INTER_CUBIC);                                        // 보간 방법

	// 메인 윈도우를 데스크톱 중앙으로 이동 (확대창 배치 준비)
	CenterWindow(GetDesktopWindow());

	// OpenCV 고정 크기 윈도우 생성 (크기 자동 조정)
	cv::namedWindow("확대 이미지", cv::WINDOW_AUTOSIZE);

	// 확대된 바운딩박스 이미지 표시
	cv::imshow("확대 이미지", zoomed);

	// 키보드 입력 대기 (0=무한대기, ESC키 입력까지 블로킹)
	int key = cv::waitKey(0);

	// 모든 OpenCV 윈도우 파괴 (메모리 정리)
	cv::destroyAllWindows();

	// ESC키(27) 입력 확인 및 상태 메시지 업데이트
	if (key == 27) {
		SetDlgItemText(IDC_STATUS, _T("확대 이미지를 닫았습니다."));
	}
}


void CPCBdefectDlg::OnBnClickedBacth()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString templateName, testName;                    // 리스트박스에서 읽을 파일명 버퍼
	std::vector<cv::Mat> tempSave, testSave;           // 원본 RAW / YOLO 결과 이미지 저장소

	// ================================
	// 리스트박스 파일 확인
	// ================================
	if (curDir.GetCount() == 0) {
		SetDlgItemText(IDC_STATUS, L"폴더를 먼저 선택해 주세요\n");
		return;
	}
	// ================================
    // 배치 설정 및 진행 표시
    // ================================
	int nCount = curDir.GetCount();        // 리스트박스 총 파일 개수
	int pairCount = nCount / 2;            // template/test 쌍 개수 (짝수 가정)

	SetDlgItemText(IDC_STATUS, L"배치 처리 중...");

	// ================================
	// 핵심 배치 처리 루프 (PCB 결함 검출 파이프라인)
	// ================================
	for (int pair = 0; pair < pairCount; pair++) {
		int i = pair * 2;  // 리스트박스 인덱스: 0(template), 1(test), 2(template), 3(test)...

		// 파일명 추출 (짝수: template, 홀수: test)
		curDir.GetText(i, templateName);     // 템플릿 이미지 경로
		curDir.GetText(i + 1, testName);     // 테스트 이미지 경로

		// 원본 이미지 로드 (전처리 전 RAW 데이터)
		imgLoad(templateName);  // → temple_Mat (원본)
		imgLoad(testName);      // → test_Mat (원본)

		// 전처리 파이프라인 (크기조정, 정규화, 필터링 등)
		m_processedTemplate = m_preprocessor.all(temple_Mat);	// 템플릿 전처리 완료
		m_processedTest = m_preprocessor.all(test_Mat);         // 테스트 전처리 완료

		// YOLOv3 결함 검출 + 바운딩 박스 그리기
        // matchTemple(): 템플릿 비교 → YOLO 검출 → 바운딩 박스가 그려진 이미지 반환
		boundingImg = m_match.matchTemple(m_processedTemplate, m_processedTest);
		// 실시간 화면 표시(MFC Picture Control 갱신)
		DrawMat(boundingImg, m_pic_test);	

		// 결과 벡터 저장(PCB 분석용 데이터 보존)
		tempSave.push_back(temple_Mat.clone());
		testSave.push_back(boundingImg.clone());
	}
	// ================================
	// 저장 확인 대화상자
	// ================================
	int nMessage = AfxMessageBox(_T("파일을 저장 하시겠습니까\n"),MB_OKCANCEL,MB_ICONSTOP);
	if (nMessage == IDOK) {
		for (int i = 0; i < tempSave.size(); i++) {
			CString tempname, testname;

			// 연속 번호 파일명 생성 
			tempname.Format(L"%s\\temple_%04d.jpg", Out, i);           // 원본
			testname.Format(L"%s\\test_%04d.jpg", Out, i);             // YOLO 결과

			// CString → std::string 변환 후 저장
			std::string temp_path = std::string(CT2CA(tempname.operator LPCWSTR()));
			std::string test_path = std::string(CT2CA(testname.operator LPCWSTR()));

			cv::imwrite(temp_path, tempSave[i]);    // 원본 저장
			cv::imwrite(test_path, testSave[i]);    // YOLO 결과 저장

		}
		SetDlgItemText(IDC_STATUS, L"배치 처리 완료\n");
	}
	else {
		// ================================
		// 메모리 정리
		// ================================
		tempSave.clear();
		testSave.clear();
	}
}
