# ������ ��� ������ � cmake

```
msvc\CLogReader\CLogReader.sln
```

# �����

����� ��������� ����� ����� ��������� submodules � ������� ������ � ������� cmake

```
git clone --recursive https://github.com/btolfa/clogreader.git 
cd clogreader
mkdir build
cd build
cmake .. -G "Visual Studio 14 2015"
cmake --build .
ctest -C Debug
```