@ECHO OFF

call ant.bat -Ddst=900i  -Dplat=doja3.5  -Dmax_res_len=20480 -Dgamen=qvga -Dbuildmode=RELEASE build
IF ERRORLEVEL 1 GOTO EXIT

call ant.bat -Ddst=505i  -Dplat=doja3    -Dmax_res_len=20480 -Dgamen=qvga -Dbuildmode=RELEASE build
IF ERRORLEVEL 1 GOTO EXIT

call ant.bat -Ddst=foma2 -Dplat=doja2    -Dmax_res_len=10240 -Dgamen=ovga -Dbuildmode=RELEASE build
IF ERRORLEVEL 1 GOTO EXIT

call ant.bat -Ddst=504i  -Dplat=doja2    -Dmax_res_len=10240 -Dgamen=ovga -Dbuildmode=RELEASE build
IF ERRORLEVEL 1 GOTO EXIT

:EXIT

