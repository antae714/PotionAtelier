@echo off
chcp 65001

echo [확인 단계] 삭제 대상 파일을 검색 중입니다...

:: Delayed Expansion 활성화
setlocal enabledelayedexpansion

:: 파일을 찾았는지 여부 확인
set found=0

:: 삭제할 파일 확장자 설정
set extension=.MaterialAsset

:: 삭제 대상 파일 검색 및 출력
for /r "%~dp0" %%f in (*%extension%) do (
    if exist "%%f" (
        echo 찾은 파일: "%%f"
        set found=1
    )
)

:: 파일을 찾지 못했을 경우 메시지 출력 후 종료
if !found! == 0 (
    echo 찾을 수 있는 삭제 대상 파일이 없습니다.
    pause
    exit /b
)

echo.
echo 삭제를 진행하시겠습니까? (Y/N)
choice /c YN /n /m "선택: "

if errorlevel 2 (
    echo 삭제를 취소했습니다.
    pause
    exit /b
)

echo 삭제를 진행합니다...

:: 실제 파일 삭제 작업 수행
for /r "%~dp0" %%f in (*%extension%) do (
    if exist "%%f" (
        echo 삭제할 파일: "%%f"
        del /f /q "%%f"
    )
)

echo 모든 지정된 파일이 삭제되었습니다.

pause
