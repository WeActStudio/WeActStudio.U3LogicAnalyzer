@echo off  

cd /d %~dp0
  
set "startDir=."  
  
for /r "%startDir%" %%d in (.) do (  
    if /i "%%~nxd"=="__pycache__" (  
        echo Removing directory: "%%d"  
        rd /s /q "%%d"  
    )  
)  

for /r "%startDir%" %%f in (*.pyc) do (  
    echo Removing file: "%%f"  
    del "%%f"  
)  
  
echo Cleanup completed.  
pause