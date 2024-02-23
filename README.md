# Enjoying graphs with Thomas H. Cormen

<p align="center">
  <img src="https://user-images.githubusercontent.com/71986226/154510384-f02decea-a93b-4cb4-9633-a6aa5c87d198.gif"/>
</p>

## How to install

```bash
git clone git@github.com:KetchuppOfficial/Graph_Course.git
cd Graph_Course
```

## How to build

### 0) Make sure you are in the root directory of the project (i.e. Graph_Course/)

### 1) Build the project

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## How to run unit tests

```bash
ctest --test-dir build
```
