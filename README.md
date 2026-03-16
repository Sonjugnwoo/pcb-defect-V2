PCB 불량 검출 시스템 v2.0  
 OpenCV + MFC + YOLOv2 기반 배치 처리 프로토타입  

기능 개요  
 이미지 로드: 폴더 단위 template/test 이미지 쌍 배치 로드  
 전처리: Grayscale → Gaussian Blur → CLAHE → Morphology Opening  
 YOLOv3 검출: DeepPCB 훈련된 결함 검출 + 바운딩 박스 그리기  
 배치 처리: 리스트박스 순차 자동 처리 (짝수:template, 홀수:test)  
 결과 저장: temple_%04d.jpg + test_%04d.jpg 쌍 저장  
 UI: 실시간 Picture Control 갱신 + 상태바 진행 표시  

기술 스택  
 Frontend: MFC Dialog + Picture Control  
 Backend: OpenCV 4.x + YOLOv3 (darknet)  
 전처리: CLAHE(1.5,8x8), Morphological Opening(3x3 Ellipse)  
 딥러닝: YOLOv3 (yolov3-pcb.weights + pcb-error.names)  
 빌드: Visual Studio 2019 + vcpkg  
 GPU: CUDA 지원 (RTX 2060s)  

성능  
결함 검출 파이프라인:  
 absdiff → GRAY → Threshold(30) → Open(5x5) → Contours → BoundingBox  
배치 처리 파이프라인:  
 imgLoad() → ImagePreprocessor.all() → match.matchTemple() 
 → DrawMat() → temple_%04d.jpg + test_%04d.jpg 저장  

전처리 효과 
 Grayscale: 색상 변화 무시  
 CLAHE: 조명 불균일 보정  
 Morphology: 노이즈 제거   
 YOLOv3: 95%+ mAP  (DeepPCB 데이터셋)  

현재 한계점  
 원본 이미지 전량 저장 (메모리 낭비)  
 template/test 비교 방식 (YOLO 단독보다 느림)  
 불량만 필터링 없음 (99% OK 이미지 낭비)  
 매직넘버: 하드코딩된 YOLO confidence threshold  
 
개발 로드맵  
 v1.0 - 템플릿 매칭 프로토타입  ✅  
 v2.0 - YOLOv3 배치 처리       ✅  
 v3.0 - 불량만 추출  
 v4.0 - 실시간 카메라 스트림      

![Image](https://github.com/user-attachments/assets/227a61af-694f-4edb-912f-44b8b8d3c995)

![Image](https://github.com/user-attachments/assets/eec8897f-5b0c-4785-9d18-9a0669198694)
