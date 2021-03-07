@rem
@rem Copyright 2015 the original author or authors.
@rem
@rem Licensed under the Apache License, Version 2.0 (the "License");
@rem you may not use this file except in compliance with the License.
@rem You may obtain a copy of the License at
@rem
@rem      https://www.apache.org/licenses/LICENSE-2.0
@rem
@rem Unless required by applicable law or agreed to in writing, software
@rem distributed under the License is distributed on an "AS IS" BASIS,
@rem WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
@rem See the License for the specific language governing permissions and
@rem limitations under the License.
@rem

@if "%DEBUG%" == "" @echo off
@rem ##########################################################################
@rem
@rem  MiraiCP-loader-New startup script for Windows
@rem
@rem ##########################################################################

@rem Set local scope for the variables with windows NT shell
if "%OS%"=="Windows_NT" setlocal

set DIRNAME=%~dp0
if "%DIRNAME%" == "" set DIRNAME=.
set APP_BASE_NAME=%~n0
set APP_HOME=%DIRNAME%..

@rem Resolve any "." and ".." in APP_HOME to make it shorter.
for %%i in ("%APP_HOME%") do set APP_HOME=%%~fi

@rem Add default JVM options here. You can also use JAVA_OPTS and MIRAI_CP_LOADER_NEW_OPTS to pass JVM options to this script.
set DEFAULT_JVM_OPTS=

@rem Find java.exe
if defined JAVA_HOME goto findJavaFromJavaHome

set JAVA_EXE=java.exe
%JAVA_EXE% -version >NUL 2>&1
if "%ERRORLEVEL%" == "0" goto execute

echo.
echo ERROR: JAVA_HOME is not set and no 'java' command could be found in your PATH.
echo.
echo Please set the JAVA_HOME variable in your environment to match the
echo location of your Java installation.

goto fail

:findJavaFromJavaHome
set JAVA_HOME=%JAVA_HOME:"=%
set JAVA_EXE=%JAVA_HOME%/bin/java.exe

if exist "%JAVA_EXE%" goto execute

echo.
echo ERROR: JAVA_HOME is set to an invalid directory: %JAVA_HOME%
echo.
echo Please set the JAVA_HOME variable in your environment to match the
echo location of your Java installation.

goto fail

:execute
@rem Setup the command line

set CLASSPATH=%APP_HOME%\lib\MiraiCP-loader-New-0.0.1.jar;%APP_HOME%\lib\MiraiCP-Core.jar;%APP_HOME%\lib\mirai-core-jvm-2.4.0.jar;%APP_HOME%\lib\mirai-core-api-jvm-2.4.0.jar;%APP_HOME%\lib\mirai-core-utils-jvm-2.4.0.jar;%APP_HOME%\lib\kotlin-stdlib-jdk8-1.4.30.jar;%APP_HOME%\lib\jansi-1.18.jar;%APP_HOME%\lib\gson-2.8.6.jar;%APP_HOME%\lib\json-20201115.jar;%APP_HOME%\lib\ktor-client-okhttp-jvm-1.5.0.jar;%APP_HOME%\lib\ktor-client-core-jvm-1.5.0.jar;%APP_HOME%\lib\ktor-http-cio-jvm-1.5.0.jar;%APP_HOME%\lib\ktor-http-jvm-1.5.0.jar;%APP_HOME%\lib\ktor-network-jvm-1.5.0.jar;%APP_HOME%\lib\ktor-utils-jvm-1.5.0.jar;%APP_HOME%\lib\kotlin-stdlib-jdk7-1.4.30.jar;%APP_HOME%\lib\kotlin-test-junit5-1.4.21.jar;%APP_HOME%\lib\kotlin-test-1.4.21.jar;%APP_HOME%\lib\kotlin-reflect-1.4.21.jar;%APP_HOME%\lib\kotlin-serialization-1.4.21.jar;%APP_HOME%\lib\kotlin-gradle-plugin-api-1.4.21.jar;%APP_HOME%\lib\kotlinx-serialization-json-jvm-1.0.1.jar;%APP_HOME%\lib\kotlinx-serialization-protobuf-jvm-1.0.1.jar;%APP_HOME%\lib\kotlinx-serialization-core-jvm-1.0.1.jar;%APP_HOME%\lib\okhttp-4.6.0.jar;%APP_HOME%\lib\okio-jvm-2.6.0.jar;%APP_HOME%\lib\ktor-io-jvm-1.5.0.jar;%APP_HOME%\lib\kotlinx-coroutines-core-jvm-1.4.2-native-mt.jar;%APP_HOME%\lib\kotlin-stdlib-1.4.30.jar;%APP_HOME%\lib\kotlinx-coroutines-io-jvm-0.1.16.jar;%APP_HOME%\lib\kotlinx-io-jvm-0.1.16.jar;%APP_HOME%\lib\bcprov-jdk15on-1.64.jar;%APP_HOME%\lib\junit-jupiter-engine-5.2.0.jar;%APP_HOME%\lib\junit-jupiter-api-5.2.0.jar;%APP_HOME%\lib\kotlinx-coroutines-io-0.1.16.jar;%APP_HOME%\lib\atomicfu-jvm-0.14.4.jar;%APP_HOME%\lib\kotlinx-io-0.1.16.jar;%APP_HOME%\lib\kotlin-test-annotations-common-1.4.21.jar;%APP_HOME%\lib\kotlin-test-common-1.4.21.jar;%APP_HOME%\lib\kotlin-stdlib-common-1.4.30.jar;%APP_HOME%\lib\annotations-19.0.0.jar;%APP_HOME%\lib\atomicfu-common-0.14.1.jar;%APP_HOME%\lib\junit-platform-engine-1.2.0.jar;%APP_HOME%\lib\junit-platform-commons-1.2.0.jar;%APP_HOME%\lib\apiguardian-api-1.0.0.jar;%APP_HOME%\lib\opentest4j-1.1.0.jar;%APP_HOME%\lib\kotlin-jvm-blocking-bridge-jvm-1.10.0-dev-1.jar;%APP_HOME%\lib\slf4j-api-1.7.30.jar


@rem Execute MiraiCP-loader-New
"%JAVA_EXE%" %DEFAULT_JVM_OPTS% %JAVA_OPTS% %MIRAI_CP_LOADER_NEW_OPTS%  -classpath "%CLASSPATH%" org.example.mirai.plugin.MainKt %*

:end
@rem End local scope for the variables with windows NT shell
if "%ERRORLEVEL%"=="0" goto mainEnd

:fail
rem Set variable MIRAI_CP_LOADER_NEW_EXIT_CONSOLE if you need the _script_ return code instead of
rem the _cmd.exe /c_ return code!
if  not "" == "%MIRAI_CP_LOADER_NEW_EXIT_CONSOLE%" exit 1
exit /b 1

:mainEnd
if "%OS%"=="Windows_NT" endlocal

:omega
